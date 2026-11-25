#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
#include <boost/assign/list_of.hpp>
#include <RobotRaconteur/ServiceDefinition.h>
#include <RobotRaconteur/RobotRaconteurNode.h>
#include <RobotRaconteur/Tap.h>
#include <boost/foreach.hpp>

#include "com__robotraconteur__testing__TestService1.h"
#include "com__robotraconteur__testing__TestService1_stubskel.h"

#include "ServiceTestClient.h"
#include "ServiceTest.h"
#include "ServiceTest.h"
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

TEST_F(ServiceTest, ReleasedObjectRequestTest)
{
    // A request thread can still hold the skel of an object the service has just released (ReleaseServicePath,
    // a changed objref). The request must fail with an error, not call through the released (null) object
    if (!test_server_node_config->node)
        return; // external test server (ROBOTRACONTEUR_TEST_SERVER_URL), its skels are not reachable
    RR_SHARED_PTR<ServerContext> ctx = test_server_node_config->node->GetService("RobotRaconteurTestService");
    std::string path = "RobotRaconteurTestService.o1";
    RR_SHARED_PTR<ServiceSkel> skel = ctx->GetObjectSkel(path);
    ASSERT_TRUE(skel);
    ctx->ReleaseServicePath(path);
    EXPECT_FALSE(skel->GetUncastObject());

    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_PropertyGetReq, "d1");
    m->ServicePath = path;
    EXPECT_THROW(skel->CallGetProperty(m), InvalidOperationException);

    // The object path works again for new requests
    RR_SHARED_PTR<ServiceSkel> skel2 = ctx->GetObjectSkel(path);
    ASSERT_TRUE(skel2);
    EXPECT_TRUE(skel2->GetUncastObject());
    try
    {
        skel2->CallGetProperty(m);
    }
    catch (InvalidOperationException& e)
    {
        ADD_FAILURE() << "object reported as released: " << e.what();
    }
    catch (std::exception&)
    {
        // The test service's o1.d1 is a null array; any other result is fine
    }
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
    std::vector<uint64_t> ret_pos =
        RRArrayToVector<uint64_t>(ret->FindElement("memorypos")->CastData<RRArray<uint64_t> >());
    std::vector<uint64_t> ret_count =
        RRArrayToVector<uint64_t>(ret->FindElement("count")->CastData<RRArray<uint64_t> >());
    EXPECT_EQ(ret_pos, pos);
    EXPECT_EQ(ret_count, count);

    node->DisconnectService(r);
}

// Tries the stub's client-side monitor mutex from another thread
struct TryLockStubMutex
{
    RR_SHARED_PTR<ServiceStub> s;
    bool* acquired;
    void operator()() const
    {
        *acquired = s->RRMutex->try_lock();
        if (*acquired)
            s->RRMutex->unlock();
    }
};

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
    TryLockStubMutex task = {s, &acquired};
    boost::thread t(task);
    t.join();
    EXPECT_TRUE(acquired);

    node->DisconnectService(r1);
}

// PipeSubscription<T>::TryReceivePacketWait with the default ep = NULL must not write through the null pointer
TEST_F(ServiceTest, PipeSubscriptionTryReceivePacketWaitNoEndpoint)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    RR_SHARED_PTR<ServiceSubscription> sub = RobotRaconteurNode::s()->SubscribeService(service_url);
    RR_SHARED_PTR<PipeSubscription<double> > pipe_sub = sub->SubscribePipe<double>("broadcastpipe");

    bool received = false;
    for (size_t i = 0; i < 100 && !received; i++)
    {
        double packet = 0;
        received = pipe_sub->TryReceivePacketWait(packet, 100);
    }
    EXPECT_TRUE(received);

    double packet2 = 0;
    RR_SHARED_PTR<PipeEndpoint<double> > ep;
    bool received2 = false;
    for (size_t i = 0; i < 100 && !received2; i++)
    {
        received2 = pipe_sub->TryReceivePacketWait(packet2, 100, false, &ep);
    }
    EXPECT_TRUE(received2);
    EXPECT_TRUE(ep);

    sub->Close();
}

struct ServiceTest_DisconnectHandler
{
    RR_SHARED_PTR<AutoResetEvent> ev;
    ServiceTest_DisconnectHandler() : ev(RR_MAKE_SHARED<AutoResetEvent>()) {}
    void operator()() { ev->Set(); }
};

