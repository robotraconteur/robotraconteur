#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <RobotRaconteur/Message.h>
#include <boost/random.hpp>

using namespace RobotRaconteur;


namespace RobotRaconteurTest
{
	class MessageSerializationTest3
	{
	public:
		static RR_INTRUSIVE_PTR<Message> NewRandomTestMessage3(boost::random::mt19937& rng);

		static void Test();
		static void Test1();
		static void Test2(bool use_string_table);
		static void Test3();

		static void RandomTest(size_t iterations);
	};
}
