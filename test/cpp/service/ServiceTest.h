#include "com__robotraconteur__testing__TestService1.h"
#include "com__robotraconteur__testing__TestService1_stubskel.h"

#include "CompareArray.h"

#pragma once

using namespace RobotRaconteur;
using namespace com::robotraconteur::testing::TestService1;

namespace RobotRaconteurTest
{

class sub1_impl;
class sub2_impl;
class sub3_impl;
class TestService2SubObj;
class RobotRaconteurTest_testroot;

class RobotRaconteurTestServiceSupport
{
public:
	RR_SHARED_PTR<RobotRaconteurTest_testroot> testservice;
	RR_SHARED_PTR<RobotRaconteurTest_testroot> testservice_auth;

	void RegisterServices(RR_SHARED_PTR<TcpTransport> transport, RR_SHARED_PTR<RobotRaconteurNode> node=RR_SHARED_PTR<RobotRaconteurNode>());
	void RegisterSecureServices(RR_SHARED_PTR<TcpTransport> tcptransport, const std::vector<NodeID>& allowed_clients);

	void UnregisterServices();



};



class RobotRaconteurTest_testroot : public virtual testroot, public RR_ENABLE_SHARED_FROM_THIS<RobotRaconteurTest_testroot>
{
public:

	RobotRaconteurTest_testroot(RR_SHARED_PTR<TcpTransport> tcptransport);

