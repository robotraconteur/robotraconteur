#include "ServiceTestClient3.h"

namespace RobotRaconteurTest
{

void ServiceTestClient3::RunFullTest(const std::string& url)
{
    Connect(url);

    TestProperties();

    TestFunctions();

    Disconnect();
}

void ServiceTestClient3::Connect(const std::string& url)
{
    r = rr_cast<asynctestroot>(RobotRaconteurNode::s()->ConnectService(url));
}

void ServiceTestClient3::Disconnect() { RobotRaconteurNode::s()->DisconnectService(r); }

void ServiceTestClient3::TestProperties()
{
    if (r->get_d1() != 8.5515)
    {
        throw std::runtime_error("");
    }

    r->set_d1(3.0819);

    bool thrown1 = false;
    try
    {
        r->get_err();
    }
    catch (InvalidArgumentException&)
    {
        thrown1 = true;
    }
    if (!thrown1)
    {
        throw std::runtime_error("");
    }

    bool thrown2 = false;
    try
    {
        r->set_err(0.0);
    }
    catch (InvalidOperationException&)
    {
        thrown2 = true;
    }
    if (!thrown2)
    {
        throw std::runtime_error("");
    }
}

void ServiceTestClient3::TestFunctions()
{
    r->f1();
    r->f2(247);
    if (r->f3(10, 20) != 30)
    {
        throw std::runtime_error("");
    }

    bool thrown1 = false;
    try
    {
        r->err_func();
    }
    catch (InvalidOperationException&)
    {
        thrown1 = true;
    }
    if (!thrown1)
    {
        throw std::runtime_error("");
    }

    bool thrown2 = false;
    try
    {
        r->err_func2();
    }
    catch (com::robotraconteur::testing::TestService5::asynctestexp&)
    {
        thrown2 = true;
    }
    if (!thrown2)
    {
        throw std::runtime_error("");
    }
}
} // namespace RobotRaconteurTest