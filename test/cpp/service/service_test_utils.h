#include "array_compare.h"

#include <RobotRaconteur/RobotRaconteurNode.h>
#include <RobotRaconteur/NodeSetup.h>

#pragma once

namespace RobotRaconteur
{
namespace test
{

class GTestThreadLocalListener : public testing::EmptyTestEventListener
{
public:
    GTestThreadLocalListener();
    virtual void OnTestPartResult(const testing::TestPartResult& test_part_result);
    uint32_t FailureCount();
    std::string FailuresMessage();
    virtual ~GTestThreadLocalListener();

    boost::thread::id thread_id;
    uint32_t failure_count;
    std::string failures_msg;
};

class ThrowIfFailures
{
public:
    ThrowIfFailures();
    uint32_t FailureCount();
    std::string FailuresMessage();
    void Throw();
    virtual ~ThrowIfFailures();

    RR_SHARED_PTR<GTestThreadLocalListener> gtest_listener;
};

class TestServerNodeConfig
{
public:
    TestServerNodeConfig(const std::string& nodename);
    std::string GetServiceURL(const std::string& service_name);
    void Shutdown();
    virtual ~TestServerNodeConfig();

    RR_SHARED_PTR<RobotRaconteur::ServerNodeSetup> server;
    RR_SHARED_PTR<RobotRaconteur::RobotRaconteurNode> node;
    std::string node_endpoint_url;
};

}
}