// Disconnecting a client that is already closed must still call the handler (DisconnectService waits for it)
TEST_F(ServiceTest, DisconnectServiceAlreadyClosed)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    RR_SHARED_PTR<RRObject> obj;
    ASSERT_NO_THROW(obj = RobotRaconteurNode::s()->ConnectService(service_url));
    RR_SHARED_PTR<ClientContext> ctx;
    ASSERT_NO_THROW(ctx = rr_cast<ServiceStub>(obj)->GetContext());
    ASSERT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(obj));

    ServiceTest_DisconnectHandler h;
    ASSERT_NO_THROW(ctx->AsyncClose(h));
    EXPECT_TRUE(h.ev->WaitOne(5000));
}

// Transport connection stand-in for calling RobotRaconteurNode::SpecialRequest directly
class ServiceTest_DummyTransportConnection : public ITransportConnection
{
  public:
    RR_WEAK_PTR<RobotRaconteurNode> node;
    ServiceTest_DummyTransportConnection(const RR_SHARED_PTR<RobotRaconteurNode>& node) : node(node) {}
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
    RR_OVIRTUAL void Close() RR_OVERRIDE {}
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

static RR_INTRUSIVE_PTR<Message> ServiceTest_CreateSpecialRequest(const RR_INTRUSIVE_PTR<MessageEntry>& e,
                                                                  uint32_t receiver_endpoint, const NodeID& sender)
{
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    m->header->ReceiverNodeID = NodeID::GetAny();
    m->header->SenderNodeID = sender;
    m->header->SenderEndpoint = 1234;
    m->header->ReceiverEndpoint = receiver_endpoint;
    m->entries.push_back(e);
    return m;
}

// An invalid clientversion element in ConnectClientCombined must produce an error reply, not an exception
TEST_F(ServiceTest, ConnectClientCombinedInvalidClientVersion)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = test_server_node_config->node;
    if (!node)
        return; // external test server (ROBOTRACONTEUR_TEST_SERVER_URL), SpecialRequest not reachable
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_ConnectClientCombined, "");
    e->ServicePath = "RobotRaconteurTestService";
    e->AddElement("clientversion", ScalarToRRArray<int32_t>(5));
    RR_INTRUSIVE_PTR<Message> m = ServiceTest_CreateSpecialRequest(e, 0, NodeID::NewUniqueID());

    RR_SHARED_PTR<ITransportConnection> tc = RR_MAKE_SHARED<ServiceTest_DummyTransportConnection>(node);
    RR_INTRUSIVE_PTR<Message> ret;
    ASSERT_NO_THROW(ret = node->SpecialRequest(m, std::numeric_limits<uint32_t>::max(), tc));
    ASSERT_TRUE(ret);
    ASSERT_EQ(ret->entries.size(), 1u);
    EXPECT_NE(ret->entries[0]->Error, MessageErrorType_None);
}

struct ServiceTest_ClientDisconnectedCounter
{
    RR_SHARED_PTR<int> count;
    ServiceTest_ClientDisconnectedCounter() : count(RR_MAKE_SHARED<int>(0)) {}
    void operator()(const RR_SHARED_PTR<ServerContext>& ctx, ServerServiceListenerEventType evt,
                    const RR_SHARED_PTR<void>& p)
    {
        RR_UNUSED(ctx);
        RR_UNUSED(p);
        if (evt == ServerServiceListenerEventType_ClientDisconnected)
            (*count)++;
    }
};

