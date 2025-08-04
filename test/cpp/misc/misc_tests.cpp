#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
#include <RobotRaconteur.h>
#include <RobotRaconteur/NodeDirectories.h>

using namespace RobotRaconteur;

TEST(RobotRaconteurMisc, LogMacros)
{
    RR_WEAK_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp();
    RobotRaconteurNode::s()->SetNodeID(NodeID("8cded4c3-5b4a-4682-8b0e-5a01f8eb3378"));
    RobotRaconteurNode::s()->SetLogLevel(RobotRaconteur_LogLevel_Trace);

    ROBOTRACONTEUR_LOG_FATAL(node, "Testing ROBOTRACONTEUR_LOG_ERROR");
    ROBOTRACONTEUR_LOG_ERROR_DEFAULT("Testing ROBOTRACONTEUR_LOG_ERROR_DEFAULT");
    ROBOTRACONTEUR_LOG_WARNING_COMPONENT(node, User, 294573, "Testing ROBOTRACONTEUR_LOG_ERROR_COMPONENT");
    ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, User, 294573, "service.path", "my_member",
                                           "Testing ROBOTRACONTEUR_LOG_ERROR_COMPONENT_PATH");
    ROBOTRACONTEUR_LOG_DEBUG_COMPONENTNAME(node, User, "my_component", "my_object", 294573,
                                           "Testing ROBOTRACONTEUR_LOG_ERROR_COMPONENTNAME");
    ROBOTRACONTEUR_LOG_TRACE_COMPONENTNAME_PATH(node, User, "my_component", "my_object", 294573, "service.path",
                                                "my_member", "Testing ROBOTRACONTEUR_LOG_ERROR_COMPONENTNAME_PATH");
    ROBOTRACONTEUR_LOG_TRACE_THROTTLE(node, User, 294573, 100, "Testing ROBOTRACONTEUR_LOG_ERROR_THROTTLE");
    ROBOTRACONTEUR_LOG_TRACE_THROTTLE_PATH(node, User, 294573, "service.path", "my_member", 101,
                                           "Testing ROBOTRACONTEUR_LOG_ERROR_THROTTLE_PATH");
    ROBOTRACONTEUR_LOG_TRACE_THROTTLE_COMPONENTNAME_PATH(node, User, "my_component", "my_object", 294573,
                                                         "service.path", "my_member", 102,
                                                         "Testing ROBOTRACONTEUR_LOG_ERROR_COMPONENTNAME_PATH");

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Default, -1, "default component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Node, -1, "node component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "transport component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Message, -1, "message component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, -1, "client component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Service, -1, "service component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Member, -1, "member component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Pack, -1, "pack component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Unpack, -1, "unpack component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, ServiceDefinition, -1, "servicedefinition component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "discovery component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Subscription, -1, "subscription component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, NodeSetup, -1, "nodesetup component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Utility, -1, "utility component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, RobDefLib, -1, "robdeflib component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, User, -1, "user component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, UserClient, -1, "user_client component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, UserService, -1, "user_service component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, ThirdParty, -1, "third_party component")
}

TEST(RobotRaconteurMisc, NodeDirectories)
{
    NodeDirectories dirs = NodeDirectoriesUtil::GetDefaultNodeDirectories();
    std::cout << "system_data_dir: " << dirs.system_data_dir << std::endl;
    std::cout << "system_config_dir: " << dirs.system_config_dir << std::endl;
    std::cout << "system_state_dir: " << dirs.system_state_dir << std::endl;
    std::cout << "system_cache_dir: " << dirs.system_cache_dir << std::endl;
    std::cout << "system_run_dir: " << dirs.system_run_dir << std::endl;
    std::cout << "user_data_dir: " << dirs.user_data_dir << std::endl;
    std::cout << "user_config_dir: " << dirs.user_config_dir << std::endl;
    std::cout << "user_state_dir: " << dirs.user_state_dir << std::endl;
    std::cout << "user_cache_dir: " << dirs.user_cache_dir << std::endl;
    std::cout << "user_run_dir: " << dirs.user_run_dir << std::endl;
}

TEST(RobotRaconteurMisc, EncodeServicePathIndexTest)
{
    EXPECT_EQ(EncodeServicePathIndex("alphaname1"), "alphaname1");
    EXPECT_EQ(DecodeServicePathIndex("beta2name3l"), "beta2name3l");
    EXPECT_EQ(EncodeServicePathIndex("gamma_with_special!%$moreWords()*.[]"),
              "gamma%5fwith%5fspecial%21%25%24moreWords%28%29%2a%2e%5b%5d");
    EXPECT_EQ(DecodeServicePathIndex("delta%5fwith%5fspecial%3F%0A%5C%2d%21%25%24More%2E%5B%5Dwords%28%29%2A"),
              "delta_with_special?\n\\-!%$More.[]words()*");
    std::string epsilon = "29471``\"'epsilon_with_special?\n\\-!%$more.[]words()*long_string_of_garbage\t\r";
    std::string epsilon_encoded = EncodeServicePathIndex(epsilon);
    std::string epsilon_decoded = DecodeServicePathIndex(epsilon_encoded);
    EXPECT_EQ(epsilon_decoded, epsilon);
    boost::regex re("^[a-zA-Z0-9%]+$");
    EXPECT_TRUE(boost::regex_match(epsilon_encoded, re));
}

