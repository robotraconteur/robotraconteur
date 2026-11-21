#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
#include <RobotRaconteur/ServiceDefinition.h>
#include <RobotRaconteur/RobotRaconteurNode.h>

#include "ServiceTestClient3.h"
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