// DisconnectClient naming another service must remove the endpoint from the service it is connected to
TEST_F(ServiceTest, DisconnectClientOtherServiceName)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = test_server_node_config->node;
    if (!node)
        return; // external test server (ROBOTRACONTEUR_TEST_SERVER_URL), SpecialRequest not reachable
    RR_SHARED_PTR<ServerContext> service1 = node->GetService("RobotRaconteurTestService");
    RR_SHARED_PTR<ServerContext> service2 = node->GetService("RobotRaconteurTestService2");
    ServiceTest_ClientDisconnectedCounter counter1;
    ServiceTest_ClientDisconnectedCounter counter2;
    boost::signals2::scoped_connection c1(service1->ServerServiceListener.connect(counter1));
    boost::signals2::scoped_connection c2(service2->ServerServiceListener.connect(counter2));

    NodeID client_id = NodeID::NewUniqueID();
    RR_SHARED_PTR<ITransportConnection> tc = RR_MAKE_SHARED<ServiceTest_DummyTransportConnection>(node);

    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_ConnectClient, "");
    e->ServicePath = "RobotRaconteurTestService";
    RR_INTRUSIVE_PTR<Message> ret = node->SpecialRequest(ServiceTest_CreateSpecialRequest(e, 0, client_id),
                                                         std::numeric_limits<uint32_t>::max(), tc);
    ASSERT_TRUE(ret);
    ASSERT_EQ(ret->entries.size(), 1u);
    ASSERT_EQ(ret->entries[0]->Error, MessageErrorType_None);
    uint32_t ep = ret->header->SenderEndpoint;
    ASSERT_NE(ep, 0u);

    RR_INTRUSIVE_PTR<MessageEntry> e2 = CreateMessageEntry(MessageEntryType_DisconnectClient, "");
    e2->AddElement("servicename", stringToRRArray("RobotRaconteurTestService2"));
    RR_INTRUSIVE_PTR<Message> ret2 = node->SpecialRequest(ServiceTest_CreateSpecialRequest(e2, ep, client_id),
                                                          std::numeric_limits<uint32_t>::max(), tc);
    ASSERT_TRUE(ret2);

    EXPECT_EQ(*counter1.count, 1);
    EXPECT_EQ(*counter2.count, 0);
}

namespace
{
struct WireDuplicateConnectHandler
{
    boost::mutex* lock;
    int* count;
    std::vector<RR_SHARED_PTR<WireConnection<double> > >* connections;
    void operator()(const RR_SHARED_PTR<WireConnection<double> >& c, const RR_SHARED_PTR<RobotRaconteurException>& err)
    {
        RR_UNUSED(err);
        boost::mutex::scoped_lock l(*lock);
        (*count)++;
        if (c)
            connections->push_back(c);
    }
};
} // namespace

TEST_F(ServiceTest, WireBroadcasterDuplicateConnect)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();
    if (!test_server_node_config->node)
    {
        // Test server in another process
        return;
    }

    RR_SHARED_PTR<com::robotraconteur::testing::TestService1::testroot> r;
    ASSERT_NO_THROW(r = rr_cast<com::robotraconteur::testing::TestService1::testroot>(
                        RobotRaconteurNode::s()->ConnectService(service_url)));
    RR_SHARED_PTR<Wire<double> > w = r->get_broadcastwire();

    RR_SHARED_PTR<ServerContext> ctx = test_server_node_config->node->GetService("RobotRaconteurTestService");
    ASSERT_TRUE(ctx);
    RR_SHARED_PTR<RobotRaconteurTest_testroot> obj = RR_DYNAMIC_POINTER_CAST<RobotRaconteurTest_testroot>(
        ctx->GetObjectSkel("RobotRaconteurTestService")->GetUncastObject());
    ASSERT_TRUE(obj);
    ASSERT_TRUE(obj->broadcastwire);
    size_t count_before = obj->broadcastwire->GetActiveWireConnectionCount();

    // Two connect requests sent before the first reply arrives reach the service with the same endpoint
    boost::mutex lock;
    int handler_count = 0;
    std::vector<RR_SHARED_PTR<WireConnection<double> > > connections;
    WireDuplicateConnectHandler h;
    h.lock = &lock;
    h.count = &handler_count;
    h.connections = &connections;
    int started = 0;
    for (int i = 0; i < 2; i++)
    {
        try
        {
            w->AsyncConnect(h);
            started++;
        }
        catch (std::exception&)
        {}
    }

    for (int i = 0; i < 1000; i++)
    {
        {
            boost::mutex::scoped_lock l(lock);
            if (handler_count >= started)
                break;
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    }

    {
        boost::mutex::scoped_lock l(lock);
        EXPECT_EQ(handler_count, started);
        EXPECT_EQ(connections.size(), 1u);
    }
    EXPECT_EQ(obj->broadcastwire->GetActiveWireConnectionCount(), count_before + 1);

    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r));
}

