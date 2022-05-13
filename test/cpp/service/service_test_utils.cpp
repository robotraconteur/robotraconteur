#include "service_test_utils.h"

#include <boost/algorithm/string.hpp>
#include <RobotRaconteur/RobotRaconteurNode.h>
#include <RobotRaconteur/NodeSetup.h>
#include "robotraconteur_generated.h"

#include "ServiceTest.h"
#include "ServiceTest2.h"
#include "ServiceTest3.h"


using namespace RobotRaconteur;
using namespace RobotRaconteurTest;
namespace RobotRaconteur
{
namespace test
{

GTestThreadLocalListener::GTestThreadLocalListener()
{
    failure_count = 0;
    thread_id = boost::this_thread::get_id();
}
void GTestThreadLocalListener::OnTestPartResult(const testing::TestPartResult& test_part_result)
{
    if (!test_part_result.failed())
    {
        return;
    }
    if (boost::this_thread::get_id() != thread_id)
    {
        return;
    }

    failure_count++;

    std::string msg(test_part_result.message());
    boost::replace_all(msg, "\n", "\\n");
    msg += (" " + std::string(test_part_result.file_name()) + "(" + boost::lexical_cast<std::string>(test_part_result.line_number()) + ")");

    failures_msg += "\\n\\n" + msg;
}
uint32_t GTestThreadLocalListener::FailureCount()
{
    return failure_count;
}
std::string GTestThreadLocalListener::FailuresMessage()
{
    return failures_msg;
}
GTestThreadLocalListener::~GTestThreadLocalListener()
{

}

ThrowIfFailures::ThrowIfFailures()
{
    gtest_listener = RR_MAKE_SHARED<GTestThreadLocalListener>();

    testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();
    listeners.Append(gtest_listener.get());

}   

uint32_t ThrowIfFailures::FailureCount()
{
    if (!gtest_listener) return 0;
    return gtest_listener->FailureCount();
}

std::string ThrowIfFailures::FailuresMessage()
{
    if (!gtest_listener) return "";
    return gtest_listener->FailuresMessage();
}

void ThrowIfFailures::Throw()
{
    if (FailureCount() > 0)
    {
        std::string msg = FailuresMessage();
        if (msg.size() > 256)
        {
            msg = msg.substr(0,253) + "...";
        }
        throw std::runtime_error("Test failures occurred: " + FailuresMessage());
    }

}

ThrowIfFailures::~ThrowIfFailures()
{
    testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();
    listeners.Release(gtest_listener.get());
}



TestServerNodeConfig::TestServerNodeConfig(const std::string& nodename)
{
    char* node_env = std::getenv("ROBOTRACONTEUR_TEST_SERVER_URL");
    if (node_env)
    {
        node_endpoint_url = std::string(node_env);
    }
    else
    {
        node = RR_MAKE_SHARED<RobotRaconteurNode>();
        node->Init();
        server = RR_MAKE_SHARED<ServerNodeSetup>(node,ROBOTRACONTEUR_SERVICE_TYPES,nodename,0);

        RR_SHARED_PTR<TcpTransport> c = server->GetTcpTransport();

        RobotRaconteurTestServiceSupport s;
        s.RegisterServices(c,node);

        RobotRaconteurTestService2Support s2;
        s2.RegisterServices(c,node);

        RobotRaconteurTestService3Support s3;
        s3.RegisterServices(node);
    }
}

std::string TestServerNodeConfig::GetServiceURL(const std::string& service_name)
{
    if (!server)
    {
        if (boost::contains(service_name,"?"))
        {
            return node_endpoint_url + "&service=" + service_name;
        }
        else
        {
            return node_endpoint_url + "?service=" + service_name;
        }
    }
    else
    {
        int32_t port = server->GetTcpTransport()->GetListenPort();
        return "rr+tcp://localhost:" + boost::lexical_cast<std::string>(port) + "?service=" + service_name;
    }
}

void TestServerNodeConfig::Shutdown()
{
    server.reset();
}

TestServerNodeConfig::~TestServerNodeConfig()
{   
    server.reset();
}

    


}
}