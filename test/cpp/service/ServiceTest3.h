#include "com__robotraconteur__testing__TestService5_stubskel.h"

#pragma once

#include "CompareArray.h"

using namespace RobotRaconteur;

namespace RobotRaconteurTest
{
    class asynctestroot_impl;

    class RobotRaconteurTestService3Support
	{
	public:
		void RegisterServices(RR_SHARED_PTR<RobotRaconteurNode> node=RR_SHARED_PTR<RobotRaconteurNode>());
		void UnregisterServices();
	protected:

		RR_SHARED_PTR<asynctestroot_impl> testservice3;
	};

    class asynctestroot_impl : public virtual com::robotraconteur::testing::TestService5::asynctestroot_default_impl,
        public virtual com::robotraconteur::testing::TestService5::async_asynctestroot,
		public RR_ENABLE_SHARED_FROM_THIS<asynctestroot_impl>
	{
	public:
		asynctestroot_impl();

        RR_OVIRTUAL void async_get_d1(boost::function<void (double,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;
        RR_OVIRTUAL void async_set_d1(double value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL void async_get_s1(boost::function<void (const std::string&,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;
        RR_OVIRTUAL void async_set_s1(const std::string& value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL void async_get_d2(boost::function<void (const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > >&,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;
        RR_OVIRTUAL void async_set_d2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > >& value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL void async_get_s2(boost::function<void (const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<char>  > >&,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;
        RR_OVIRTUAL void async_set_s2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<char>  > >& value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL void async_get_s3(boost::function<void (const RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService5::teststruct4 >&,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;
        RR_OVIRTUAL void async_set_s3(const RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService5::teststruct4 >& value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL void async_f1(boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL void async_f2(int32_t a,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL void async_f3(double a, double b,boost::function<void (int32_t, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL void async_f4(boost::function<void (RR_SHARED_PTR<RobotRaconteur::Generator<double,void > >, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL void async_f5(double a,boost::function<void (RR_SHARED_PTR<RobotRaconteur::Generator<void,double > >, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL void async_f6(double a,boost::function<void (RR_SHARED_PTR<RobotRaconteur::Generator<RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService5::teststruct4 >,RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService5::teststruct5 > > >, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL void async_get_err(boost::function<void (double,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;
        RR_OVIRTUAL void async_set_err(double value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL void async_err_func(boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL void async_err_func2(boost::function<void (double, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL void async_err_func3(boost::function<void (RR_SHARED_PTR<RobotRaconteur::Generator<double,void > >, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE ;

        RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Generator<double,void > > err_func3() RR_OVERRIDE ;


    };
}