namespace
{
class ArrayMemoryClientUnpackTest : public ArrayMemoryClient<double>
{
  public:
    ArrayMemoryClientUnpackTest(const RR_SHARED_PTR<ServiceStub>& stub)
        : ArrayMemoryClientBase("m1", stub, DataTypes_double_t, sizeof(double), MemberDefinition_Direction_both),
          ArrayMemoryClient<double>("m1", stub, MemberDefinition_Direction_both)
    {}
    void Unpack(const RR_INTRUSIVE_PTR<MessageElementData>& res, RR_INTRUSIVE_PTR<RRArray<double> >& buffer,
                uint64_t bufferpos, uint64_t count)
    {
        UnpackReadResult(res, &buffer, bufferpos, count);
    }
};
} // namespace

TEST_F(ServiceTest, ArrayMemoryClientReadResultLength)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    RR_SHARED_PTR<RRObject> r;
    ASSERT_NO_THROW(r = RobotRaconteurNode::s()->ConnectService(service_url));
    RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(r);

    RR_SHARED_PTR<ArrayMemoryClientUnpackTest> c = RR_MAKE_SHARED<ArrayMemoryClientUnpackTest>(stub);
    RR_INTRUSIVE_PTR<RRArray<double> > buffer = AllocateRRArray<double>(100);
    // Reply shorter than the requested count, or a void reply
    EXPECT_THROW(c->Unpack(AllocateRRArray<double>(2), buffer, 0, 100), DataTypeException);
    EXPECT_THROW(c->Unpack(RR_INTRUSIVE_PTR<MessageElementData>(), buffer, 0, 10), DataTypeException);
    EXPECT_NO_THROW(c->Unpack(AllocateRRArray<double>(10), buffer, 5, 10));

    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r));
}

TEST_F(ServiceTest, MemoryMaxTransferSizeSmallerThanElement)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    RR_SHARED_PTR<com::robotraconteur::testing::TestService1::testroot> r;
    ASSERT_NO_THROW(r = rr_cast<com::robotraconteur::testing::TestService1::testroot>(
                        RobotRaconteurNode::s()->ConnectService(service_url)));

    // The negotiated MaxTransferSize is the smaller of the service's and the client node's value.
    // A value smaller than the element size (here 4 bytes for double) gives zero elements per block.
    uint32_t old_max_transfer_size = RobotRaconteurNode::s()->GetMemoryMaxTransferSize();
    RobotRaconteurNode::s()->SetMemoryMaxTransferSize(4);

    RR_INTRUSIVE_PTR<RRArray<double> > buf = AllocateRRArray<double>(2);
    EXPECT_THROW(r->get_m1()->Read(0, buf, 0, 2), InvalidOperationException);
    EXPECT_THROW(r->get_m1()->Write(0, buf, 0, 2), InvalidOperationException);

    std::vector<uint64_t> pos(5, 0);
    std::vector<uint64_t> count(5, 1);
    count[0] = 2;
    RR_INTRUSIVE_PTR<RRArray<uint32_t> > dims = AllocateRRArray<uint32_t>(5);
    for (size_t i = 0; i < 5; i++)
        (*dims)[i] = boost::numeric_cast<uint32_t>(count[i]);
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > mbuf = AllocateRRMultiDimArray<double>(dims, AllocateRRArray<double>(2));
    EXPECT_THROW(r->get_m2()->Read(pos, mbuf, pos, count), InvalidArgumentException);
    EXPECT_THROW(r->get_m2()->Write(pos, mbuf, pos, count), InvalidArgumentException);

    RobotRaconteurNode::s()->SetMemoryMaxTransferSize(old_max_transfer_size);

    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r));
}

