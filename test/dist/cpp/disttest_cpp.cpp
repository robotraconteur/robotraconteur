// Simple test to read the RobotRaconteurNode version

#include <RobotRaconteur.h>
#include <iostream>
#include "robotraconteur_generated.h"

using namespace RobotRaconteur;
using namespace experimental::distfiles_test;

class DistfilesTest_impl : public virtual DistfilesTest
{
    // NOLINTBEGIN
  public:
    virtual double get_c() { return c; }
    virtual void set_c(double value) { c = value; }

    virtual double add(double a, int32_t b) { return a + b + c; }

  private:
    double c;
};

int main(int argc, char* argv[])
{
    RR_SHARED_PTR<DistfilesTest_impl> impl = RR_MAKE_SHARED<DistfilesTest_impl>();

    ServerNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, "experimental.distfiles_test", 0, argc, argv);
    int32_t port = node_setup.GetTcpTransport()->GetListenPort();
    RobotRaconteurNode::s()->RegisterService("DistfilesTest", "experimental.distfiles_test", impl);

    RR_SHARED_PTR<DistfilesTest> c = rr_cast<DistfilesTest>(RobotRaconteurNode::s()->ConnectService(
        "rr+tcp://localhost:" + boost::lexical_cast<std::string>(port) + "?service=DistfilesTest"));

    c->set_c(5.0);
    if (c->add(1.0, 2) != 8.0)
    {
        throw std::runtime_error("Unexpected result");
    }

    std::cout << "Test completed successfully" << std::endl;

    return 0;
}