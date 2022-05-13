#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
#include <RobotRaconteur.h>


using namespace RobotRaconteur;


TEST(RobotRaconteurMisc,LogMacros)
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



int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    
    int ret = RUN_ALL_TESTS();

    return ret;
}