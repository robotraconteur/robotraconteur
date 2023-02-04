/**
 * @file AsyncUtils.h
 *
 * @author John Wason, PhD
 *
 * @copyright Copyright 2011-2020 Wason Technology, LLC
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * @par
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <boost/bind/placeholders.hpp>
#include <boost/asio.hpp>
#include <boost/scope_exit.hpp>

#include "RobotRaconteur/Error.h"
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/ErrorUtil.h"
#include "RobotRaconteur/Timer.h"
#include "RobotRaconteur/AutoResetEvent.h"

#pragma once

namespace RobotRaconteur
{
class ROBOTRACONTEUR_CORE_API RobotRaconteurNode;

namespace detail
{
template <typename T>
class sync_async_handler : private boost::noncopyable
{
  public:
    RR_SHARED_PTR<AutoResetEvent> ev;
    RR_SHARED_PTR<RobotRaconteurException> err;
    RR_SHARED_PTR<T> data;
    boost::mutex data_lock;

    sync_async_handler() { ev = RR_MAKE_SHARED<AutoResetEvent>(); }

    sync_async_handler(const RR_SHARED_PTR<RobotRaconteurException>& err)
    {
        ev = RR_MAKE_SHARED<AutoResetEvent>();
        this->err = err;
    }

    void operator()() { ev->Set(); }

    void operator()(const RR_SHARED_PTR<RobotRaconteurException>& err)
    {
        boost::mutex::scoped_lock lock(data_lock);
        this->err = err;
        ev->Set();
    }

    void operator()(const RR_SHARED_PTR<T>& data, const RR_SHARED_PTR<RobotRaconteurException>& err)
    {
        boost::mutex::scoped_lock lock(data_lock);
        this->err = err;
        this->data = data;
        ev->Set();
    }

    RR_SHARED_PTR<T> end()
    {
        ev->WaitOne();

        boost::mutex::scoped_lock lock(data_lock);
        if (err)
        {
            RobotRaconteurExceptionUtil::DownCastAndThrowException(err);
        }

        if (!data)
            throw InternalErrorException("Internal async error");

        return data;
    }

    void end_void()
    {
        ev->WaitOne();

        boost::mutex::scoped_lock lock(data_lock);
        if (err)
        {
            RobotRaconteurExceptionUtil::DownCastAndThrowException(err);
        }
    }

    bool try_end(RR_SHARED_PTR<T>& res, RR_SHARED_PTR<RobotRaconteurException>& err_out)
    {
        ev->WaitOne();

        boost::mutex::scoped_lock lock(data_lock);
        if (err)
        {
            err_out = err;
            return false;
        }

        if (!data)
        {
            err_out = RR_MAKE_SHARED<InternalErrorException>("Internal async error");
            return false;
        }

        res = data;
        return true;
    }

    bool try_end_void(RR_SHARED_PTR<RobotRaconteurException>& err_out)
    {
        ev->WaitOne();

        boost::mutex::scoped_lock lock(data_lock);
        if (err)
        {
            err_out = err;
            return false;
        }

        return true;
    }
};

RR_SHARED_PTR<Timer> async_timeout_wrapper_CreateTimer(const RR_SHARED_PTR<RobotRaconteurNode>& node,
                                                       const boost::posix_time::time_duration& period,
                                                       RR_MOVE_ARG(boost::function<void(const TimerEvent&)>) handler,
                                                       bool oneshot);

template <typename T>
void async_timeout_wrapper_closer(const RR_SHARED_PTR<T>& d)
{
    try
    {
        d->Close();
    }
    catch (std::exception&)
    {}
}

template <typename T, typename T2>
void async_timeout_wrapper_closer(const RR_SHARED_PTR<T>& d)
{
    try
    {
        RR_SHARED_PTR<T2> t2 = RR_DYNAMIC_POINTER_CAST<T2>(d);
        if (!t2)
            return;
        t2->Close();
    }
    catch (std::exception&)
    {}
}

template <typename T>
class async_timeout_wrapper : public RR_ENABLE_SHARED_FROM_THIS<async_timeout_wrapper<T> >, private boost::noncopyable
{
  private:
    boost::function<void(const RR_SHARED_PTR<T>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler_;
    RR_SHARED_PTR<Timer> timeout_timer_;
    boost::mutex handled_lock;
    bool handled;
    RR_SHARED_PTR<RobotRaconteurException> timeout_exception_;
    boost::function<void(const RR_SHARED_PTR<T>&)> deleter_;
    RR_WEAK_PTR<RobotRaconteurNode> node;

  public:
    async_timeout_wrapper(
        const RR_SHARED_PTR<RobotRaconteurNode>& node,
        boost::function<void(const RR_SHARED_PTR<T>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
        RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<T>&)>) deleter = 0)
        : handler_(handler), handled(false), node(node)
    {
        deleter_.swap(deleter);
    }

    void start_timer(int32_t timeout, const RR_SHARED_PTR<RobotRaconteurException>& timeout_exception =
                                          RR_MAKE_SHARED<ConnectionException>("Timeout during operation"));

    void operator()(const RR_SHARED_PTR<T>& data, const RR_SHARED_PTR<RobotRaconteurException>& err)
    {
        {
            boost::mutex::scoped_lock lock(handled_lock);
            if (handled)
            {
                if (data && deleter_)
                    deleter_(data);
                return;
            }
            handled = true;

            try
            {
                if (timeout_timer_)
                    timeout_timer_->Stop();
            }
            catch (std::exception&)
            {}
            timeout_timer_.reset();
        }

        handler_(data, err);
    }

    void handle_error(const RR_SHARED_PTR<RobotRaconteurException>& err)
    {
        {
            boost::mutex::scoped_lock lock(handled_lock);
            if (handled)
                return;
            handled = true;

            {

                try
                {
                    if (timeout_timer_)
                        timeout_timer_->Stop();
                }
                catch (std::exception&)
                {}
                timeout_timer_.reset();
            }
        }

        handler_(RR_SHARED_PTR<T>(), err);
    }

    void handle_error(const boost::system::error_code& err)
    {
        if (err.value() == boost::system::errc::timed_out)
            handle_error(timeout_exception_);
        handle_error(RR_MAKE_SHARED<ConnectionException>(err.message()));
    }

  private:
    void timeout_handler(const TimerEvent& /*e*/)
    {
        {
            boost::mutex::scoped_lock lock(handled_lock);
            if (handled)
                return;
            handled = true;
            //	timeout_timer_.reset();

            timeout_timer_.reset();
        }

        handler_(RR_SHARED_PTR<T>(), timeout_exception_);
    }
};

