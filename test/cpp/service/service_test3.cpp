#include <boost/shared_array.hpp>
#include <cstdlib>

#include <gtest/gtest.h>
#include <RobotRaconteur/ServiceDefinition.h>
#include <RobotRaconteur/RobotRaconteurNode.h>

#include "ServiceTestClient3.h"
#include "ServiceTest3.h"
#include "robotraconteur_generated.h"
#include "service_test_utils.h"

using namespace RobotRaconteur;
using namespace RobotRaconteur::test;
using namespace RobotRaconteurTest;

int my_argc;
char** my_argv;
static std::string service3_url;

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
            service3_url = test_server_node_config->GetServiceURL("RobotRaconteurTestService3");
        }
        if (!client_node_setup)
        {
            client_node_setup = RR_MAKE_SHARED<ClientNodeSetup>(ROBOTRACONTEUR_SERVICE_TYPES, my_argc, my_argv);
        }
    }

    RR_OVIRTUAL void TearDown() RR_OVERRIDE {}
};

TEST_F(ServiceTest, Properties3Test)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient3 c;
    ASSERT_NO_THROW(c.Connect(service3_url));
    ASSERT_NO_THROW(c.TestProperties());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST_F(ServiceTest, Functions3Test)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient3 c;
    ASSERT_NO_THROW(c.Connect(service3_url));
    ASSERT_NO_THROW(c.TestProperties());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST(ServiceTest3Pod, NamedArrayFieldArrayInvalidLength)
{
    using com::robotraconteur::testing::TestService3::transform;
    typedef pod_field_array<transform, 15, true> field_type;

    struct
    {
        field_type v;
        double guard[8];
    } s = {};

    // 15 * 7 + 3 doubles: not a whole number of 7-double transforms, and 3 more than the field can hold
    RR_INTRUSIVE_PTR<RRArray<double> > bad = AllocateRRArray<double>(15 * 7 + 3);
    std::fill(bad->begin(), bad->end(), 1.0);
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > a1;
    a1.push_back(CreateMessageElement("array", bad));
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > in;
    in.push_back(CreateMessageElement(
        "t3", CreateMessageElementNestedElementList(DataTypes_namedarray_array_t,
                                                    "com.robotraconteur.testing.TestService3.transform", RR_MOVE(a1))));
    EXPECT_THROW(PodStub<field_type>::UnpackField(s.v, "t3", in), DataTypeException);
    EXPECT_EQ(s.v.size(), 0u);
    for (size_t i = 0; i < 8; i++)
    {
        EXPECT_EQ(s.guard[i], 0.0);
    }

    std::vector<RR_INTRUSIVE_PTR<MessageElement> > a2;
    a2.push_back(CreateMessageElement("array", AllocateRRArray<double>(7 * 2)));
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > in2;
    in2.push_back(CreateMessageElement(
        "t3", CreateMessageElementNestedElementList(DataTypes_namedarray_array_t,
                                                    "com.robotraconteur.testing.TestService3.transform", RR_MOVE(a2))));
    EXPECT_NO_THROW(PodStub<field_type>::UnpackField(s.v, "t3", in2));
    EXPECT_EQ(s.v.size(), 2u);
}

struct RegisterServiceContextTask
{
    RR_SHARED_PTR<RobotRaconteurNode> node;
    RR_SHARED_PTR<ServerContext> c;
    boost::mutex lock;
    boost::condition_variable cv;
    bool done;
    bool failed;

    void run()
    {
        bool f = false;
        try
        {
            node->RegisterService(c);
        }
        catch (std::exception&)
        {
            f = true;
        }
        boost::mutex::scoped_lock l(lock);
        failed = f;
        done = true;
        cv.notify_all();
    }
};

TEST(ServiceTest3Node, RegisterServiceContextReplacesExisting)
{
    // RegisterService(ServerContext) with a name already in use closes the old service. It called
    // CloseService() while holding services_lock, which locks it again and deadlocked.
    TestServerNodeConfig cfg("unit_registerservice_replace_test");
    if (!cfg.node)
        return;
    RR_SHARED_PTR<ServerContext> old_c = cfg.node->GetService("RobotRaconteurTestService3");
    RR_SHARED_PTR<ServerContext> c =
        RR_MAKE_SHARED<ServerContext>(cfg.node->GetServiceType("com.robotraconteur.testing.TestService5"), cfg.node);
    c->SetBaseObject("RobotRaconteurTestService3", RR_MAKE_SHARED<asynctestroot_impl>());

    RegisterServiceContextTask task;
    task.node = cfg.node;
    task.c = c;
    task.done = false;
    task.failed = false;
    boost::thread t(boost::bind(&RegisterServiceContextTask::run, &task));
    {
        boost::mutex::scoped_lock lock(task.lock);
        boost::system_time deadline = boost::get_system_time() + boost::posix_time::seconds(10);
        while (!task.done && task.cv.timed_wait(lock, deadline))
        {}
        if (!task.done)
        {
            ADD_FAILURE() << "RegisterService(ServerContext) deadlocked";
            std::cerr << "RegisterService(ServerContext) deadlocked" << std::endl;
            std::_Exit(1);
        }
    }
    t.join();
    EXPECT_FALSE(task.failed);
    EXPECT_EQ(cfg.node->GetService("RobotRaconteurTestService3"), c);
    EXPECT_NE(old_c, c);
    cfg.Shutdown();
}