	RR_SHARED_PTR<TcpTransport> tcptransport;

RR_OVIRTUAL double get_d1() RR_OVERRIDE ;
RR_OVIRTUAL void set_d1(double value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > get_d2() RR_OVERRIDE ;
RR_OVIRTUAL void set_d2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > get_d3() RR_OVERRIDE ;
RR_OVIRTUAL void set_d3(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > get_d4() RR_OVERRIDE ;
RR_OVIRTUAL void set_d4(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double> > get_d5() RR_OVERRIDE ;
RR_OVIRTUAL void set_d5(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double> >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double> > get_d6() RR_OVERRIDE ;
RR_OVIRTUAL void set_d6(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double> >& value) RR_OVERRIDE ;

RR_OVIRTUAL float get_s1() RR_OVERRIDE ;
RR_OVIRTUAL void set_s1(float value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<float> > get_s2() RR_OVERRIDE ;
RR_OVIRTUAL void set_s2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<float> >& value) RR_OVERRIDE ;

RR_OVIRTUAL int8_t get_i8_1() RR_OVERRIDE ;
RR_OVIRTUAL void set_i8_1(int8_t value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int8_t> > get_i8_2() RR_OVERRIDE ;
RR_OVIRTUAL void set_i8_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int8_t> >& value) RR_OVERRIDE ;

RR_OVIRTUAL uint8_t get_u8_1() RR_OVERRIDE ;
RR_OVIRTUAL void set_u8_1(uint8_t value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> > get_u8_2() RR_OVERRIDE ;
RR_OVIRTUAL void set_u8_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<uint8_t> > get_u8_3() RR_OVERRIDE ;
RR_OVIRTUAL void set_u8_3(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<uint8_t> >& value) RR_OVERRIDE ;

RR_OVIRTUAL int16_t get_i16_1() RR_OVERRIDE ;
RR_OVIRTUAL void set_i16_1(int16_t value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int16_t> > get_i16_2() RR_OVERRIDE ;
RR_OVIRTUAL void set_i16_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int16_t> >& value) RR_OVERRIDE ;

RR_OVIRTUAL uint16_t get_u16_1() RR_OVERRIDE ;
RR_OVIRTUAL void set_u16_1(uint16_t value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint16_t> > get_u16_2() RR_OVERRIDE ;
RR_OVIRTUAL void set_u16_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint16_t> >& value) RR_OVERRIDE ;

RR_OVIRTUAL int32_t get_i32_1() RR_OVERRIDE ;
RR_OVIRTUAL void set_i32_1(int32_t value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > get_i32_2() RR_OVERRIDE ;
RR_OVIRTUAL void set_i32_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > get_i32_huge() RR_OVERRIDE ;
RR_OVIRTUAL void set_i32_huge(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> >& value) RR_OVERRIDE ;

RR_OVIRTUAL uint32_t get_u32_1() RR_OVERRIDE ;
RR_OVIRTUAL void set_u32_1(uint32_t value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > get_u32_2() RR_OVERRIDE ;
RR_OVIRTUAL void set_u32_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> >& value) RR_OVERRIDE ;

RR_OVIRTUAL int64_t get_i64_1() RR_OVERRIDE ;
RR_OVIRTUAL void set_i64_1(int64_t value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int64_t> > get_i64_2() RR_OVERRIDE ;
RR_OVIRTUAL void set_i64_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int64_t> >& value) RR_OVERRIDE ;

RR_OVIRTUAL uint64_t get_u64_1() RR_OVERRIDE ;
RR_OVIRTUAL void set_u64_1(uint64_t value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint64_t> > get_u64_2() RR_OVERRIDE ;
RR_OVIRTUAL void set_u64_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint64_t> >& value) RR_OVERRIDE ;

RR_OVIRTUAL std::string get_str1() RR_OVERRIDE ;
RR_OVIRTUAL void set_str1(const std::string& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<teststruct1> get_struct1() RR_OVERRIDE ;
RR_OVIRTUAL void set_struct1(const RR_INTRUSIVE_PTR<teststruct1>& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<teststruct2> get_struct2() RR_OVERRIDE ;
RR_OVIRTUAL void set_struct2(const RR_INTRUSIVE_PTR<teststruct2>& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<double> > > get_is_d1() RR_OVERRIDE ;
RR_OVIRTUAL void set_is_d1(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<double> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRArray<double> > > get_is_d2() RR_OVERRIDE ;
RR_OVIRTUAL void set_is_d2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRArray<double> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<double> > > get_is_d3() RR_OVERRIDE ;
RR_OVIRTUAL void set_is_d3(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<double> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRArray<double> > > get_is_d4() RR_OVERRIDE ;
RR_OVIRTUAL void set_is_d4(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRArray<double> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRMultiDimArray<double> > > get_is_d5() RR_OVERRIDE ;
RR_OVIRTUAL void set_is_d5(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRMultiDimArray<double> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRMultiDimArray<double> > > get_is_d6() RR_OVERRIDE ;
RR_OVIRTUAL void set_is_d6(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRMultiDimArray<double> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<char> > > get_is_str1() RR_OVERRIDE ;
RR_OVIRTUAL void set_is_str1(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<char> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRArray<char> > > get_is_str2() RR_OVERRIDE ;
RR_OVIRTUAL void set_is_str2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRArray<char> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,teststruct2> > get_is_struct1() RR_OVERRIDE ;
RR_OVIRTUAL void set_is_struct1(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,teststruct2> >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,teststruct2> > get_is_struct2() RR_OVERRIDE ;
RR_OVIRTUAL void set_is_struct2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string,teststruct2> >& value) RR_OVERRIDE ;

//Lists

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double> > > get_list_d1() RR_OVERRIDE ;
RR_OVIRTUAL void set_list_d1(const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double> > > get_list_d3() RR_OVERRIDE ;
RR_OVIRTUAL void set_list_d3(const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<double> > > get_list_d5() RR_OVERRIDE ;
RR_OVIRTUAL void set_list_d5(const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<double> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<char> > > get_list_str1() RR_OVERRIDE ;
RR_OVIRTUAL void set_list_str1(const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<char> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRList<teststruct2> > get_list_struct1() RR_OVERRIDE ;
RR_OVIRTUAL void set_list_struct1(const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<teststruct2> >& value) RR_OVERRIDE ;

//End Lists

RR_OVIRTUAL RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService2::ostruct2> get_struct3() RR_OVERRIDE ;
RR_OVIRTUAL void set_struct3(const RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService2::ostruct2>& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var1() RR_OVERRIDE ;
RR_OVIRTUAL void set_var1(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRValue> > get_var2() RR_OVERRIDE ;
RR_OVIRTUAL void set_var2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRValue> >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var_num() RR_OVERRIDE ;
RR_OVIRTUAL void set_var_num(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var_str() RR_OVERRIDE ;
RR_OVIRTUAL void set_var_str(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var_struct() RR_OVERRIDE ;
RR_OVIRTUAL void set_var_struct(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var_vector() RR_OVERRIDE ;
RR_OVIRTUAL void set_var_vector(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var_dictionary() RR_OVERRIDE ;
RR_OVIRTUAL void set_var_dictionary(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var_list() RR_OVERRIDE ;
RR_OVIRTUAL void set_var_list(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> get_var_multidimarray() RR_OVERRIDE ;
RR_OVIRTUAL void set_var_multidimarray(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value) RR_OVERRIDE ;

RR_OVIRTUAL double get_errtest() RR_OVERRIDE ;
RR_OVIRTUAL void set_errtest(double value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<teststruct1> get_nulltest() RR_OVERRIDE ;
RR_OVIRTUAL void set_nulltest(const RR_INTRUSIVE_PTR<teststruct1>& value) RR_OVERRIDE ;

RR_OVIRTUAL void func1() RR_OVERRIDE ;

void func1_thread();

RR_OVIRTUAL void func2(double d1, double d2) RR_OVERRIDE ;

void func2_thread(double d1, double d2);

RR_OVIRTUAL double func3(double d1, double d2) RR_OVERRIDE ;

RR_OVIRTUAL int32_t meaning_of_life() RR_OVERRIDE ;

RR_OVIRTUAL void func_errtest() RR_OVERRIDE ;

RR_OVIRTUAL void func_errtest1() RR_OVERRIDE ;

RR_OVIRTUAL void func_errtest2() RR_OVERRIDE ;

RR_OVIRTUAL void func_errtest3() RR_OVERRIDE ;

RR_OVIRTUAL void o6_op(int32_t op) RR_OVERRIDE ;

RR_OVIRTUAL void pipe_check_error() RR_OVERRIDE ;

RR_OVIRTUAL void test_callbacks() RR_OVERRIDE ;

RR_OVIRTUAL boost::signals2::signal<void()>& get_ev1() RR_OVERRIDE ;

RR_OVIRTUAL boost::signals2::signal<void(double,RR_INTRUSIVE_PTR<teststruct2>)>& get_ev2() RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<sub1> get_o1() RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<sub1> get_o2(int32_t ind) RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<sub1> get_o3(int32_t ind) RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<sub1> get_o4(const std::string& ind) RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<com::robotraconteur::testing::TestService2::subobj> get_o5() RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::RRObject> get_o6() RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > > > get_p1() RR_OVERRIDE ;
RR_OVIRTUAL void set_p1(const RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > > >& value) RR_OVERRIDE ;

void p1_connect_callback(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > p);
void p1_packet_received(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > p);
void p1_packet_received_threadfunc(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > p);
void p1_packet_ack_received(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > p, uint32_t packetnum);

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<teststruct2> > > get_p2() RR_OVERRIDE ;
RR_OVIRTUAL void set_p2(const RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<teststruct2> > >& value) RR_OVERRIDE ;

void p2_connect_callback(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<teststruct2> > > p);
void p2_packet_received(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<teststruct2> > > p);
void p2_packet_received_threadfunc(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<teststruct2> > > p);

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Pipe<double > > get_broadcastpipe() RR_OVERRIDE ;
RR_OVIRTUAL void set_broadcastpipe(const RR_SHARED_PTR<RobotRaconteur::Pipe<double > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void()> > > get_cb1() RR_OVERRIDE ;
RR_OVIRTUAL void set_cb1(const RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void()> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void(double,double)> > > get_cb2() RR_OVERRIDE ;
RR_OVIRTUAL void set_cb2(const RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void(double,double)> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<double(double,double)> > > get_cb3() RR_OVERRIDE ;
RR_OVIRTUAL void set_cb3(const RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<double(double,double)> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<int32_t()> > > get_cb_meaning_of_life() RR_OVERRIDE ;
RR_OVIRTUAL void set_cb_meaning_of_life(const RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<int32_t()> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void()> > > get_cb_errtest() RR_OVERRIDE ;
RR_OVIRTUAL void set_cb_errtest(const RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void()> > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > > > get_w1() RR_OVERRIDE ;
RR_OVIRTUAL void set_w1(const RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > > >& value) RR_OVERRIDE ;

void w1_connect_callback(RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRArray<double> > > > wire);
void w1_value_changed(RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRArray<double> > > > wire, RR_INTRUSIVE_PTR<RRArray<double> > value, TimeSpec time);

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<teststruct2> > > get_w2() RR_OVERRIDE ;
RR_OVIRTUAL void set_w2(const RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<teststruct2> > >& value) RR_OVERRIDE ;

void w2_connect_callback(RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<teststruct2> > > wire);
void w2_value_changed(RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<teststruct2> > > wire, RR_INTRUSIVE_PTR<teststruct2> value, TimeSpec time);

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t> > > > get_w3() RR_OVERRIDE ;
RR_OVIRTUAL void set_w3(const RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t> > > >& value) RR_OVERRIDE ;

void w3_connect_callback(RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > > > wire);
void w3_value_changed(RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > > > wire, RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > value, TimeSpec time);

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Wire<double > > get_broadcastwire() RR_OVERRIDE ;
RR_OVIRTUAL void set_broadcastwire(const RR_SHARED_PTR<RobotRaconteur::Wire<double > >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::ArrayMemory<double> > get_m1() RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<double> > get_m2() RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<uint8_t> > get_m3() RR_OVERRIDE ;


//member variables
RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> var1;
RR_INTRUSIVE_PTR<RRMap<int32_t,RobotRaconteur::RRValue> > var2;

boost::signals2::signal<void()> ev1;

boost::signals2::signal<void(double,RR_INTRUSIVE_PTR<teststruct2>)> ev2;

RR_SHARED_PTR<sub1_impl> o1;
std::map<int32_t,RR_SHARED_PTR<sub1_impl> > o2;
std::map<int32_t,RR_SHARED_PTR<sub1_impl> > o3;
std::map<std::string,RR_SHARED_PTR<sub1_impl> > o4;
RR_SHARED_PTR<TestService2SubObj> o5;
RR_SHARED_PTR<RRObject> o6;

boost::recursive_mutex o2_lock;
boost::recursive_mutex o3_lock;

boost::recursive_mutex o4_lock;

boost::mutex o6_lock;

//pipe servers
RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > > > p1;
boost::recursive_mutex p1_lock;
uint32_t packetnum;
bool packet_sent;
bool ack_recv;

RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<teststruct2> > > p2;
boost::recursive_mutex p2_lock;

RR_SHARED_PTR<RobotRaconteur::Pipe<double > > broadcastpipe_pipe;
RR_SHARED_PTR<RobotRaconteur::PipeBroadcaster<double > > broadcastpipe;
boost::mutex broadcastpipe_lock;
RR_SHARED_PTR<Timer> broadcastpipe_timer;

static void broadcastpipe_timer_handler(RR_WEAK_PTR<RobotRaconteurTest_testroot> obj, const TimerEvent& ev);

//Callback
RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void()> > > cb1;
RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void(double,double)> > > cb2;
RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<double(double,double)> > > cb3;
RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<int32_t()> > > cb_mol;
RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void()> > > cb_err;

//wires
RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > > > w1;
RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<teststruct2> > > w2;
RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t> > > > w3;

RR_SHARED_PTR<RobotRaconteur::Wire<double > > broadcastwire_wire;
RR_SHARED_PTR<RobotRaconteur::WireBroadcaster<double > > broadcastwire;
boost::mutex broadcastwire_lock;
RR_SHARED_PTR<Timer> broadcastwire_timer;

static void broadcastwire_timer_handler(RR_WEAK_PTR<RobotRaconteurTest_testroot> obj, const TimerEvent& ev);

RR_SHARED_PTR<ArrayMemory<double> > m1;
RR_SHARED_PTR<MultiDimArrayMemory<double> > m2;
RR_SHARED_PTR<MultiDimArrayMemory<uint8_t> > m3;

void Shutdown();

};

class sub1_impl : public virtual sub1 ,public virtual IRobotRaconteurMonitorObject
{
public:
	sub1_impl();

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > get_d1() RR_OVERRIDE ;
RR_OVIRTUAL void set_d1(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> >& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double> > get_d2() RR_OVERRIDE ;
RR_OVIRTUAL void set_d2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double> >& value) RR_OVERRIDE ;

RR_OVIRTUAL std::string get_s_ind() RR_OVERRIDE ;
RR_OVIRTUAL void set_s_ind(const std::string& value) RR_OVERRIDE ;

RR_OVIRTUAL int32_t get_i_ind() RR_OVERRIDE ;
RR_OVIRTUAL void set_i_ind(int32_t value) RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<sub2> get_o2_1() RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<sub2> get_o2_2(int32_t ind) RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<sub2> get_o2_3(const std::string& ind) RR_OVERRIDE ;

RR_INTRUSIVE_PTR<RRArray<double> > d1;

RR_INTRUSIVE_PTR<RRMultiDimArray<double> > d2;

std::string s_ind;
int32_t i_ind;
RR_SHARED_PTR<sub2> o2_1;
std::map<int32_t,RR_SHARED_PTR<sub2_impl> > o2_2;
std::map<std::string,RR_SHARED_PTR<sub2_impl> > o2_3;

RR_OVIRTUAL void RobotRaconteurMonitorEnter() RR_OVERRIDE ;

RR_OVIRTUAL void RobotRaconteurMonitorEnter(int32_t timeout) RR_OVERRIDE ;

RR_OVIRTUAL void RobotRaconteurMonitorExit() RR_OVERRIDE ;

boost::recursive_timed_mutex rrlock;


};

class sub2_impl : public virtual sub2
{
public:
RR_OVIRTUAL std::string get_s_ind() RR_OVERRIDE ;
RR_OVIRTUAL void set_s_ind(const std::string& value) RR_OVERRIDE ;

RR_OVIRTUAL int32_t get_i_ind() RR_OVERRIDE ;
RR_OVIRTUAL void set_i_ind(int32_t value) RR_OVERRIDE ;

RR_OVIRTUAL std::string get_data() RR_OVERRIDE ;
RR_OVIRTUAL void set_data(const std::string& value) RR_OVERRIDE ;

RR_OVIRTUAL RR_SHARED_PTR<sub3> get_o3_1(const std::string& ind) RR_OVERRIDE ;

std::string s_ind;
int32_t i_ind;
std::string data;
std::map<std::string,RR_SHARED_PTR<sub3_impl> > o3_1;

};

class sub3_impl : public virtual sub3, public virtual async_sub3
{
public:
RR_OVIRTUAL std::string get_ind() RR_OVERRIDE ;
RR_OVIRTUAL void set_ind(const std::string& value) RR_OVERRIDE ;

RR_OVIRTUAL std::string get_data2() RR_OVERRIDE ;
RR_OVIRTUAL void set_data2(const std::string& value) RR_OVERRIDE ;

RR_OVIRTUAL double get_data3() RR_OVERRIDE ;
RR_OVIRTUAL void set_data3(double value) RR_OVERRIDE ;

RR_OVIRTUAL double add(double d) RR_OVERRIDE ;

RR_OVIRTUAL void async_get_ind(boost::function<void (const std::string&,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;
RR_OVIRTUAL void async_set_ind(const std::string& value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

RR_OVIRTUAL void async_get_data2(boost::function<void (const std::string&,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;
RR_OVIRTUAL void async_set_data2(const std::string& value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

RR_OVIRTUAL void async_get_data3(boost::function<void (double,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;
RR_OVIRTUAL void async_set_data3(double value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

RR_OVIRTUAL void async_add(double d,boost::function<void (double, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;


std::string ind;
std::string data2;
double data3;

};

class TestService2SubObj : public com::robotraconteur::testing::TestService2::subobj
{
public:

	RR_OVIRTUAL double add_val(double v) RR_OVERRIDE ;

};


}
