#include "ServiceTest3.h"

namespace RobotRaconteurTest
{
    void RobotRaconteurTestService3Support::RegisterServices(RR_SHARED_PTR<RobotRaconteurNode> node)
	{
        if (!node)
        {
            node = RobotRaconteurNode::sp();
        }
		testservice3 = RR_MAKE_SHARED<asynctestroot_impl>();
		RR_SHARED_PTR<ServerContext> c = node->RegisterService("RobotRaconteurTestService3", "com.robotraconteur.testing.TestService5", testservice3);
	}

	void RobotRaconteurTestService3Support::UnregisterServices()
	{
		RobotRaconteurNode::s()->CloseService("RobotRaconteurTestService3");
	}


	asynctestroot_impl::asynctestroot_impl()
	{

    }

    void asynctestroot_impl::async_get_d1(boost::function<void (double,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        RobotRaconteurNode::TryPostToThreadPool(RobotRaconteurNode::sp(), boost::bind(rr_handler,8.5515,RR_SHARED_PTR<RobotRaconteurException>()));
    }

    void asynctestroot_impl::async_set_d1(double value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        if (value == 3.0819)
        {
            RobotRaconteurNode::TryPostToThreadPool(RobotRaconteurNode::sp(), boost::bind(rr_handler,RR_SHARED_PTR<RobotRaconteurException>()));
            return;
        }
        RobotRaconteurNode::TryPostToThreadPool(RobotRaconteurNode::sp(), boost::bind(rr_handler,RR_MAKE_SHARED<InvalidArgumentException>("")));
    }

    void asynctestroot_impl::async_get_s1(boost::function<void (const std::string&,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        throw RobotRaconteur::NotImplementedException("");
    }

    void asynctestroot_impl::async_set_s1(const std::string& value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        throw RobotRaconteur::NotImplementedException("");
    }

    void asynctestroot_impl::async_get_d2(boost::function<void (const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > >&,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        throw RobotRaconteur::NotImplementedException("");
    }

    void asynctestroot_impl::async_set_d2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > >& value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        throw RobotRaconteur::NotImplementedException("");
    }
    
    void asynctestroot_impl::async_get_s2(boost::function<void (const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<char>  > >&,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        throw RobotRaconteur::NotImplementedException("");
    }

    void asynctestroot_impl::async_set_s2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<char>  > >& value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        throw RobotRaconteur::NotImplementedException("");
    }
    
    void asynctestroot_impl::async_get_s3(boost::function<void (const RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService5::teststruct4 >&,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        throw RobotRaconteur::NotImplementedException("");
    }

    void asynctestroot_impl::async_set_s3(const RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService5::teststruct4 >& value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        throw RobotRaconteur::NotImplementedException("");
    }

    void asynctestroot_impl::async_f1(boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        RobotRaconteurNode::TryPostToThreadPool(RobotRaconteurNode::sp(), boost::bind(rr_handler,RR_SHARED_PTR<RobotRaconteurException>()));
    }

    void asynctestroot_impl::async_f2(int32_t a,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        if (a == 247)
        {
            RobotRaconteurNode::TryPostToThreadPool(RobotRaconteurNode::sp(), boost::bind(rr_handler,RR_SHARED_PTR<RobotRaconteurException>()));
            return;
        }
        RobotRaconteurNode::TryPostToThreadPool(RobotRaconteurNode::sp(), boost::bind(rr_handler,RR_MAKE_SHARED<InvalidArgumentException>("")));
    }

    void asynctestroot_impl::async_f3(double a, double b,boost::function<void (int32_t, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        int32_t res = boost::numeric_cast<int32_t>(a + b);
        RobotRaconteurNode::TryPostToThreadPool(RobotRaconteurNode::sp(), boost::bind(rr_handler,res,RR_SHARED_PTR<RobotRaconteurException>()));
    }

    void asynctestroot_impl::async_f4(boost::function<void (RR_SHARED_PTR<RobotRaconteur::Generator<double,void > >, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        throw RobotRaconteur::NotImplementedException("");
    }

    void asynctestroot_impl::async_f5(double a,boost::function<void (RR_SHARED_PTR<RobotRaconteur::Generator<void,double > >, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        throw RobotRaconteur::NotImplementedException("");
    }

    void asynctestroot_impl::async_f6(double a,boost::function<void (RR_SHARED_PTR<RobotRaconteur::Generator<RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService5::teststruct4 >,RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService5::teststruct5 > > >, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        throw RobotRaconteur::NotImplementedException("");
    }

    void asynctestroot_impl::async_get_err(boost::function<void (double,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        RobotRaconteurNode::TryPostToThreadPool(RobotRaconteurNode::sp(), boost::bind(rr_handler,0.0,RR_MAKE_SHARED<InvalidArgumentException>("Test message 1")));
    }

    void asynctestroot_impl::async_set_err(double value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        RobotRaconteurNode::TryPostToThreadPool(RobotRaconteurNode::sp(), boost::bind(rr_handler,RR_MAKE_SHARED<InvalidOperationException>("Test message 2")));
    }


    void asynctestroot_impl::async_err_func(boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        RobotRaconteurNode::TryPostToThreadPool(RobotRaconteurNode::sp(), boost::bind(rr_handler,RR_MAKE_SHARED<InvalidOperationException>("")));
    }

    void asynctestroot_impl::async_err_func2(boost::function<void (double, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        RobotRaconteurNode::TryPostToThreadPool(RobotRaconteurNode::sp(), boost::bind(rr_handler,0.0,RR_MAKE_SHARED<com::robotraconteur::testing::TestService5::asynctestexp>("")));
    }

    void asynctestroot_impl::async_err_func3(boost::function<void (RR_SHARED_PTR<RobotRaconteur::Generator<double,void > >, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout)
    {
        throw NotImplementedException("");        
    }

    RR_SHARED_PTR<RobotRaconteur::Generator<double,void > > asynctestroot_impl::err_func3()
    {
        throw InvalidOperationException("");        
    }

}