#include "RobotRaconteur/RobotRaconteurEmscripten.h"

#include <emscripten/html5.h>
#include <emscripten/emscripten.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "RobotRaconteur/Timer.h"

namespace boost
{
namespace asio
{

static void post_timeout_handler(void* user_data)
{
    boost::function<void()> f;
    boost::function<void()>* f_ptr = reinterpret_cast<boost::function<void()>*>(user_data);
    f_ptr->swap(f);
    delete f_ptr;
    f();
}

void io_service::post(boost::function<void()> f)
{
    auto f_ptr = new boost::function<void()>();
    f_ptr->swap(f);
    emscripten_set_timeout(&post_timeout_handler, 0, f_ptr);
}

deadline_timer::deadline_timer(io_service& service) { next_timeout = 0.0; }

deadline_timer::deadline_timer(io_service& service, boost::posix_time::time_duration duration)
{
    next_timeout = emscripten_get_now() + (((double)duration.total_microseconds()) * 1e-3);
}

deadline_timer::~deadline_timer()
{
    if (timer)
    {
        timer->TryStop();
    }
}

void deadline_timer::expires_from_now(boost::posix_time::time_duration duration)
{
    next_timeout = emscripten_get_now() + (((double)duration.total_microseconds()) * 1e-3);
}

boost::posix_time::time_duration deadline_timer::expires_from_now()
{
    double interval = next_timeout - emscripten_get_now();
    return boost::posix_time::microseconds((uint64_t)(interval * 1e3));
}

void deadline_timer::expires_at(boost::posix_time::ptime time)
{
    double interval =
        ((double)(time - boost::posix_time::microsec_clock::universal_time()).total_microseconds()) * 1e-3;
    next_timeout = interval + emscripten_get_now();
}

void deadline_timer::async_wait(boost::function<void(boost::system::error_code)> f)
{
    if (timer)
    {
        timer->TryStop();
        timer.reset();
    }

    int32_t interval = boost::numeric_cast<int32_t>((next_timeout - emscripten_get_now()) * 1e3);
    if (interval < 0)
    {
        interval = 0;
    }

    timer = boost::make_shared<RobotRaconteur::WallTimer>(
        boost::posix_time::microseconds(interval),
        [f](const RobotRaconteur::TimerEvent& ev) {
            boost::system::error_code ec;
            if (ev.stopped)
            {
                ec = boost::asio::error::make_error_code(boost::asio::error::operation_aborted);
            }
            f(ec);
        },
        true, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurNode>());
}

void deadline_timer::cancel()
{
    if (timer)
    {
        timer->TryStop();
    }
}

void deadline_timer::cancel(boost::system::error_code ec)
{
    if (timer)
    {
        timer->TryStop();
    }
}

void deadline_timer::wait()
{
    throw std::runtime_error("deadline_timer::wait() not supported for single threaded emscripten");
}
} // namespace asio

namespace this_thread
{
int get_id() { return 1; }
} // namespace this_thread

} // namespace boost