TEST(RobotRaconteurMisc, BuildServicePathTest)
{
    // cSpell: disable
    std::vector<ServicePathSegment> segments;
    segments.push_back(ServicePathSegment("alpha"));
    segments.push_back(ServicePathSegment("be2_ta"));
    segments.push_back(ServicePathSegment("gamma_31", std::string("gamma_index")));
    segments.push_back(
        ServicePathSegment("delta", std::string("delta_index_with_special!%$morewords()*.[]\n\t4927`\"?-")));
    segments.push_back(ServicePathSegment("epsilon", std::string("38563")));

    std::string path = BuildServicePath(segments);
    EXPECT_EQ(path, "alpha.be2_ta.gamma_31[gamma%5findex].delta[delta%5findex%5fwith%5fspecial%21%25%24morewords%28%29%"
                    "2a%2e%5b%5d%0a%094927%60%22%3f%2d].epsilon[38563]");

    std::vector<ServicePathSegment> segments2 = ParseServicePath(path);
    EXPECT_EQ(segments, segments2);

    std::string path2_str = "zeta[alpha].eta1[%2D0175].Theta_3[gamma%5findex].iota[%5B%5D%21bunch%2Dof%2Dgarbage%5f%"
                            "0928576%0A%21%40%23%24%25%5E%26%2A%28%29%2e%0D%5c%2f].kappa";
    std::vector<ServicePathSegment> segments3 = ParseServicePath(path2_str);
    ASSERT_EQ(segments3.size(), 5);
    EXPECT_EQ(segments3[0], ServicePathSegment("zeta", std::string("alpha")));
    EXPECT_EQ(segments3[1], ServicePathSegment("eta1", std::string("-0175")));
    EXPECT_EQ(segments3[2], ServicePathSegment("Theta_3", std::string("gamma_index")));
    EXPECT_EQ(segments3[3], ServicePathSegment("iota", std::string("[]!bunch-of-garbage_\t28576\n!@#$%^&*().\r\\/")));
    EXPECT_EQ(segments3[4], ServicePathSegment("kappa"));

    std::string path2 = BuildServicePath(segments3);
    EXPECT_EQ(boost::to_lower_copy(path2), boost::to_lower_copy(path2_str));
    // cSpell: enable
}

TEST(RobotRaconteurMisc, CreateIdentifierAttributeFilterTest)
{
    ServiceSubscriptionFilterAttribute attr1 = CreateServiceSubscriptionFilterAttributeIdentifier("identifier_1", "");
    EXPECT_TRUE(attr1.IsMatch("identifier_1"));
    EXPECT_FALSE(attr1.IsMatch("identifier_2"));
    EXPECT_TRUE(attr1.IsMatch("identifier_1|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_FALSE(attr1.IsMatch("identifier_2|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));

    ServiceSubscriptionFilterAttribute attr2 =
        CreateServiceSubscriptionFilterAttributeIdentifier("", "cbfea7a4-0361-4aad-95bb-d4dcd967047a");
    EXPECT_FALSE(attr2.IsMatch("identifier_1"));
    EXPECT_FALSE(attr2.IsMatch("identifier_2"));
    EXPECT_TRUE(attr2.IsMatch("identifier_1|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_TRUE(attr2.IsMatch("identifier_2|{cbfea7a4-0361-4aAD-95bb-d4dcd967047a}"));
    EXPECT_TRUE(attr2.IsMatch("identifier_2|cbfea7a403614aAD95bbd4dcd967047a"));
    EXPECT_FALSE(attr2.IsMatch("identifier_2|{cbfea7a4-0361-1aad-95bb-d4dcd967047a}"));

    ServiceSubscriptionFilterAttribute attr3 =
        CreateServiceSubscriptionFilterAttributeIdentifier("identifier_1", "{00000000-0000-0000-0000-000000000000}");
    EXPECT_TRUE(attr3.IsMatch("identifier_1"));
    EXPECT_FALSE(attr3.IsMatch("identifier_2"));
    EXPECT_TRUE(attr3.IsMatch("identifier_1|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_FALSE(attr3.IsMatch("identifier_2|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));

    ServiceSubscriptionFilterAttribute attr4 =
        CreateServiceSubscriptionFilterAttributeIdentifier("identifier_1", "cbfea7a4-0361-4aad-95bb-d4dcd967047a");

    EXPECT_FALSE(attr4.IsMatch("identifier_1"));
    EXPECT_FALSE(attr4.IsMatch("identifier_2"));
    EXPECT_TRUE(attr4.IsMatch("identifier_1|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_FALSE(attr4.IsMatch("identifier_2|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));

    ServiceSubscriptionFilterAttribute attr5 = CreateServiceSubscriptionFilterAttributeIdentifier(
        "identifier_1.with.do_ts", "cbfea7a4-0361-4aad-95bb-d4dcd967047a");
    EXPECT_FALSE(attr5.IsMatch("identifier_1.with.do_ts"));
    EXPECT_FALSE(attr5.IsMatch("identifier_2.WIT_H.dot4s"));
    EXPECT_FALSE(attr5.IsMatch("identifier_1"));
    EXPECT_FALSE(attr5.IsMatch("identifier_2"));
    EXPECT_TRUE(attr5.IsMatch("identifier_1.with.do_ts|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_TRUE(attr5.IsMatch("identifier_1.with.do_ts|{cbfea7a4-0361-4AAD95bb-d4dcd967047a}"));
    EXPECT_FALSE(attr5.IsMatch("identifier_2.WIT_H.dot4s|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_FALSE(attr5.IsMatch("identifier_1.with.do_ts|cbfea7a4-0361-41ad-95bb-d4dcd967047a"));
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    int ret = RUN_ALL_TESTS();

    return ret;
}
