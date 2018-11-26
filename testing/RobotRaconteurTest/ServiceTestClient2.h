#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"
#include "ServiceTest2.h"

#pragma once

using namespace std;
using namespace RobotRaconteur;
using namespace com::robotraconteur::testing::TestService3;


namespace RobotRaconteurTest
{
	class ServiceTestClient2
	{
	public:

		void RunFullTest(const std::string& url);

		void Connect(const std::string& url);
		void Disconnect();

		void TestWirePeekPoke();
		void AsyncTestWirePeekPoke();
		void AsyncTestWirePeekPoke1(const int32_t& value, const TimeSpec& ts, RR_SHARED_PTR<RobotRaconteurException> err);
		void AsyncTestWirePeekPoke2(RR_SHARED_PTR<RobotRaconteurException> err);
		void AsyncTestWirePeekPoke3(const int32_t& value, const TimeSpec& ts, RR_SHARED_PTR<RobotRaconteurException> err);

		void TestEnums();
		void TestCStructs();
		void TestMemories();
		void test_m1();
		void test_m2();

		void TestGenerators();

		RR_SHARED_PTR<testroot3> r;

		AutoResetEvent async_peekpoke_evt;
		RR_SHARED_PTR<RobotRaconteurException> async_peekpoke_err;

	};


}
