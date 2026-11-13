#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
#include <boost/assign/list_of.hpp>
#include <RobotRaconteur/ServiceDefinition.h>
#include <RobotRaconteur/RobotRaconteurNode.h>

#include "com__robotraconteur__testing__TestService1.h"
#include "com__robotraconteur__testing__TestService1_stubskel.h"

#include "ServiceTestClient.h"
#include "robotraconteur_generated.h"
#include "service_test_utils.h"

using namespace RobotRaconteur;
using namespace RobotRaconteur::test;
using namespace RobotRaconteurTest;

int my_argc;
char** my_argv;
static std::string service_url;
static std::string service_auth_url;

RR_SHARED_PTR<TestServerNodeConfig> test_server_node_config;
RR_SHARED_PTR<ClientNodeSetup> client_node_setup;

class ServiceTest : public testing::Test
{
  protected:
    RR_OVIRTUAL void SetUp() RR_OVERRIDE
    {
        if (!test_server_node_config)
        {
            test_server_node_config = RR_MAKE_SHARED<TestServerNodeConfig>("unit_service_test");
            service_url = test_server_node_config->GetServiceURL("RobotRaconteurTestService");
            service_auth_url = test_server_node_config->GetServiceURL("RobotRaconteurTestService_auth");
        }
        if (!client_node_setup)
        {
            client_node_setup = RR_MAKE_SHARED<ClientNodeSetup>(ROBOTRACONTEUR_SERVICE_TYPES, my_argc, my_argv);
        }
    }

    RR_OVIRTUAL void TearDown() RR_OVERRIDE {}
};

TEST_F(ServiceTest, MinimalTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.RunMinimalTest(service_url));
}

TEST_F(ServiceTest, PropertyTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestProperties());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, FunctionTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestFunctions());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, EventTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestEvents());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, ObjRefTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestObjRefs());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, PipeTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestPipes());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, CallbackTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestCallbacks());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, WireTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestWires());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, MemoryTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestMemories());
    ASSERT_NO_THROW(c.DisconnectService());
}

class MemoryGetParamCounterTap : public MessageTap
{
  public:
    boost::mutex lock;
    int count;
    MemoryGetParamCounterTap() : count(0) {}
    RR_OVIRTUAL void Open() RR_OVERRIDE {}
    RR_OVIRTUAL void Close() RR_OVERRIDE {}
    RR_OVIRTUAL void RecordLogRecord(const RRLogRecord& log_record) RR_OVERRIDE { RR_UNUSED(log_record); }
    RR_OVIRTUAL void RecordMessage(const RR_INTRUSIVE_PTR<Message>& message) RR_OVERRIDE
    {
        boost::mutex::scoped_lock l(lock);
        BOOST_FOREACH (const RR_INTRUSIVE_PTR<MessageEntry>& e, message->entries)
        {
            if (e->EntryType == MessageEntryType_MemoryGetParam && e->MemberName == "m1")
            {
                count++;
            }
        }
    }
    int GetCount()
    {
        boost::mutex::scoped_lock l(lock);
        return count;
    }
};

TEST_F(ServiceTest, MemoryMaxTransferSizeCachedTest)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp();
    RR_SHARED_PTR<com::robotraconteur::testing::TestService1::testroot> r =
        rr_cast<com::robotraconteur::testing::TestService1::testroot>(node->ConnectService(service_url));

    RR_SHARED_PTR<MemoryGetParamCounterTap> tap = RR_MAKE_SHARED<MemoryGetParamCounterTap>();
    RR_SHARED_PTR<MessageTap> old_tap = node->GetMessageTap();
    node->SetMessageTap(tap);

    RR_INTRUSIVE_PTR<RRArray<double> > buf = AllocateRRArray<double>(10);
    EXPECT_NO_THROW(r->get_m1()->Read(0, buf, 0, 10));
    int first_count = tap->GetCount();
    EXPECT_NO_THROW(r->get_m1()->Read(0, buf, 0, 10));
    EXPECT_NO_THROW(r->get_m1()->Read(0, buf, 0, 10));
    int final_count = tap->GetCount();

    node->SetMessageTap(old_tap);
    node->DisconnectService(r);

    // MaxTransferSize should only be requested once per memory client
    EXPECT_GT(first_count, 0);
    EXPECT_EQ(final_count, first_count);
}