TEST(NamedArray, ConstAt)
{
    typedef com::robotraconteur::testing::TestService3::vector3 vector3;
    RR_INTRUSIVE_PTR<RRNamedArray<vector3> > a = AllocateEmptyRRNamedArray<vector3>(2);
    (*a)[1].s.x = 5.0;
    const RRNamedArray<vector3>& ca = *a;
    EXPECT_EQ(ca.at(1).s.x, 5.0);
    EXPECT_THROW(ca.at(2), std::out_of_range);
}

TEST(NamedArray, UnpackVoidElementsThrow)
{
    typedef com::robotraconteur::testing::TestService3::vector3 vector3;

    // "array" element of type void (null data)
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
    v.push_back(CreateMessageElement("array", RR_INTRUSIVE_PTR<MessageElementData>()));
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> l = CreateMessageElementNestedElementList(
        DataTypes_namedarray_array_t, RRPrimUtil<vector3>::GetElementTypeString(), v);
    EXPECT_THROW(NamedArrayStub_UnpackNamedArrayFromArray<vector3>(l), NullValueException);
    EXPECT_THROW(NamedArrayStub_UnpackNamedArray<vector3>(l), NullValueException);

    // "dims" element of type void (null data)
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > v2;
    v2.push_back(CreateMessageElement("dims", RR_INTRUSIVE_PTR<MessageElementData>()));
    v2.push_back(CreateMessageElement("array", NamedArrayStub_PackNamedArray(AllocateEmptyRRNamedArray<vector3>(1))));
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> l2 = CreateMessageElementNestedElementList(
        DataTypes_namedarray_multidimarray_t, RRPrimUtil<vector3>::GetElementTypeString(), v2);
    EXPECT_THROW(NamedArrayStub_UnpackNamedMultiDimArray<vector3>(l2), NullValueException);
}

TEST(ServiceTest3Node, CloseServiceUpdatesNonce)
{
    // Closing a service must change the service state nonce so discovery clients see the removal
    TestServerNodeConfig cfg("unit_closeservice_nonce_test");
    if (!cfg.node)
        return;
    std::string nonce1 = cfg.node->GetServiceStateNonce();
    cfg.node->CloseService("RobotRaconteurTestService3");
    EXPECT_NE(cfg.node->GetServiceStateNonce(), nonce1);
    cfg.Shutdown();
}

TEST(ServiceTest3Node, SpecialRequestErrorName)
{
    // Special request error replies must use the standard error name, not the C++ class name.
    // Core clients map these replies by error code, so the name is checked on the raw reply.
    TestServerNodeConfig cfg("unit_special_request_error_name_test");
    if (!cfg.node)
        return;
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    m->header->ReceiverNodeID = NodeID::GetAny();
    m->header->SenderNodeID = NodeID::NewUniqueID();
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_ObjectTypeName, "");
    e->ServicePath = "NoSuchService";
    m->entries.push_back(e);

    RR_INTRUSIVE_PTR<Message> ret = cfg.node->SpecialRequest(m, 0, RR_SHARED_PTR<ITransportConnection>());
    ASSERT_TRUE(ret);
    ASSERT_EQ(ret->entries.size(), 1u);
    EXPECT_EQ(ret->entries[0]->Error, MessageErrorType_ObjectNotFound);
    EXPECT_EQ(ret->entries[0]->FindElement("errorname")->CastDataToString(), "RobotRaconteur.ObjectNotFound");
    cfg.Shutdown();
}

TEST(NamedArray, MemoryElementTypeID)
{
    typedef com::robotraconteur::testing::TestService3::vector3 vector3;
    RR_SHARED_PTR<NamedArrayMemory<vector3> > m1 =
        RR_MAKE_SHARED<NamedArrayMemory<vector3> >(AllocateEmptyRRNamedArray<vector3>(2));
    EXPECT_EQ(m1->ElementTypeID(), DataTypes_namedarray_t);
    std::vector<uint32_t> dims(2, 2);
    RR_SHARED_PTR<NamedMultiDimArrayMemory<vector3> > m2 =
        RR_MAKE_SHARED<NamedMultiDimArrayMemory<vector3> >(AllocateEmptyRRNamedMultiDimArray<vector3>(dims));
    EXPECT_EQ(m2->ElementTypeID(), DataTypes_namedarray_t);
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
