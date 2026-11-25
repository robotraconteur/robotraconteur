#include <gtest/gtest.h>
#include <RobotRaconteur.h>

#include "robotraconteur_generated.h"
#include "service_test_utils.h"

using namespace RobotRaconteur;
using namespace RobotRaconteur::test;

// Transport connection stand-in for calling SpecialRequest directly. Counts Close calls
class SingleEntryTestTransportConnection : public ITransportConnection
{
  public:
    RR_WEAK_PTR<RobotRaconteurNode> node;
    int close_count;
    SingleEntryTestTransportConnection(const RR_SHARED_PTR<RobotRaconteurNode>& node) : node(node), close_count(0) {}
    RR_OVIRTUAL void SendMessage(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE
    {
        RR_UNUSED(m);
        throw ConnectionException("Not connected");
    }
    RR_OVIRTUAL void AsyncSendMessage(
        const RR_INTRUSIVE_PTR<Message>& m,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE
    {
        RR_UNUSED(m);
        RR_UNUSED(handler);
        throw ConnectionException("Not connected");
    }
    RR_OVIRTUAL void Close() RR_OVERRIDE { close_count++; }
    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE { RR_UNUSED(endpoint); }
    RR_OVIRTUAL uint32_t GetLocalEndpoint() RR_OVERRIDE { return 0; }
    RR_OVIRTUAL uint32_t GetRemoteEndpoint() RR_OVERRIDE { return 0; }
    RR_OVIRTUAL NodeID GetRemoteNodeID() RR_OVERRIDE { return NodeID::GetAny(); }
    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteurNode> GetNode() RR_OVERRIDE { return node.lock(); }
    RR_OVIRTUAL bool CheckCapabilityActive(uint32_t flag) RR_OVERRIDE
    {
        RR_UNUSED(flag);
        return false;
    }
    RR_OVIRTUAL RR_SHARED_PTR<Transport> GetTransport() RR_OVERRIDE { return RR_SHARED_PTR<Transport>(); }
};

static RR_INTRUSIVE_PTR<Message> NewSpecialRequestMessage(size_t entry_count)
{
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    m->header->ReceiverNodeID = NodeID::GetAny();
    m->header->SenderNodeID = NodeID::NewUniqueID();
    m->header->SenderEndpoint = 1234;
    for (size_t i = 0; i < entry_count; i++)
    {
        RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_GetServiceDesc, "");
        e->AddElement("ServiceType", stringToRRArray("com.robotraconteur.testing.TestService1"));
        e->AddElement("clientversion", stringToRRArray(ROBOTRACONTEUR_VERSION_TEXT));
        m->entries.push_back(e);
    }
    return m;
}

// A special request must be the only entry in its message. A message with more entries is a protocol violation:
// the node closes the connection and does not answer. Each entry used to be answered, so one message could make
// the node build a reply of entries x service definition size
TEST(RobotRaconteurService, SpecialRequestSingleEntry)
{
    TestServerNodeConfig config("special_request_single_entry_test");
    RR_SHARED_PTR<RobotRaconteurNode> node = config.node;
    ASSERT_TRUE(node);

    {
        RR_SHARED_PTR<SingleEntryTestTransportConnection> tc = RR_MAKE_SHARED<SingleEntryTestTransportConnection>(node);
        RR_INTRUSIVE_PTR<Message> ret =
            node->SpecialRequest(NewSpecialRequestMessage(1), std::numeric_limits<uint32_t>::max(), tc);
        ASSERT_TRUE(ret);
        ASSERT_EQ(ret->entries.size(), 1u);
        EXPECT_EQ(ret->entries[0]->EntryType, MessageEntryType_GetServiceDescRet);
        EXPECT_EQ(ret->entries[0]->Error, MessageErrorType_None);
        EXPECT_EQ(tc->close_count, 0);
    }

    {
        RR_SHARED_PTR<SingleEntryTestTransportConnection> tc = RR_MAKE_SHARED<SingleEntryTestTransportConnection>(node);
        EXPECT_THROW(node->SpecialRequest(NewSpecialRequestMessage(2), std::numeric_limits<uint32_t>::max(), tc),
                     ProtocolException);
        EXPECT_EQ(tc->close_count, 1);
    }

    {
        RR_SHARED_PTR<SingleEntryTestTransportConnection> tc = RR_MAKE_SHARED<SingleEntryTestTransportConnection>(node);
        EXPECT_THROW(node->SpecialRequest(NewSpecialRequestMessage(0), std::numeric_limits<uint32_t>::max(), tc),
                     ProtocolException);
        EXPECT_EQ(tc->close_count, 1);
    }

    config.Shutdown();
    node->Shutdown();
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