TEST_F(ServiceTest, AuthenticationTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.TestAuthentication(service_auth_url));
}

TEST_F(ServiceTest, ObjectLockTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.TestObjectLock(service_auth_url));
}

TEST_F(ServiceTest, MonitorLockTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.TestMonitorLock(service_url));
}

TEST_F(ServiceTest, MonitorLockTimeoutErrorTest)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp();
    RR_SHARED_PTR<com::robotraconteur::testing::TestService1::testroot> r1 =
        rr_cast<com::robotraconteur::testing::TestService1::testroot>(node->ConnectService(service_url));
    RR_SHARED_PTR<com::robotraconteur::testing::TestService1::testroot> r2 =
        rr_cast<com::robotraconteur::testing::TestService1::testroot>(node->ConnectService(service_url));
    RR_SHARED_PTR<com::robotraconteur::testing::TestService1::sub1> o1 = r1->get_o1();
    RR_SHARED_PTR<com::robotraconteur::testing::TestService1::sub1> o2 = r2->get_o1();

    ASSERT_NO_THROW(node->MonitorEnter(o1));
    // The service object throws RequestTimeoutException when the monitor lock times out.
    // The exception must be returned to the client, not replaced by an unknown error.
    EXPECT_THROW(node->MonitorEnter(o2, 200), RequestTimeoutException);
    EXPECT_NO_THROW(node->MonitorExit(o1));

    node->DisconnectService(r1);
    node->DisconnectService(r2);
}

TEST_F(ServiceTest, AsyncTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.TestAsync(service_auth_url));
}

TEST_F(ServiceTest, MultiDimMemoryRequestOverflowTest)
{
    // A crafted MemoryRead whose count wraps 32-bit arithmetic must be rejected by the service
    RR_SHARED_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp();
    RR_SHARED_PTR<com::robotraconteur::testing::TestService1::testroot> r =
        rr_cast<com::robotraconteur::testing::TestService1::testroot>(node->ConnectService(service_url));
    RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(r);

    std::vector<uint64_t> memorypos = boost::assign::list_of(1)(1);
    std::vector<uint64_t> count = boost::assign::list_of(0xFFFFFFFFu)(0xFFFFFFFFu);
    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_MemoryRead, "m2");
    m->AddElement("memorypos", VectorToRRArray<uint64_t>(memorypos));
    m->AddElement("count", VectorToRRArray<uint64_t>(count));
    EXPECT_THROW(stub->ProcessRequest(m), InvalidArgumentException);

    node->DisconnectService(r);
}

