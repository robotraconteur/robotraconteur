#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <RobotRaconteur/Message.h>


#include <boost/random.hpp>

using namespace RobotRaconteur;



namespace RobotRaconteurTest
{
	class MessageSerializationTest
	{
	public:
		static void Test();
		static void RandomTest(size_t iterations);

		static RR_SHARED_PTR<Message> NewTestMessage();

		static RR_SHARED_PTR<Message> NewRandomTestMessage(boost::random::mt19937& rng);

		static void CompareMessage(RR_SHARED_PTR<Message> m1, RR_SHARED_PTR<Message> m2);
		static void CompareMessageEntry(RR_SHARED_PTR<MessageEntry> m1, RR_SHARED_PTR<MessageEntry> m2);
		static void CompareMessageElement(RR_SHARED_PTR<MessageElement> m1, RR_SHARED_PTR<MessageElement> m2);
		
	};
}
