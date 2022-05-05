#include <RobotRaconteur/AsyncMessageIO.h>
#include <boost/random.hpp>

#pragma once

namespace RobotRaconteur
{
class Message;
}

namespace RobotRaconteurTest
{
class AsyncMessageReaderTest
{
  public:
    static void Test();
    static void Test1();

    static void DoTest(RR_INTRUSIVE_PTR<RobotRaconteur::Message> m, uint16_t version, boost::random::mt19937& rng);
    static void RandomTest(size_t n);
    static void RandomTest4(size_t n);
    static void LoadRandomTest();
};

class AsyncMessageWriterTest
{
  public:
    static void Test();
    static void Test1();

    static void DoTest(RR_INTRUSIVE_PTR<RobotRaconteur::Message> m, uint16_t version, boost::random::mt19937& rng);
    static void RandomTest(size_t n);
    static void RandomTest4(size_t n);

    static void WriteMessage(RR_INTRUSIVE_PTR<RobotRaconteur::Message> m, uint16_t version);
    static void LoadRandomTest();
};
} // namespace RobotRaconteurTest