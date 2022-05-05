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
    void RegisterServices();
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

    virtual void async_get_d1(
        boost::function<void(double, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE);
    virtual void async_set_d1(double value,
                              boost::function<void(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> rr_handler,
                              int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual void async_get_s1(
        boost::function<void(const std::string&, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE);
    virtual void async_set_s1(const std::string& value,
                              boost::function<void(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> rr_handler,
                              int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual void async_get_d2(boost::function<void(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> >,
                                                   RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)>
                                  rr_handler,
                              int32_t rr_timeout = RR_TIMEOUT_INFINITE);
    virtual void async_set_d2(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > value,
                              boost::function<void(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> rr_handler,
                              int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual void async_get_s2(
        boost::function<void(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<char> > >,
                             RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)>
            rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE);
    virtual void async_set_s2(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<char> > > value,
                              boost::function<void(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> rr_handler,
                              int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual void async_get_s3(
        boost::function<void(RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService5::teststruct4>,
                             RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)>
            rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE);
    virtual void async_set_s3(RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService5::teststruct4> value,
                              boost::function<void(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> rr_handler,
                              int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual void async_f1(boost::function<void(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> rr_handler,
                          int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual void async_f2(int32_t a,
                          boost::function<void(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> rr_handler,
                          int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual void async_f3(
        double a, double b,
        boost::function<void(int32_t, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual void async_f4(boost::function<void(RR_SHARED_PTR<RobotRaconteur::Generator<double, void> >,
                                               RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)>
                              rr_handler,
                          int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual void async_f5(double a,
                          boost::function<void(RR_SHARED_PTR<RobotRaconteur::Generator<void, double> >,
                                               RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)>
                              rr_handler,
                          int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual void async_f6(
        double a,
        boost::function<void(RR_SHARED_PTR<RobotRaconteur::Generator<
                                 RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService5::teststruct4>,
                                 RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService5::teststruct5> > >,
                             RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)>
            rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual void async_get_err(
        boost::function<void(double, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE);
    virtual void async_set_err(double value,
                               boost::function<void(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> rr_handler,
                               int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual void async_err_func(
        boost::function<void(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual void async_err_func2(
        boost::function<void(double, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual void async_err_func3(boost::function<void(RR_SHARED_PTR<RobotRaconteur::Generator<double, void> >,
                                                      RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)>
                                     rr_handler,
                                 int32_t rr_timeout = RR_TIMEOUT_INFINITE);

    virtual RR_SHARED_PTR<RobotRaconteur::Generator<double, void> > err_func3();
};
} // namespace RobotRaconteurTest