template <typename T>
void async_timeout_wrapper<T>::start_timer(int32_t timeout,
                                           const RR_SHARED_PTR<RobotRaconteurException>& timeout_exception)
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");

    boost::mutex::scoped_lock lock(handled_lock);

    if (handled)
        return;

    if (timeout != RR_TIMEOUT_INFINITE)
    {
        timeout_timer_ =
            async_timeout_wrapper_CreateTimer(n, boost::posix_time::milliseconds(timeout),
                                              boost::bind(&async_timeout_wrapper<T>::timeout_handler,
                                                          this->shared_from_this(), RR_BOOST_PLACEHOLDERS(_1)),
                                              true);
        timeout_timer_->Start();
        timeout_exception_ = RR_MOVE(timeout_exception);
    }
}

/*template<typename Handler>
class handler_move_wrapper
{
public:
    handler_move_wrapper(Handler& handler)
        : handler_(RR_MOVE(handler))
    {}


    Handler&& operator()
    {
        return RR_MOVE(handler_);
    }

protected:
    Handler handler_;
};

template<typename Handler>
handler_move_wrapper<Handler> make_handler_move_wrapper(Handler& handler)
{
    return handler_move_wrapper<Handler>(handler);
}*/

class ROBOTRACONTEUR_CORE_API async_signal_semaphore : private boost::noncopyable
{
  protected:
    boost::mutex this_lock;
    boost::condition_variable next_wait;
    boost::initialized<bool> running;
    boost::initialized<bool> next;
    boost::initialized<uint64_t> next_id;

  public:
    template <typename F>
    bool try_fire_next(BOOST_ASIO_MOVE_ARG(F) h)
    {
        boost::mutex::scoped_lock lock(this_lock);

        if (running || next)
        {
            uint64_t my_id = ++next_id.data();

            if (next.data())
            {
                next_wait.notify_all();
            }
            else
            {
                next.data() = true;
            }

            while (running)
            {
                next_wait.wait(lock);
                if (my_id != next_id)
                    return false;
            }

            next.data() = false;
        }

        running.data() = true;

        BOOST_SCOPE_EXIT_TPL(this_)
        {
            boost::mutex::scoped_lock lock2(this_->this_lock);
            this_->running.data() = false;
            this_->next_wait.notify_all();
        }
        BOOST_SCOPE_EXIT_END;

        lock.unlock();

        h();

        return true;
    }
};