TEST_F(ServiceTest, MultiDimMemorySplitAboveTwoDims)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    RR_SHARED_PTR<com::robotraconteur::testing::TestService1::testroot> r;
    ASSERT_NO_THROW(r = rr_cast<com::robotraconteur::testing::TestService1::testroot>(
                        RobotRaconteurNode::s()->ConnectService(service_url)));
    RR_SHARED_PTR<MultiDimArrayMemory<double> > m2 = r->get_m2();

    std::vector<uint64_t> pos(5, 0);
    pos[0] = 1;
    pos[1] = 2;
    pos[2] = 3;
    std::vector<uint64_t> bufpos(5, 0);
    std::vector<uint64_t> count(5, 1);
    count[0] = 2;
    count[1] = 2;
    count[2] = 2;
    RR_INTRUSIVE_PTR<RRArray<uint32_t> > dims = AllocateRRArray<uint32_t>(5);
    for (size_t i = 0; i < 5; i++)
        (*dims)[i] = boost::numeric_cast<uint32_t>(count[i]);

    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > expected =
        AllocateRRMultiDimArray<double>(dims, AllocateRRArray<double>(8));
    ASSERT_NO_THROW(m2->Read(pos, expected, bufpos, count));

    // One element per block: the split dimension is 0 and dimensions 1 and 2 must be iterated
    uint32_t old_max_transfer_size = RobotRaconteurNode::s()->GetMemoryMaxTransferSize();
    RobotRaconteurNode::s()->SetMemoryMaxTransferSize(sizeof(double));

    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > split =
        AllocateRRMultiDimArray<double>(dims, AllocateRRArray<double>(8));
    for (size_t i = 0; i < 8; i++)
        (*split->Array)[i] = -1.0;
    EXPECT_NO_THROW(m2->Read(pos, split, bufpos, count));
    EXPECT_NO_THROW(m2->Write(pos, split, bufpos, count));

    RobotRaconteurNode::s()->SetMemoryMaxTransferSize(old_max_transfer_size);

    for (size_t i = 0; i < 8; i++)
        EXPECT_EQ((*split->Array)[i], (*expected->Array)[i]) << "element " << i;

    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > after =
        AllocateRRMultiDimArray<double>(dims, AllocateRRArray<double>(8));
    ASSERT_NO_THROW(m2->Read(pos, after, bufpos, count));
    for (size_t i = 0; i < 8; i++)
        EXPECT_EQ((*after->Array)[i], (*expected->Array)[i]) << "element " << i;

    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r));
}

TEST_F(ServiceTest, ClientObjectLockReleaseOtherSession)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();
    using com::robotraconteur::testing::TestService1::sub1;
    using com::robotraconteur::testing::TestService1::testroot;

    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > cred1 = AllocateEmptyRRMap<std::string, RRValue>();
    cred1->insert(std::make_pair("password", stringToRRArray("testpass1")));

    // Two sessions of the same user
    RR_SHARED_PTR<testroot> r1;
    RR_SHARED_PTR<testroot> r2;
    ASSERT_NO_THROW(
        r1 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(service_auth_url, "testuser1", cred1)));
    ASSERT_NO_THROW(
        r2 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(service_auth_url, "testuser1", cred1)));

    RR_SHARED_PTR<sub1> r1_o;
    RR_SHARED_PTR<sub1> r2_o;
    ASSERT_NO_THROW(r1_o = r1->get_o3(2));
    ASSERT_NO_THROW(r2_o = r2->get_o3(2));

    double d1[] = {1.0};
    RR_INTRUSIVE_PTR<RRArray<double> > d = AttachRRArrayCopy(d1, 1);

    EXPECT_NO_THROW(RobotRaconteurNode::s()->RequestObjectLock(r1_o, RobotRaconteurObjectLockFlags_CLIENT_LOCK));
    EXPECT_THROW(r2_o->set_d1(d), ObjectLockedException);
    // The other session may not release the session lock
    EXPECT_THROW(RobotRaconteurNode::s()->ReleaseObjectLock(r2_o), ObjectLockedException);
    EXPECT_THROW(r2_o->set_d1(d), ObjectLockedException);

    EXPECT_NO_THROW(RobotRaconteurNode::s()->ReleaseObjectLock(r1_o));
    EXPECT_NO_THROW(r2_o->set_d1(d));

    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r1));
    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r2));
}

