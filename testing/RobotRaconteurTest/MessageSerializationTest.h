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

    static void CompareMessage(const RR_INTRUSIVE_PTR<Message>& m1, const RR_INTRUSIVE_PTR<Message>& m2);
    static void CompareMessageEntry(const RR_INTRUSIVE_PTR<MessageEntry>& m1, const RR_INTRUSIVE_PTR<MessageEntry>& m2);
    static void CompareMessageElement(const RR_INTRUSIVE_PTR<MessageElement>& m1, const RR_INTRUSIVE_PTR<MessageElement>& m2);
};
} // namespace RobotRaconteurTest
