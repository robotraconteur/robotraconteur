// Members whose names contain "virtual" (a property is_virtual, a wire named virtual): the out-of-class
// definitions must match their declarations

#include <gtest/gtest.h>
#include <RobotRaconteur.h>

#include "experimental__gen_test_cpp_dforc.h"
#include "experimental__gen_test_cpp_dforc_stubskel.h"

using namespace RobotRaconteur;
using namespace experimental::gen_test_cpp_dforc;

namespace
{
class testobj_impl : public testobj_default_impl
{
  public:
    // The default implementation does not provide bidirectional wires
    virtual RR_SHARED_PTR<Wire<double> > get_virtual_() { return wire_virtual; }
    virtual void set_virtual_(const RR_SHARED_PTR<Wire<double> >& value) { wire_virtual = value; }

  protected:
    RR_SHARED_PTR<Wire<double> > wire_virtual;
};
} // namespace

TEST(RobotRaconteurGen, CppMemberNamesContainingVirtual)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    node->SetNodeName("gen_test_cpp_dforc");
    RR_SHARED_PTR<IntraTransport> t = RR_MAKE_SHARED<IntraTransport>(node);
    t->StartServer();
    node->RegisterTransport(t);
    node->RegisterServiceType(RR_MAKE_SHARED<experimental__gen_test_cpp_dforcFactory>());

    RR_SHARED_PTR<testobj_impl> obj = RR_MAKE_SHARED<testobj_impl>();
    node->RegisterService("test_service", "experimental.gen_test_cpp_dforc", obj);

    RR_SHARED_PTR<testobj> c =
        rr_cast<testobj>(node->ConnectService("rr+intra:///?nodename=gen_test_cpp_dforc&service=test_service"));

    c->set_is_virtual(3.5);
    EXPECT_EQ(c->get_is_virtual(), 3.5);
    EXPECT_TRUE(c->get_virtual_());

    node->DisconnectService(c);
    node->Shutdown();
}
