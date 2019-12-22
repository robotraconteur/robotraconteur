//This file is automatically generated. DO NOT EDIT!

#include <RobotRaconteur.h>
#include <boost/signals2.hpp>
#include <boost/array.hpp>
#include <boost/container/static_vector.hpp>
#include "com__robotraconteur__testing__TestService2.h"
#pragma once

namespace com
{
namespace robotraconteur
{
namespace testing
{
namespace TestService1
{

class teststruct1;
class teststruct2;
class testroot;
class sub1;
class sub2;
class sub3;

namespace com__robotraconteur__testing__TestService1Constants 
{
    namespace testroot
    {
    static const double doubleconst[]={3.4, 4.8, 14372.8};
    static const char* strconst="This is a constant";
    }
}

class teststruct1 : public RobotRaconteur::RRStructure {
public:
RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > dat1;
std::string str2;
RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<char>  > > vec3;
RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRArray<char>  > > dict4;
RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<char>  > > list5;
RR_INTRUSIVE_PTR<teststruct2 > struct1;
RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,teststruct2  > > dstruct2;
RR_INTRUSIVE_PTR<RobotRaconteur::RRList<teststruct2  > > lstruct3;
RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double > > multidimarray;
RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> var3;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService1.teststruct1";  }
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using teststruct1Ptr = RR_INTRUSIVE_PTR<teststruct1>;
#endif

class teststruct2 : public RobotRaconteur::RRStructure {
public:
RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > mydat;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService1.teststruct2";  }
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using teststruct2Ptr = RR_INTRUSIVE_PTR<teststruct2>;
#endif

class testroot : public virtual com::robotraconteur::testing::TestService2::baseobj
{
public:
virtual double get_d1()=0;
virtual void set_d1(double value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > get_d2()=0;
virtual void set_d2(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > get_d3()=0;
virtual void set_d3(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > get_d4()=0;
virtual void set_d4(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double > > get_d5()=0;
virtual void set_d5(RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double > > get_d6()=0;
virtual void set_d6(RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double > > value)=0;

virtual float get_s1()=0;
virtual void set_s1(float value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<float > > get_s2()=0;
virtual void set_s2(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<float > > value)=0;

virtual int8_t get_i8_1()=0;
virtual void set_i8_1(int8_t value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int8_t > > get_i8_2()=0;
virtual void set_i8_2(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int8_t > > value)=0;

virtual uint8_t get_u8_1()=0;
virtual void set_u8_1(uint8_t value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t > > get_u8_2()=0;
virtual void set_u8_2(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<uint8_t > > get_u8_3()=0;
virtual void set_u8_3(RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<uint8_t > > value)=0;

virtual int16_t get_i16_1()=0;
virtual void set_i16_1(int16_t value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int16_t > > get_i16_2()=0;
virtual void set_i16_2(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int16_t > > value)=0;

virtual uint16_t get_u16_1()=0;
virtual void set_u16_1(uint16_t value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint16_t > > get_u16_2()=0;
virtual void set_u16_2(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint16_t > > value)=0;

virtual int32_t get_i32_1()=0;
virtual void set_i32_1(int32_t value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > get_i32_2()=0;
virtual void set_i32_2(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > get_i32_huge()=0;
virtual void set_i32_huge(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > value)=0;

virtual uint32_t get_u32_1()=0;
virtual void set_u32_1(uint32_t value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t > > get_u32_2()=0;
virtual void set_u32_2(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t > > value)=0;

virtual int64_t get_i64_1()=0;
virtual void set_i64_1(int64_t value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int64_t > > get_i64_2()=0;
virtual void set_i64_2(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int64_t > > value)=0;

virtual uint64_t get_u64_1()=0;
virtual void set_u64_1(uint64_t value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint64_t > > get_u64_2()=0;
virtual void set_u64_2(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint64_t > > value)=0;

virtual std::string get_str1()=0;
virtual void set_str1(const std::string& value)=0;

virtual RR_INTRUSIVE_PTR<teststruct1 > get_struct1()=0;
virtual void set_struct1(RR_INTRUSIVE_PTR<teststruct1 > value)=0;

virtual RR_INTRUSIVE_PTR<teststruct2 > get_struct2()=0;
virtual void set_struct2(RR_INTRUSIVE_PTR<teststruct2 > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<double >  > > get_is_d1()=0;
virtual void set_is_d1(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<double >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRArray<double >  > > get_is_d2()=0;
virtual void set_is_d2(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRArray<double >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<double >  > > get_is_d3()=0;
virtual void set_is_d3(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<double >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRArray<double >  > > get_is_d4()=0;
virtual void set_is_d4(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRArray<double >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRMultiDimArray<double >  > > get_is_d5()=0;
virtual void set_is_d5(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRMultiDimArray<double >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRMultiDimArray<double >  > > get_is_d6()=0;
virtual void set_is_d6(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRMultiDimArray<double >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<char>  > > get_is_str1()=0;
virtual void set_is_str1(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<char>  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRArray<char>  > > get_is_str2()=0;
virtual void set_is_str2(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRArray<char>  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,teststruct2  > > get_is_struct1()=0;
virtual void set_is_struct1(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,teststruct2  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,teststruct2  > > get_is_struct2()=0;
virtual void set_is_struct2(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,teststruct2  > > value)=0;

virtual RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService2::ostruct2 > get_struct3()=0;
virtual void set_struct3(RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService2::ostruct2 > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double >  > > get_list_d1()=0;
virtual void set_list_d1(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double >  > > get_list_d3()=0;
virtual void set_list_d3(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<double >  > > get_list_d5()=0;
virtual void set_list_d5(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<double >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<char>  > > get_list_str1()=0;
virtual void set_list_str1(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<char>  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<teststruct2  > > get_list_struct1()=0;
virtual void set_list_struct1(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<teststruct2  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var1()=0;
virtual void set_var1(RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRValue > > get_var2()=0;
virtual void set_var2(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRValue > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var_num()=0;
virtual void set_var_num(RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var_str()=0;
virtual void set_var_str(RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var_struct()=0;
virtual void set_var_struct(RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var_vector()=0;
virtual void set_var_vector(RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var_dictionary()=0;
virtual void set_var_dictionary(RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var_list()=0;
virtual void set_var_list(RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var_multidimarray()=0;
virtual void set_var_multidimarray(RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> value)=0;

virtual double get_errtest()=0;
virtual void set_errtest(double value)=0;

virtual RR_INTRUSIVE_PTR<teststruct1 > get_nulltest()=0;
virtual void set_nulltest(RR_INTRUSIVE_PTR<teststruct1 > value)=0;

virtual void func1()=0;

virtual void func2(double d1, double d2)=0;

virtual double func3(double d1, double d2)=0;

virtual int32_t meaning_of_life()=0;

virtual void func_errtest()=0;

virtual void func_errtest1()=0;

virtual void func_errtest2()=0;

virtual void func_errtest3()=0;

virtual void o6_op(int32_t op)=0;

virtual void pipe_check_error()=0;

virtual void test_callbacks()=0;

virtual boost::signals2::signal<void ()>& get_ev1()=0;

virtual boost::signals2::signal<void (double, RR_INTRUSIVE_PTR<teststruct2 >)>& get_ev2()=0;

virtual RR_SHARED_PTR<sub1 > get_o1()=0;

virtual RR_SHARED_PTR<sub1 > get_o2(int32_t ind)=0;

virtual RR_SHARED_PTR<sub1 > get_o3(int32_t ind)=0;

virtual RR_SHARED_PTR<sub1 > get_o4(const std::string& ind)=0;

virtual RR_SHARED_PTR<com::robotraconteur::testing::TestService2::subobj > get_o5()=0;

virtual RR_SHARED_PTR<RobotRaconteur::RRObject > get_o6()=0;

virtual RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > > > get_p1()=0;
virtual void set_p1(RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<teststruct2 > > > get_p2()=0;
virtual void set_p2(RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<teststruct2 > > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Pipe<double > > get_broadcastpipe()=0;
virtual void set_broadcastpipe(RR_SHARED_PTR<RobotRaconteur::Pipe<double > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void() > > > get_cb1()=0;
virtual void set_cb1(RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void()> > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void(double, double) > > > get_cb2()=0;
virtual void set_cb2(RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void(double, double)> > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<double(double, double) > > > get_cb3()=0;
virtual void set_cb3(RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<double(double, double)> > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<int32_t() > > > get_cb_meaning_of_life()=0;
virtual void set_cb_meaning_of_life(RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<int32_t()> > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void() > > > get_cb_errtest()=0;
virtual void set_cb_errtest(RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void()> > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > > > get_w1()=0;
virtual void set_w1(RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<teststruct2 > > > get_w2()=0;
virtual void set_w2(RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<teststruct2 > > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > > get_w3()=0;
virtual void set_w3(RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Wire<double > > get_broadcastwire()=0;
virtual void set_broadcastwire(RR_SHARED_PTR<RobotRaconteur::Wire<double > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::ArrayMemory<double > > get_m1()=0;

virtual RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<double > > get_m2()=0;

virtual RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<uint8_t > > get_m3()=0;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService1.testroot";  }
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using testrootPtr = RR_SHARED_PTR<testroot>;
#endif

class sub1 : public virtual RobotRaconteur::RRObject
{
public:
virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > get_d1()=0;
virtual void set_d1(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double > > get_d2()=0;
virtual void set_d2(RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double > > value)=0;

virtual std::string get_s_ind()=0;
virtual void set_s_ind(const std::string& value)=0;

virtual int32_t get_i_ind()=0;
virtual void set_i_ind(int32_t value)=0;

virtual RR_SHARED_PTR<sub2 > get_o2_1()=0;

virtual RR_SHARED_PTR<sub2 > get_o2_2(int32_t ind)=0;

virtual RR_SHARED_PTR<sub2 > get_o2_3(const std::string& ind)=0;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService1.sub1";  }
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using sub1Ptr = RR_SHARED_PTR<sub1>;
#endif

class sub2 : public virtual RobotRaconteur::RRObject
{
public:
virtual std::string get_s_ind()=0;
virtual void set_s_ind(const std::string& value)=0;

virtual int32_t get_i_ind()=0;
virtual void set_i_ind(int32_t value)=0;

virtual std::string get_data()=0;
virtual void set_data(const std::string& value)=0;

virtual RR_SHARED_PTR<sub3 > get_o3_1(const std::string& ind)=0;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService1.sub2";  }
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using sub2Ptr = RR_SHARED_PTR<sub2>;
#endif

class sub3 : public virtual RobotRaconteur::RRObject
{
public:
virtual std::string get_ind()=0;
virtual void set_ind(const std::string& value)=0;

virtual std::string get_data2()=0;
virtual void set_data2(const std::string& value)=0;

virtual double get_data3()=0;
virtual void set_data3(double value)=0;

virtual double add(double d)=0;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService1.sub3";  }
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using sub3Ptr = RR_SHARED_PTR<sub3>;
#endif

class testexception1 : public RobotRaconteur::RobotRaconteurRemoteException
{
    public:
    testexception1(const std::string& message, std::string sub_name = "", RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> param_ = RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>()) : RobotRaconteur::RobotRaconteurRemoteException("com.robotraconteur.testing.TestService1.testexception1",message,sub_name,param_) {}
};
#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using testexception1Ptr = RR_SHARED_PTR<testexception1>;
#endif
class testexception2 : public RobotRaconteur::RobotRaconteurRemoteException
{
    public:
    testexception2(const std::string& message, std::string sub_name = "", RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> param_ = RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>()) : RobotRaconteur::RobotRaconteurRemoteException("com.robotraconteur.testing.TestService1.testexception2",message,sub_name,param_) {}
};
#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using testexception2Ptr = RR_SHARED_PTR<testexception2>;
#endif
}
}
}
}

