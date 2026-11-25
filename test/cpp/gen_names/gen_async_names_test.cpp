// A property named delete (delete_ in C++): the asynchronous get / set stubs must send the member name from the
// service definition, like the synchronous ones

#include <gtest/gtest.h>
#include <RobotRaconteur.h>

#include "experimental__gen_test_cpp_async_names.h"
#include "experimental__gen_test_cpp_async_names_stubskel.h"

using namespace RobotRaconteur;
using namespace experimental::gen_test_cpp_async_names;

namespace
{
class testobj_impl : public testobj_default_impl
{
};

struct async_result
{
    boost::mutex lock;
    boost::condition_variable cv;
    bool done;
    double value;
    RR_SHARED_PTR<RobotRaconteurException> err;

    async_result() : done(false), value(0) {}

    void get_handler(double v, const RR_SHARED_PTR<RobotRaconteurException>& e)
    {
        boost::mutex::scoped_lock l(lock);
        value = v;
        err = e;
        done = true;
        cv.notify_all();
    }

    void set_handler(const RR_SHARED_PTR<RobotRaconteurException>& e)
    {
        boost::mutex::scoped_lock l(lock);
        err = e;
        done = true;
        cv.notify_all();
    }

    bool wait()
    {
        boost::mutex::scoped_lock l(lock);
        boost::posix_time::ptime end =
            boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(10);
        while (!done)
        {
            if (!cv.timed_wait(l, end))
                return done;
        }
        return true;
    }
};
} // namespace

TEST(RobotRaconteurGen, CppAsyncPropertyKeywordName)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    node->SetNodeName("gen_test_cpp_async_names");
    RR_SHARED_PTR<IntraTransport> t = RR_MAKE_SHARED<IntraTransport>(node);
    t->StartServer();
    node->RegisterTransport(t);
    node->RegisterServiceType(RR_MAKE_SHARED<experimental__gen_test_cpp_async_namesFactory>());

    RR_SHARED_PTR<testobj_impl> obj = RR_MAKE_SHARED<testobj_impl>();
    node->RegisterService("test_service", "experimental.gen_test_cpp_async_names", obj);

    RR_SHARED_PTR<testobj> c =
        rr_cast<testobj>(node->ConnectService("rr+intra:///?nodename=gen_test_cpp_async_names&service=test_service"));

    c->set_delete_(1.5);
    EXPECT_EQ(c->get_delete_(), 1.5);

    RR_SHARED_PTR<async_testobj> c_async = rr_cast<async_testobj>(c);

    async_result r1;
    c_async->async_set_delete_(2.5, boost::bind(&async_result::set_handler, &r1, RR_BOOST_PLACEHOLDERS(_1)));
    ASSERT_TRUE(r1.wait());
    EXPECT_FALSE(r1.err) << r1.err->what();

    async_result r2;
    c_async->async_get_delete_(
        boost::bind(&async_result::get_handler, &r2, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));
    ASSERT_TRUE(r2.wait());
    EXPECT_FALSE(r2.err) << r2.err->what();
    EXPECT_EQ(r2.value, 2.5);

    node->DisconnectService(c);
    node->Shutdown();
}