TEST_F(ServiceTest, MemoryWriteReplyTypeTest)
{
    // MemoryWrite requests must be answered with MemoryWriteRet
    RR_SHARED_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp();
    RR_SHARED_PTR<com::robotraconteur::testing::TestService1::testroot> r =
        rr_cast<com::robotraconteur::testing::TestService1::testroot>(node->ConnectService(service_url));
    RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(r);

    // Array memory: read element 0 and write the same value back
    RR_INTRUSIVE_PTR<MessageEntry> rd = CreateMessageEntry(MessageEntryType_MemoryRead, "m1");
    rd->AddElement("memorypos", ScalarToRRArray<uint64_t>(0));
    rd->AddElement("count", ScalarToRRArray<uint64_t>(1));
    RR_INTRUSIVE_PTR<MessageEntry> rd_ret = stub->ProcessRequest(rd);
    RR_INTRUSIVE_PTR<MessageEntry> wr = CreateMessageEntry(MessageEntryType_MemoryWrite, "m1");
    wr->AddElement("memorypos", ScalarToRRArray<uint64_t>(0));
    wr->AddElement("count", ScalarToRRArray<uint64_t>(1));
    wr->AddElement("data", rd_ret->FindElement("data")->GetData());
    RR_INTRUSIVE_PTR<MessageEntry> wr_ret = stub->ProcessRequest(wr);
    EXPECT_EQ(wr_ret->EntryType, MessageEntryType_MemoryWriteRet);

    // Multidimensional memory: read one element and write the same value back
    std::vector<uint64_t> pos(5, 0);
    std::vector<uint64_t> count(5, 1);
    RR_INTRUSIVE_PTR<MessageEntry> rd2 = CreateMessageEntry(MessageEntryType_MemoryRead, "m2");
    rd2->AddElement("memorypos", VectorToRRArray<uint64_t>(pos));
    rd2->AddElement("count", VectorToRRArray<uint64_t>(count));
    RR_INTRUSIVE_PTR<MessageEntry> rd2_ret = stub->ProcessRequest(rd2);
    RR_INTRUSIVE_PTR<MessageEntry> wr2 = CreateMessageEntry(MessageEntryType_MemoryWrite, "m2");
    wr2->AddElement("memorypos", VectorToRRArray<uint64_t>(pos));
    wr2->AddElement("count", VectorToRRArray<uint64_t>(count));
    wr2->AddElement("data", rd2_ret->FindElement("data")->GetData());
    RR_INTRUSIVE_PTR<MessageEntry> wr2_ret = stub->ProcessRequest(wr2);
    EXPECT_EQ(wr2_ret->EntryType, MessageEntryType_MemoryWriteRet);

    node->DisconnectService(r);
}

TEST_F(ServiceTest, MultiDimMemoryReadReplyCountTest)
{
    // The "count" element of a multidimensional MemoryRead reply must echo the requested count
    RR_SHARED_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp();
    RR_SHARED_PTR<com::robotraconteur::testing::TestService1::testroot> r =
        rr_cast<com::robotraconteur::testing::TestService1::testroot>(node->ConnectService(service_url));
    RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(r);

    std::vector<uint64_t> pos(5, 1);
    std::vector<uint64_t> count(5, 2);
    RR_INTRUSIVE_PTR<MessageEntry> rd = CreateMessageEntry(MessageEntryType_MemoryRead, "m2");
    rd->AddElement("memorypos", VectorToRRArray<uint64_t>(pos));
    rd->AddElement("count", VectorToRRArray<uint64_t>(count));
    RR_INTRUSIVE_PTR<MessageEntry> ret = stub->ProcessRequest(rd);
    std::vector<uint64_t> ret_pos = RRArrayToVector<uint64_t>(ret->FindElement("memorypos")->CastData<RRArray<uint64_t> >());
    std::vector<uint64_t> ret_count = RRArrayToVector<uint64_t>(ret->FindElement("count")->CastData<RRArray<uint64_t> >());
    EXPECT_EQ(ret_pos, pos);
    EXPECT_EQ(ret_count, count);

    node->DisconnectService(r);
}

TEST_F(ServiceTest, MonitorEnterFailureReleasesStubLock)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp();
    RR_SHARED_PTR<com::robotraconteur::testing::TestService1::testroot> r1 =
        rr_cast<com::robotraconteur::testing::TestService1::testroot>(node->ConnectService(service_url));

    // testroot is not monitor lockable, so the service rejects MonitorEnter.
    EXPECT_ANY_THROW(node->MonitorEnter(r1, 200));

    // A failed MonitorEnter must not leave the stub's client-side monitor mutex held by this thread.
    RR_SHARED_PTR<ServiceStub> s = rr_cast<ServiceStub>(r1);
    bool acquired = false;
    boost::thread t([&]() {
        acquired = s->RRMutex->try_lock();
        if (acquired)
            s->RRMutex->unlock();
    });
    t.join();
    EXPECT_TRUE(acquired);

    node->DisconnectService(r1);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    my_argc = argc;
    my_argv = argv;

    int ret = RUN_ALL_TESTS();

    test_server_node_config.reset();
    client_node_setup.reset();

    return ret;
}
