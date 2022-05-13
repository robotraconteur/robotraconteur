#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <RobotRaconteur/Message.h>

#include "robotraconteur_test_lfsr_cpp.h"
#include "array_compare.h"

namespace RobotRaconteur
{
namespace test
{
    RR_INTRUSIVE_PTR<RobotRaconteur::Message> NewTestMessage();

    RR_INTRUSIVE_PTR<RobotRaconteur::Message> NewRandomTestMessage(LFSRSeqGen& rng);
    RR_INTRUSIVE_PTR<RobotRaconteur::Message> NewRandomTestMessage4(LFSRSeqGen& rng);

    void CompareMessage(RR_INTRUSIVE_PTR<RobotRaconteur::Message> m1, RR_INTRUSIVE_PTR<RobotRaconteur::Message> m2);
    void CompareMessageEntry(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m1, RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m2);
    void CompareMessageElement(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> m1, RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> m2);
}
}