class ROBOTRACONTEUR_CORE_API async_signal_pool_semaphore
    : public RR_ENABLE_SHARED_FROM_THIS<async_signal_pool_semaphore>,
      private boost::noncopyable
{
  protected:
    boost::mutex this_lock;
    boost::initialized<bool> running;
    boost::function<void()> next;
    RR_WEAK_PTR<RobotRaconteurNode> node;

  public:
    async_signal_pool_semaphore(const RR_SHARED_PTR<RobotRaconteurNode>& node) : node(node) {}

    template <typename F>
    void try_fire_next(BOOST_ASIO_MOVE_ARG(F) h)
    {
        boost::mutex::scoped_lock lock(this_lock);

        if (!running)
        {
            RR_SHARED_PTR<RobotRaconteurNode> node1 = this->node.lock();
            if (!node1)
                return;
            do_post(node1, h);
        }
        else
        {
            next = h;
        }
    }

  protected:
    void do_fire_next(const boost::function<void()>& h)
    {
        try
        {
            h();
        }
        catch (std::exception& exp)
        {
            handle_exception(&exp);
        }

        BOOST_SCOPE_EXIT(this_)
        {
            boost::mutex::scoped_lock lock2(this_->this_lock);

            boost::function<void()> h2;
            h2.swap(this_->next);
            this_->next.clear();
            this_->running.data() = false;
            if (!h2)
                return;
            RR_SHARED_PTR<RobotRaconteurNode> node = this_->node.lock();
            if (!node)
                return;
            try
            {
                this_->do_post(node,
                               boost::bind(&async_signal_pool_semaphore::do_fire_next, this_->shared_from_this(), h2));
            }
            catch (std::exception&)
            {}
            this_->running.data() = true;
        }
        BOOST_SCOPE_EXIT_END;
    }

    void do_post(const RR_SHARED_PTR<RobotRaconteurNode>& node1, RR_MOVE_ARG(boost::function<void()>) h);

    void handle_exception(std::exception* exp);
};

ROBOTRACONTEUR_CORE_API void InvokeHandler_HandleException(RR_WEAK_PTR<RobotRaconteurNode> node, std::exception& exp);

ROBOTRACONTEUR_CORE_API void InvokeHandler_DoPost(RR_WEAK_PTR<RobotRaconteurNode> node,
                                                  const boost::function<void()>& h, bool shutdown_op = false,
                                                  bool throw_on_released = true);

ROBOTRACONTEUR_CORE_API void InvokeHandler(RR_WEAK_PTR<RobotRaconteurNode> node,
                                           const boost::function<void()>& handler);

template <typename T>
void InvokeHandler(RR_WEAK_PTR<RobotRaconteurNode> node, const typename boost::function<void(const T&)>& handler,
                   const T& value)
{
    try
    {
        handler(value);
    }
    catch (std::exception& exp)
    {
        InvokeHandler_HandleException(RR_MOVE(node), exp);
    }
}

