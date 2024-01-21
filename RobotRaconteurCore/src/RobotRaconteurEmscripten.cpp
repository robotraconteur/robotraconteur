#include "RobotRaconteur/RobotRaconteurEmscripten.h"

namespace boost
{
namespace asio
{
    void io_service::post(boost::function<void()> f)
    {

    }

    deadline_timer::deadline_timer(io_service& service)
    {

    }

    void deadline_timer::expires_from_now(boost::posix_time::time_duration duration)
    {

    }

    boost::posix_time::time_duration deadline_timer::expires_from_now()
    {
        throw std::runtime_error("Not implemented");
    }

    void deadline_timer::expires_at(boost::posix_time::ptime duration)
    {

    }

    void deadline_timer::async_wait(boost::function<void(boost::system::error_code)> f)
    {

    }

    void deadline_timer::cancel()
    {

    }

    void deadline_timer::cancel(boost::system::error_code ec)
    {

    }

    void deadline_timer::wait()
    {

    }
}

namespace this_thread
{
    int get_id()
    {
        return 1;
    }
}

}