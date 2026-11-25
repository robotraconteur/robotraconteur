// An enum value, a field and a parameter named constexpr / not_eq must be renamed in the generated C++

#include <gtest/gtest.h>
#include <RobotRaconteur.h>

#include "experimental__gen_test_cpp_keywords.h"
#include "experimental__gen_test_cpp_keywords_stubskel.h"

using namespace RobotRaconteur;
using namespace experimental::gen_test_cpp_keywords;

namespace
{
class testobj_impl : public testobj_default_impl
{
  public:
    virtual double f(double not_eq_) { return not_eq_ * 2; }
};
} // namespace

TEST(RobotRaconteurGen, CppKeywordNamesConstexprNotEq)
{
    keyword_enum::keyword_enum e = keyword_enum::not_eq_;
    EXPECT_EQ(static_cast<int>(e), 1);
    RR_SHARED_PTR<keyword_struct> s(new keyword_struct());
    s->constexpr_ = 1.5;
    s->not_eq_ = 2;

    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    node->SetNodeName("gen_test_cpp_keywords");
    RR_SHARED_PTR<IntraTransport> t = RR_MAKE_SHARED<IntraTransport>(node);
    t->StartServer();
    node->RegisterTransport(t);
    node->RegisterServiceType(RR_MAKE_SHARED<experimental__gen_test_cpp_keywordsFactory>());

    RR_SHARED_PTR<testobj_impl> obj = RR_MAKE_SHARED<testobj_impl>();
    node->RegisterService("test_service", "experimental.gen_test_cpp_keywords", obj);

    RR_SHARED_PTR<testobj> c =
        rr_cast<testobj>(node->ConnectService("rr+intra:///?nodename=gen_test_cpp_keywords&service=test_service"));

    EXPECT_EQ(c->f(1.5), 3.0);

    node->DisconnectService(c);
    node->Shutdown();
}
