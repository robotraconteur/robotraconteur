#include <RobotRaconteur.h>
#include "com__robotraconteur__testing__TestService1.h"
#include "com__robotraconteur__testing__TestService1_stubskel.h"

#include "CompareArray.h"
#include "array_compare.h"

#pragma once

using namespace std;
using namespace RobotRaconteur;
using namespace com::robotraconteur::testing::TestService1;
using namespace com::robotraconteur::testing::TestService2;


#define ASYNC_TEST_CALL(command) \
	if (exp) \
	{\
		TestAsync_err(exp);\
		return;\
	}\
	try\
	{\
	command;\
	}\
	catch (std::exception& ee)\
	{\
	FAIL() << ee.what(); \
	TestAsync_err(RR_MAKE_SHARED<UnknownException>("ERR","Err occurred"));\
	}\


namespace RobotRaconteurTest
{
	class ServiceTestClient
	{
	public:	
		void ConnectService(string url);

		void RunFullTest(string url, string authurl);
		void RunMinimalTest(string url);
		void RunSingleThreadTest(string url, RR_BOOST_ASIO_IO_CONTEXT& io_context);

		
		void TestProperties();

		void TestFunctions();

		void TestEvents();

		void TestObjRefs();

		void ev1_cb();

		void ev2_cb(double d, RR_INTRUSIVE_PTR<teststruct2> s);


		void TestPipes();

		void ee1_cb(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > p);
		void ee1_ack_cb(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > p, uint32_t packetnum);
		void ee2_cb(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > p);
		void ee3_cb(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<teststruct2> > > p);

		AutoResetEvent ee1;
		AutoResetEvent ee2;
		AutoResetEvent ee3;

		uint32_t packetnum;
		bool ack_recv;


		void TestCallbacks();

		void cb1_func();

		void cb2_func(double d1, double d2);

		double cb3_func(double d1, double d2); 

		int cb_meaning_of_life_func();

		void cb_errtest();


		void TestWires();

		void w1_changed(RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRArray<double> > > > c, RR_INTRUSIVE_PTR<RRArray<double> > value, TimeSpec t);
		void w2_changed(RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<teststruct2> > > c, RR_INTRUSIVE_PTR<teststruct2> value, TimeSpec t);
		void w3_changed(RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > > > c, RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > value, TimeSpec t);

		bool w1_called;
		bool w2_called;
		bool w3_called;

		AutoResetEvent we1;
		AutoResetEvent we2;
		AutoResetEvent we3;

		
		RR_SHARED_PTR<testroot> r;

		AutoResetEvent ev1_event;
		AutoResetEvent ev2_event;

		bool cb1_called;
        bool cb2_called;
        bool cb3_called;
        bool cb4_called;
        bool cb5_called;


		void TestMemories();

		void test_m1();

		void test_m2();

		void test_m3();



		void DisconnectService();

		void TestAuthentication(string url);
		
		void TestObjectLock(string url);
		
		void TestMonitorLock(string url);

		bool t1;
		bool t2;

		RR_SHARED_PTR<testroot> r2;
		RR_SHARED_PTR<sub1> r2_o;
		RR_SHARED_PTR<sub2> r2_o_o2;

		bool threaderr;

		void test_monitor_lock_thread();
		AutoResetEvent e1;


		AutoResetEvent async_wait;
		RR_SHARED_PTR<RobotRaconteurException> async_err;
		boost::mutex async_err_lock;
		void TestAsync_err(RR_SHARED_PTR<RobotRaconteurException> exp);

		void TestAsync(string url);
		
		void TestAsync1(RR_SHARED_PTR<RRObject> r, RR_SHARED_PTR<RobotRaconteurException> exp);

		void TestAsync2(RR_SHARED_PTR<async_testroot> r, RR_INTRUSIVE_PTR<RRArray<double> > ret, RR_SHARED_PTR<RobotRaconteurException> exp);

		void TestAsync3(RR_SHARED_PTR<async_testroot> r, RR_SHARED_PTR<RobotRaconteurException> exp);

		void TestAsync4(RR_SHARED_PTR<async_testroot> r, RR_SHARED_PTR<RobotRaconteurException> exp);

		void TestAsync5(RR_SHARED_PTR<async_testroot> r, double ret, RR_SHARED_PTR<RobotRaconteurException> exp);

		void TestAsync6(RR_SHARED_PTR<async_testroot> r, RR_SHARED_PTR<RobotRaconteurException> exp);

		void TestAsync7(RR_SHARED_PTR<async_testroot> r, RR_SHARED_PTR<sub1> o1, RR_SHARED_PTR<RobotRaconteurException> exp);
		
		void TestAsync8(RR_SHARED_PTR<testroot> r, RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > e1, RR_SHARED_PTR<RobotRaconteurException> exp);

		void TestAsync9(RR_SHARED_PTR<testroot> r, RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > e1, uint32_t pnum, RR_SHARED_PTR<RobotRaconteurException> exp);

		void TestAsync10(RR_SHARED_PTR<testroot> r, RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > e1, RR_SHARED_PTR<RobotRaconteurException> exp);

		void TestAsync11(RR_SHARED_PTR<testroot> r, RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRArray<double> > > > w1, RR_SHARED_PTR<RobotRaconteurException> exp);

		void TestAsync12(RR_SHARED_PTR<testroot> r, RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRArray<double> > > > w1, RR_SHARED_PTR<RobotRaconteurException> exp);

		void TestAsync13(RR_SHARED_PTR<testroot> r, RR_SHARED_PTR<std::string> res, RR_SHARED_PTR<RobotRaconteurException> exp);

		void TestAsync14(RR_SHARED_PTR<testroot> r, RR_SHARED_PTR<std::string> res, RR_SHARED_PTR<RobotRaconteurException> exp);

		void TestAsync15();
	};


}