TEST_F(ServiceTest, ClientObjectLockReleasedOnDisconnectAfterReauthentication)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();
    using com::robotraconteur::testing::TestService1::sub1;
    using com::robotraconteur::testing::TestService1::testroot;

    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > cred1 = AllocateEmptyRRMap<std::string, RRValue>();
    cred1->insert(std::make_pair("password", stringToRRArray("testpass1")));
    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > cred2 = AllocateEmptyRRMap<std::string, RRValue>();
    cred2->insert(std::make_pair("password", stringToRRArray("testpass2")));

    RR_SHARED_PTR<testroot> r1;
    RR_SHARED_PTR<testroot> r2;
    ASSERT_NO_THROW(
        r1 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(service_auth_url, "testuser1", cred1)));
    ASSERT_NO_THROW(
        r2 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(service_auth_url, "testuser2", cred2)));

    RR_SHARED_PTR<sub1> r1_o;
    RR_SHARED_PTR<sub1> r2_o;
    ASSERT_NO_THROW(r1_o = r1->get_o3(3));
    ASSERT_NO_THROW(r2_o = r2->get_o3(3));

    double d1[] = {1.0};
    RR_INTRUSIVE_PTR<RRArray<double> > d = AttachRRArrayCopy(d1, 1);

    EXPECT_NO_THROW(RobotRaconteurNode::s()->RequestObjectLock(r1_o, RobotRaconteurObjectLockFlags_CLIENT_LOCK));
    EXPECT_THROW(r2_o->set_d1(d), ObjectLockedException);

    // Authenticate the locking session as another user, then disconnect it. The session lock must be released.
    EXPECT_NO_THROW(rr_cast<ServiceStub>(r1)->GetContext()->AuthenticateUser("testuser2", cred2));
    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r1));

    bool released = false;
    for (int i = 0; i < 100 && !released; i++)
    {
        try
        {
            r2_o->set_d1(d);
            released = true;
        }
        catch (ObjectLockedException&)
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(20));
        }
    }
    EXPECT_TRUE(released);

    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r2));
}

TEST_F(ServiceTest, MemoryRequestMaxTransferSizeTest)
{
    // The service must enforce MaxTransferSize on memory requests itself. Clients split larger transfers, but a
    // crafted request could otherwise make the service allocate count elements before the memory is consulted
    RR_SHARED_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp();
    RR_SHARED_PTR<com::robotraconteur::testing::TestService1::testroot> r =
        rr_cast<com::robotraconteur::testing::TestService1::testroot>(node->ConnectService(service_url));
    RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(r);

    // m3 is a 1024 x 1024 uint8 multidimensional memory, 1 MB, larger than the default 100 KB MaxTransferSize
    {
        std::vector<uint64_t> memorypos(2, 0);
        std::vector<uint64_t> count(2, 1024);
        RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_MemoryRead, "m3");
        m->AddElement("memorypos", VectorToRRArray<uint64_t>(memorypos));
        m->AddElement("count", VectorToRRArray<uint64_t>(count));
        EXPECT_THROW(stub->ProcessRequest(m), InvalidArgumentException);
    }

    // m1 is a double array memory: one element more than MaxTransferSize allows
    {
        uint64_t count = node->GetMemoryMaxTransferSize() / sizeof(double) + 1;
        RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_MemoryRead, "m1");
        m->AddElement("memorypos", ScalarToRRArray<uint64_t>(0));
        m->AddElement("count", ScalarToRRArray<uint64_t>(count));
        EXPECT_THROW(stub->ProcessRequest(m), InvalidArgumentException);
    }

    // Requests within MaxTransferSize still work
    {
        std::vector<uint64_t> memorypos(2, 0);
        std::vector<uint64_t> count(2, 16);
        RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_MemoryRead, "m3");
        m->AddElement("memorypos", VectorToRRArray<uint64_t>(memorypos));
        m->AddElement("count", VectorToRRArray<uint64_t>(count));
        EXPECT_NO_THROW(stub->ProcessRequest(m));
    }

    node->DisconnectService(r);
}

TEST_F(ServiceTest, ServicePathIndexAliases)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();
    if (!test_server_node_config->node)
    {
        // Test server in another process
        return;
    }

    RR_SHARED_PTR<com::robotraconteur::testing::TestService1::testroot> r;
    ASSERT_NO_THROW(r = rr_cast<com::robotraconteur::testing::TestService1::testroot>(
                        RobotRaconteurNode::s()->ConnectService(service_url)));
    RR_SHARED_PTR<ServerContext> ctx = test_server_node_config->node->GetService("RobotRaconteurTestService");
    ASSERT_TRUE(ctx);

    RR_SHARED_PTR<ServiceSkel> s1;
    RR_SHARED_PTR<ServiceSkel> s2;
    ASSERT_NO_THROW(s1 = ctx->GetObjectSkel("RobotRaconteurTestService.o2[1]"));
    ASSERT_NO_THROW(s2 = ctx->GetObjectSkel("RobotRaconteurTestService.o2[%31]"));
    EXPECT_EQ(s1, s2);
    EXPECT_EQ(s2->GetServicePath(), "RobotRaconteurTestService.o2[1]");

    RR_SHARED_PTR<ServiceSkel> s3;
    RR_SHARED_PTR<ServiceSkel> s4;
    ASSERT_NO_THROW(s3 = ctx->GetObjectSkel("RobotRaconteurTestService.o4[alias%2etest]"));
    ASSERT_NO_THROW(s4 = ctx->GetObjectSkel("RobotRaconteurTestService.o4[alias%2Etest]"));
    EXPECT_EQ(s3, s4);
    EXPECT_EQ(s4->GetServicePath(), "RobotRaconteurTestService.o4[alias%2etest]");

    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r));
}

