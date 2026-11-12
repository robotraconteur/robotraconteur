#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
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
