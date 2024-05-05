#include <RobotRaconteur.h>
#include <gtest/gtest.h>
#include "robotraconteur_generated.h"

using namespace RobotRaconteur;

class testobj_impl : public experimental::tls_test::testobj_default_impl
{
  public:
    RR_OVIRTUAL double add_two_numbers(double a, double b) RR_OVERRIDE { return a + b; }
};

TEST(RobotRaconteurTLS, RobotRaconteurTLS)
{
    RR_SHARED_PTR<RobotRaconteurNode> node1 = RR_MAKE_SHARED<RobotRaconteurNode>();
    node1->Init();
    RR_SHARED_PTR<RobotRaconteurNode> node2 = RR_MAKE_SHARED<RobotRaconteurNode>();
    node2->Init();
    RR_SHARED_PTR<RobotRaconteurNode> node3 = RR_MAKE_SHARED<RobotRaconteurNode>();
    node3->Init();
    RR_SHARED_PTR<RobotRaconteurNode> node4 = RR_MAKE_SHARED<RobotRaconteurNode>();
    node4->Init();

    RobotRaconteurNodeSetupFlags tls_client_flags =
        (RobotRaconteurNodeSetupFlags)(RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT |
                                       RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT);

    RobotRaconteurNodeSetupFlags client_override = RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT_ALLOWED_OVERRIDE;

    RR_SHARED_PTR<RobotRaconteurNodeSetup> node1_setup =
        RR_MAKE_SHARED<SecureServerNodeSetup>(node1, ROBOTRACONTEUR_SERVICE_TYPES, "testprog_tls1");
    RR_SHARED_PTR<RobotRaconteurNodeSetup> node2_setup =
        RR_MAKE_SHARED<ClientNodeSetup>(node2, ROBOTRACONTEUR_SERVICE_TYPES);
    RR_SHARED_PTR<RobotRaconteurNodeSetup> node3_setup = RR_MAKE_SHARED<RobotRaconteurNodeSetup>(
        node3, ROBOTRACONTEUR_SERVICE_TYPES, "testprog_tls2", 0, tls_client_flags);
    RR_SHARED_PTR<RobotRaconteurNodeSetup> node4_setup = RR_MAKE_SHARED<RobotRaconteurNodeSetup>(
        node4, ROBOTRACONTEUR_SERVICE_TYPES, "testprog_tls3", 0, tls_client_flags);

    RR_SHARED_PTR<testobj_impl> obj = RR_MAKE_SHARED<testobj_impl>();
    RR_SHARED_PTR<testobj_impl> obj2 = RR_MAKE_SHARED<testobj_impl>();
    node1->RegisterService("test_service", "experimental.tls_test", obj);

    std::string authdata = "testuser1 0b91dec4fe98266a03b136b59219d0d6 objectlock " + node3->NodeID().ToString() +
                           "\n" + "testuser2 841c4221c2e7e0cefbc0392a35222512 objectlock " +
                           node4->NodeID().ToString() + "\n" +
                           "testsuperuser 503ed776c50169f681ad7bbc14198b68 objectlock,objectlockoverride " +
                           node3->NodeID().ToString() + "\n";

    RR_SHARED_PTR<UserAuthenticator> p = RR_MAKE_SHARED<PasswordFileUserAuthenticator>(authdata, true);
    std::map<std::string, std::string> policies;
    policies.insert(std::make_pair("requirevaliduser", "true"));
    policies.insert(std::make_pair("allowobjectlock", "true"));

    RR_SHARED_PTR<ServiceSecurityPolicy> s = RR_MAKE_SHARED<ServiceSecurityPolicy>(p, policies);

    node1->RegisterService("test_service_secure", "experimental.tls_test", obj, s);

    int32_t server_tcp_port = node1_setup->GetTcpTransport()->GetListenPort();
    std::string server_tcp_port_s = boost::lexical_cast<std::string>(server_tcp_port);

    std::string url = std::string("rrs+tcp://localhost:") + server_tcp_port_s + "?service=test_service";
    std::string url_unsecure = std::string("rr+tcp://localhost:") + server_tcp_port_s + "?service=test_service";
    std::string url_ws = std::string("rrs+ws://localhost:") + server_tcp_port_s + "?service=test_service";
    std::string url_unsecure_ws = std::string("rr+ws://localhost:") + server_tcp_port_s + "?service=test_service";
    std::string url_secure = std::string("rrs+tcp://localhost:") + server_tcp_port_s + "?service=test_service_secure";

    RR_SHARED_PTR<experimental::tls_test::testobj> c1;

    ASSERT_NO_THROW(c1 = rr_cast<experimental::tls_test::testobj>(node2->ConnectService(url));
                    EXPECT_EQ(c1->add_two_numbers(5, 6), 11); node1->DisconnectService(c1););

    ASSERT_THROW(rr_cast<experimental::tls_test::testobj>(node2->ConnectService(url_unsecure));, ConnectionException);

    ASSERT_NO_THROW(c1 = rr_cast<experimental::tls_test::testobj>(node2->ConnectService(url_ws));
                    EXPECT_EQ(c1->add_two_numbers(5, 6), 11); node1->DisconnectService(c1););

    ASSERT_THROW(rr_cast<experimental::tls_test::testobj>(node2->ConnectService(url_unsecure_ws));
                 , ConnectionException);

    std::string user1 = "testuser1";
    std::string user2 = "testuser2";

    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > cred1 = AllocateEmptyRRMap<std::string, RRValue>();
    cred1->insert(std::make_pair("password", stringToRRArray("testpass1")));

    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > cred2 = AllocateEmptyRRMap<std::string, RRValue>();
    cred2->insert(std::make_pair("password", stringToRRArray("testpass2")));

    EXPECT_THROW(node2->ConnectService(url_secure, user1, cred1);, AuthenticationException);

    ASSERT_NO_THROW(c1 = rr_cast<experimental::tls_test::testobj>(node3->ConnectService(url_secure, user1, cred1));
                    EXPECT_EQ(c1->add_two_numbers(5, 7), 12); node1->DisconnectService(c1););

    EXPECT_THROW(node4->ConnectService(url_secure, user1, cred1);, AuthenticationException);

    ASSERT_NO_THROW(c1 = rr_cast<experimental::tls_test::testobj>(node4->ConnectService(url_secure, user2, cred2));
                    EXPECT_EQ(c1->add_two_numbers(9, 7), 16); node1->DisconnectService(c1););
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    int ret = RUN_ALL_TESTS();

    return ret;
}
