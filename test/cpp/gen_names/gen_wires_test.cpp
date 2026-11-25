// A readonly wire named operator and a writeonly wire named register: the default implementation's broadcaster /
// receiver members must have the same name in its header and definition

#include <gtest/gtest.h>
#include <RobotRaconteur.h>

#include "experimental__gen_test_cpp_wires.h"
#include "experimental__gen_test_cpp_wires_stubskel.h"

using namespace RobotRaconteur;
using namespace experimental::gen_test_cpp_wires;

namespace
{
class testobj_impl : public testobj_default_impl
{
};
} // namespace

TEST(RobotRaconteurGen, CppDefaultImplWireKeywordNames)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    node->SetNodeName("gen_test_cpp_wires");
    RR_SHARED_PTR<IntraTransport> t = RR_MAKE_SHARED<IntraTransport>(node);
    t->StartServer();
    node->RegisterTransport(t);
    node->RegisterServiceType(RR_MAKE_SHARED<experimental__gen_test_cpp_wiresFactory>());

    RR_SHARED_PTR<testobj_impl> obj = RR_MAKE_SHARED<testobj_impl>();
    node->RegisterService("test_service", "experimental.gen_test_cpp_wires", obj);

    RR_SHARED_PTR<testobj> c =
        rr_cast<testobj>(node->ConnectService("rr+intra:///?nodename=gen_test_cpp_wires&service=test_service"));

    EXPECT_TRUE(c->get_operator_());
    EXPECT_TRUE(c->get_register_());

    node->DisconnectService(c);
    node->Shutdown();
}