ROBOTRACONTEUR_CORE_API void InvokeHandler(
    RR_WEAK_PTR<RobotRaconteurNode> node,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

ROBOTRACONTEUR_CORE_API void InvokeHandlerWithException(
    RR_WEAK_PTR<RobotRaconteurNode> node,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
    const RR_SHARED_PTR<RobotRaconteurException>& exp);

ROBOTRACONTEUR_CORE_API void InvokeHandlerWithException(
    RR_WEAK_PTR<RobotRaconteurNode> node,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler, std::exception& exp,
    MessageErrorType default_err = MessageErrorType_UnknownError);

template <typename T>
void InvokeHandler(
    RR_WEAK_PTR<RobotRaconteurNode> node,
    const typename boost::function<void(const T&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
    const T& value)
{
    try
    {
        handler(value, RR_SHARED_PTR<RobotRaconteurException>());
    }
    catch (std::exception& exp)
    {
        InvokeHandler_HandleException(RR_MOVE(node), exp);
    }
}

template <typename T>
void InvokeHandlerWithException(
    RR_WEAK_PTR<RobotRaconteurNode> node,
    const typename boost::function<void(T, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
    const RR_SHARED_PTR<RobotRaconteurException>& exp)
{
    typename boost::initialized<typename boost::remove_reference<T>::type> default_value;
    try
    {
        handler(default_value, exp);
    }
    catch (std::exception& exp)
    {
        InvokeHandler_HandleException(RR_MOVE(node), exp);
    }
}

template <typename T>
void InvokeHandlerWithException(
    RR_WEAK_PTR<RobotRaconteurNode> node,
    const typename boost::function<void(T, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
    std::exception& exp, MessageErrorType default_err = MessageErrorType_UnknownError)
{
    typename boost::initialized<typename boost::remove_const<typename boost::remove_reference<T>::type>::type>
        default_value;
    try
    {
        RR_SHARED_PTR<RobotRaconteurException> err =
            RobotRaconteurExceptionUtil::ExceptionToSharedPtr(exp, default_err);
        handler(default_value, err);
    }
    catch (std::exception& exp)
    {
        InvokeHandler_HandleException(RR_MOVE(node), exp);
    }
}

ROBOTRACONTEUR_CORE_API void PostHandler(RR_WEAK_PTR<RobotRaconteurNode> node, const boost::function<void()>& handler,
                                         bool shutdown_op = false, bool throw_on_released = true);

template <typename T>
void PostHandler(RR_WEAK_PTR<RobotRaconteurNode> node, const typename boost::function<void(const T&)>& handler,
                 const T& value, bool shutdown_op = false, bool throw_on_released = true)
{
    boost::function<void()> h = boost::bind(handler, value);
    InvokeHandler_DoPost(RR_MOVE(node), h, shutdown_op, throw_on_released);
}

ROBOTRACONTEUR_CORE_API void PostHandler(
    RR_WEAK_PTR<RobotRaconteurNode> node,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler, bool shutdown_op = false,
    bool throw_on_released = true);

ROBOTRACONTEUR_CORE_API void PostHandlerWithException(
    RR_WEAK_PTR<RobotRaconteurNode> node,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
    const RR_SHARED_PTR<RobotRaconteurException>& exp, bool shutdown_op = false, bool throw_on_released = true);

ROBOTRACONTEUR_CORE_API void PostHandlerWithException(
    RR_WEAK_PTR<RobotRaconteurNode> node,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler, std::exception& exp,
    MessageErrorType default_err = MessageErrorType_UnknownError, bool shutdown_op = false,
    bool throw_on_released = true);

template <typename T>
void PostHandler(RR_WEAK_PTR<RobotRaconteurNode> node,
                 const typename boost::function<void(const T&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
                 const T& value, bool shutdown_op = false, bool throw_on_released = true)
{
    boost::function<void()> h = boost::bind(handler, value, RR_SHARED_PTR<RobotRaconteurException>());
    InvokeHandler_DoPost(RR_MOVE(node), h, shutdown_op, throw_on_released);
}

template <typename T>
void PostHandlerWithException(
    RR_WEAK_PTR<RobotRaconteurNode> node,
    const typename boost::function<void(T, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
    const RR_SHARED_PTR<RobotRaconteurException>& exp, bool shutdown_op = false, bool throw_on_released = true)
{
    typename boost::initialized<typename boost::remove_reference<T>::type> default_value;
    boost::function<void()> h = boost::bind(handler, default_value, exp);
    InvokeHandler_DoPost(RR_MOVE(node), h, shutdown_op, throw_on_released);
}

template <typename T>
void PostHandlerWithException(
    RR_WEAK_PTR<RobotRaconteurNode> node,
    const typename boost::function<void(T, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
    std::exception& exp, MessageErrorType default_err = MessageErrorType_UnknownError, bool shutdown_op = false,
    bool throw_on_released = true)
{
    typename boost::initialized<typename boost::remove_reference<T>::type> default_value;
    RR_SHARED_PTR<RobotRaconteurException> err = RobotRaconteurExceptionUtil::ExceptionToSharedPtr(exp, default_err);
    boost::function<void()> h = boost::bind(handler, default_value, err);
    InvokeHandler_DoPost(RR_MOVE(node), h, shutdown_op, throw_on_released);
}

} // namespace detail
} // namespace RobotRaconteur
