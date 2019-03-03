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

		static RR_INTRUSIVE_PTR<Message> NewTestMessage();

		static RR_INTRUSIVE_PTR<Message> NewRandomTestMessage(boost::random::mt19937& rng);

		static void CompareMessage(RR_INTRUSIVE_PTR<Message> m1, RR_INTRUSIVE_PTR<Message> m2);
		static void CompareMessageEntry(RR_INTRUSIVE_PTR<MessageEntry> m1, RR_INTRUSIVE_PTR<MessageEntry> m2);
		static void CompareMessageElement(RR_INTRUSIVE_PTR<MessageElement> m1, RR_INTRUSIVE_PTR<MessageElement> m2);
		
	};
}
