#pragma once

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/function.hpp>
#include <boost/system/error_code.hpp>

namespace RobotRaconteur
{
class WallTimer;
}

namespace boost
{
template <typename T>
class null_lock
{
  public:
    null_lock() {}
    template <typename U>
    null_lock(U& u)
    {}
    void lock() {}
    void unlock() {}

    template <typename U>
    void swap(U& l)
    {}
};

template <typename T>
using unique_lock = null_lock<T>;
template <typename T>
using shared_lock = null_lock<T>;
template <typename T>
using upgrade_lock = null_lock<T>;
template <typename T>
using upgrade_to_unique_lock = null_lock<T>;

class null_mutex
{
  public:
    null_mutex() {}

    void lock() {}
    void notify_all() {}
    void unlock() {}
    void notify_one() {}

    template <typename U>
    void wait(U& u)
    {
        throw std::runtime_error("Operation requires threading");
    }

    template <typename U, typename V>
    void timed_wait(U& u, V v)
    {
        throw std::runtime_error("Operation requires threading");
    }

    template <typename U, typename V>
    void wait_for(U& u, V v)
    {
        throw std::runtime_error("Operation requires threading");
    }

    typedef unique_lock<null_mutex> scoped_lock;
};

typedef boost::null_mutex mutex;
typedef boost::null_mutex recursive_mutex;
typedef boost::null_mutex shared_mutex;
typedef boost::null_mutex condition_variable;

namespace asio
{
class strand
{
  public:
    template <typename T>
    strand(T& t)
    {}
};
class io_service_work
{
  public:
    template <typename U>
    io_service_work(U& u)
    {}
};
class io_service
{
  public:
    typedef io_service_work work;
    typedef strand strand;
    void run_one() {}
    bool stopped() { return false; }
    void stop() {}
    void post(boost::function<void()> f);
};
class deadline_timer
{
  public:
    deadline_timer(io_service& service);
    deadline_timer(io_service& service, boost::posix_time::time_duration duration);
    ~deadline_timer();
    void expires_from_now(boost::posix_time::time_duration duration);
    boost::posix_time::time_duration expires_from_now();
    void expires_at(boost::posix_time::ptime duration);
    void async_wait(boost::function<void(boost::system::error_code)> f);
    void cancel();
    void cancel(boost::system::error_code ec);
    void wait();

  protected:
    boost::shared_ptr<RobotRaconteur::WallTimer> timer;
    double next_timeout;
};
} // namespace asio

template <typename T>
class thread_specific_ptr
{
    T* val_;

  public:
    thread_specific_ptr() : val_(0) {}
    thread_specific_ptr(T* val) : val_(val) {}

    void reset(T* val)
    {
        if (val_)
        {
            delete val_;
        }
        val_ = val;
    }

    void reset() { reset(0); }

    T* get() { return val_; }

    T& operator*() { return *val_; }
};

class thread
{
  public:
    thread() {}
    template <typename U>
    thread(U u)
    {}

    void join() {}
};

typedef boost::posix_time::ptime system_time;

namespace this_thread
{
template <typename T>
static void sleep(T& t)
{
    throw std::runtime_error("Operation requires threading");
}

int get_id();
} // namespace this_thread
} // namespace boost

#undef RR_BOOST_ASIO_STRAND_WRAP
#define RR_BOOST_ASIO_STRAND_WRAP(strand, f) (f)