// A connect reply received after the client is connected must not change the service's identity
TEST_F(ServiceTest, ClientIgnoresLateConnectClientRet)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    RR_SHARED_PTR<RRObject> obj;
    ASSERT_NO_THROW(obj = RobotRaconteurNode::s()->ConnectService(service_url));
    RR_SHARED_PTR<ClientContext> ctx = rr_cast<ServiceStub>(obj)->GetContext();
    NodeID remote_nodeid = ctx->GetRemoteNodeID();
    uint32_t remote_ep = ctx->GetRemoteEndpoint();
    std::string remote_nodename = ctx->GetRemoteNodeName();

    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    m->header->SenderNodeID = NodeID::NewUniqueID();
    m->header->SenderNodeName = "other_node";
    m->header->SenderEndpoint = remote_ep + 1;
    m->header->ReceiverNodeID = RobotRaconteurNode::s()->NodeID();
    m->header->ReceiverEndpoint = ctx->GetLocalEndpoint();
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_ConnectClientCombinedRet, "");
    e->RequestID = 0xFFFFFFF0;
    m->entries.push_back(e);
    ctx->MessageReceived(m);

    EXPECT_EQ(ctx->GetRemoteNodeID(), remote_nodeid);
    EXPECT_EQ(ctx->GetRemoteEndpoint(), remote_ep);
    EXPECT_EQ(ctx->GetRemoteNodeName(), remote_nodename);

    RobotRaconteurNode::s()->DisconnectService(obj);
}

// Records replies (PropertyGetRes) with a given request id that arrive at the client node
class EndpointCheckReplyTap : public MessageTap
{
  public:
    boost::mutex lock;
    uint32_t request_id;
    std::vector<RR_INTRUSIVE_PTR<Message> > replies;
    EndpointCheckReplyTap(uint32_t request_id) : request_id(request_id) {}
    RR_OVIRTUAL void Open() RR_OVERRIDE {}
    RR_OVIRTUAL void Close() RR_OVERRIDE {}
    RR_OVIRTUAL void RecordLogRecord(const RRLogRecord& log_record) RR_OVERRIDE { RR_UNUSED(log_record); }
    RR_OVIRTUAL void RecordMessage(const RR_INTRUSIVE_PTR<Message>& message) RR_OVERRIDE
    {
        BOOST_FOREACH (const RR_INTRUSIVE_PTR<MessageEntry>& e, message->entries)
        {
            if (e->EntryType == MessageEntryType_PropertyGetRes && e->RequestID == request_id)
            {
                boost::mutex::scoped_lock l(lock);
                replies.push_back(message);
            }
        }
    }
};

static void EndpointCheckSendHandler(const RR_SHARED_PTR<RobotRaconteurException>& err) { RR_UNUSED(err); }

