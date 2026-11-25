// An event named throw (throw_ in C++): the event helpers in the skel and the default implementation must use one name

#include <gtest/gtest.h>
#include <RobotRaconteur.h>

#include "experimental__gen_test_cpp_events.h"
#include "experimental__gen_test_cpp_events_stubskel.h"

using namespace RobotRaconteur;
using namespace experimental::gen_test_cpp_events;

namespace
{
class testobj_impl : public testobj_default_impl
{
};

struct event_result
{
    boost::mutex lock;
    boost::condition_variable cv;
    bool done;
    double value;
    event_result() : done(false), value(0) {}
    void handler(double v)
    {
        boost::mutex::scoped_lock l(lock);
        value = v;
        done = true;
        cv.notify_all();
    }
};
} // namespace

TEST(RobotRaconteurGen, CppEventKeywordName)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    node->SetNodeName("gen_test_cpp_events");
    RR_SHARED_PTR<IntraTransport> t = RR_MAKE_SHARED<IntraTransport>(node);
    t->StartServer();
    node->RegisterTransport(t);
    node->RegisterServiceType(RR_MAKE_SHARED<experimental__gen_test_cpp_eventsFactory>());

    RR_SHARED_PTR<testobj_impl> obj = RR_MAKE_SHARED<testobj_impl>();
    node->RegisterService("test_service", "experimental.gen_test_cpp_events", obj);

    RR_SHARED_PTR<testobj> c =
        rr_cast<testobj>(node->ConnectService("rr+intra:///?nodename=gen_test_cpp_events&service=test_service"));

    event_result r;
    boost::signals2::connection ec =
        c->get_throw_().connect(boost::bind(&event_result::handler, &r, RR_BOOST_PLACEHOLDERS(_1)));
    obj->get_throw_()(4.5);
    {
        boost::mutex::scoped_lock l(r.lock);
        boost::posix_time::ptime end =
            boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(10);
        while (!r.done)
        {
            if (!r.cv.timed_wait(l, end))
                break;
        }
    }
    EXPECT_TRUE(r.done);
    EXPECT_EQ(r.value, 4.5);
    ec.disconnect();

    node->DisconnectService(c);
    node->Shutdown();
}