TEST_F(ServiceTest, ConnectionEndpointCheckTest)
{
    // A connection carries one client / service endpoint pair. A request on client B's connection that names client
    // A's service endpoint must be rejected, not run in A's session (with A's user, locks and member state)
    RR_SHARED_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp();
    RR_SHARED_PTR<RRObject> ra = node->ConnectService(service_url);
    RR_SHARED_PTR<RRObject> rb = node->ConnectService(service_url);
    RR_SHARED_PTR<ServiceStub> stub_a = rr_cast<ServiceStub>(ra);
    RR_SHARED_PTR<ClientContext> ctx_a = stub_a->GetContext();
    RR_SHARED_PTR<ClientContext> ctx_b = rr_cast<ServiceStub>(rb)->GetContext();
    ASSERT_NE(ctx_a->GetLocalEndpoint(), ctx_b->GetLocalEndpoint());

    const uint32_t request_id = 0x7f3a5c21;
    RR_SHARED_PTR<EndpointCheckReplyTap> tap = RR_MAKE_SHARED<EndpointCheckReplyTap>(request_id);
    RR_SHARED_PTR<MessageTap> old_tap = node->GetMessageTap();
    node->SetMessageTap(tap);

    // Sent on B's connection (routed by SenderEndpoint), addressed to A's service endpoint
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    m->header->SenderNodeID = node->NodeID();
    m->header->ReceiverNodeID = ctx_b->GetRemoteNodeID();
    m->header->SenderEndpoint = ctx_b->GetLocalEndpoint();
    m->header->ReceiverEndpoint = ctx_a->GetRemoteEndpoint();
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_PropertyGetReq, "d1");
    e->ServicePath = stub_a->ServicePath;
    e->RequestID = request_id;
    m->entries.push_back(e);
    node->AsyncSendMessage(m, &EndpointCheckSendHandler);

    RR_INTRUSIVE_PTR<Message> reply;
    for (int i = 0; i < 100 && !reply; i++)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(50));
        boost::mutex::scoped_lock l(tap->lock);
        if (!tap->replies.empty())
            reply = tap->replies.front();
    }
    node->SetMessageTap(old_tap);

    ASSERT_TRUE(reply) << "no reply to the request";
    // Not run as A: the reply did not go to A's client endpoint
    EXPECT_NE(reply->header->ReceiverEndpoint, ctx_a->GetLocalEndpoint());
    EXPECT_EQ(reply->header->ReceiverEndpoint, ctx_b->GetLocalEndpoint());
    ASSERT_EQ(reply->entries.size(), 1u);
    EXPECT_EQ(reply->entries.at(0)->Error, MessageErrorType_InvalidEndpoint);

    // A's session is unaffected. B gets the InvalidEndpoint error, and a client treats that as a lost session and
    // closes (Client.cpp, ClientContext::MessageReceived)
    EXPECT_NO_THROW(rr_cast<com::robotraconteur::testing::TestService1::testroot>(ra)->get_d1());

    node->DisconnectService(ra);
    // B has already closed itself; not disconnected again here (DisconnectService on a closed client waits for a
    // handler AsyncClose never calls, CORE-T6-D8)
}

TEST_F(ServiceTest, ObjectLockRequiresAllowObjectLockPolicy)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();
    if (!test_server_node_config->node)
    {
        // Test server in another process
        return;
    }

    std::string authdata = "testuser1 0b91dec4fe98266a03b136b59219d0d6 objectlock\n";
    RR_SHARED_PTR<PasswordFileUserAuthenticator> p = RR_MAKE_SHARED<PasswordFileUserAuthenticator>(authdata);
    std::map<std::string, std::string> policies;
    policies.insert(std::make_pair("requirevaliduser", "true"));
    RR_SHARED_PTR<ServiceSecurityPolicy> s = RR_MAKE_SHARED<ServiceSecurityPolicy>(p, policies);
    ASSERT_NO_THROW(test_server_node_config->node->RegisterService("RobotRaconteurTestService_nolockpolicy",
                                                                   "com.robotraconteur.testing.TestService2",
                                                                   RR_MAKE_SHARED<TestService2SubObj>(), s));
    std::string url = test_server_node_config->GetServiceURL("RobotRaconteurTestService_nolockpolicy");

    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > cred1 = AllocateEmptyRRMap<std::string, RRValue>();
    cred1->insert(std::make_pair("password", stringToRRArray("testpass1")));
    RR_SHARED_PTR<RRObject> r;
    ASSERT_NO_THROW(r = RobotRaconteurNode::s()->ConnectService(url, "testuser1", cred1));

    EXPECT_THROW(RobotRaconteurNode::s()->RequestObjectLock(r, RobotRaconteurObjectLockFlags_USER_LOCK),
                 PermissionDeniedException);
    EXPECT_THROW(RobotRaconteurNode::s()->RequestObjectLock(r, RobotRaconteurObjectLockFlags_CLIENT_LOCK),
                 PermissionDeniedException);

    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r));
    EXPECT_NO_THROW(test_server_node_config->node->CloseService("RobotRaconteurTestService_nolockpolicy"));
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
