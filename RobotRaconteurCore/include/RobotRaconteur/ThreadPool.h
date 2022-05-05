/**
 * @file ThreadPool.h
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
#include "RobotRaconteur/DataTypes.h"

#pragma once

namespace RobotRaconteur
{
class ROBOTRACONTEUR_CORE_API RobotRaconteurNode;

/**
 * @brief Thread pool for Robot Raconteur nodes
 *
 * See \ref threading for more information on threading.
 *
 * Robot Raconteur uses a Boost.Asio IO Context with a thread pool to handle
 * network and other events. ThreadPool implements a multi-threaded
 * pool that by default starts 20 threads. These threads wait for events
 * and are dispatched by IO Context.
 *
 * ThreadPool is normally created by the node using ThreadPoolFactory. The
 * factory is configured using RobotRaconteurNode::SetThreadPoolFactory(). The
 * thread pool may optionally be configured using the
 * RobotRaconteurNode::SetThreadPool() if the thread pool is created directly.
 *
 * The boost::asio::io_context object can be accessed using the
 * ThreadPool::get_io_context() function. This can be used to initialize Boost.Asio
 * objects like boost::asio::ip::tcp::socket and boost::asio::deadline_timer.
 *
 * The IOContextThreadPool subclass of ThreadPool can be used if an external
 * thread pool is used by an application. This is particularly useful for
 * environments where single threaded operation is required, such as a simulation
 * environment.
 *
 */
class ROBOTRACONTEUR_CORE_API ThreadPool : public RR_ENABLE_SHARED_FROM_THIS<ThreadPool>, private boost::noncopyable
{

  protected:
    std::vector<RR_SHARED_PTR<boost::thread> > threads;

    boost::mutex queue_mutex;

    RR_BOOST_ASIO_IO_CONTEXT _io_context;

    size_t thread_count;

    bool keepgoing;
    boost::mutex keepgoing_lock;

#if BOOST_ASIO_VERSION < 101200
    RR_SHARED_PTR<RR_BOOST_ASIO_IO_CONTEXT::work> _work;
#else
    RR_SHARED_PTR<boost::asio::executor_work_guard<RR_BOOST_ASIO_IO_CONTEXT::executor_type> > _work;
#endif

    RR_WEAK_PTR<RobotRaconteurNode> node;

  public:
    /**
     * @brief Construct a new ThreadPool
     *
     * Must use boost::make_shared<ThreadPool>()
     *
     * Use of ThreadPoolFactory is recommended.
     *
     * Set the thread pool using RobotRaconteurNode::SetThreadPool()
     *
     * @param node The node that owns the thread pool
     */
    ThreadPool(RR_SHARED_PTR<RobotRaconteurNode> node);
    virtual ~ThreadPool();

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

    /**
     * @brief Get the number of threads in the thread pool
     *
     * @return size_t The number of threads
     */
    virtual size_t GetThreadPoolCount();

    /**
     * @brief Set the desired number of threads in the thread pool
     *
     * Setting less than two threads may have unpredictable results
     *
     * @param count The desired number of threads
     */
    virtual void SetThreadPoolCount(size_t count);

    /**
     * @brief Post a function to be executed by the thread pool in a worker thread and return immediately
     *
     * @param function The function to execute
     */
    virtual void Post(boost::function<void()> function);

    /**
     * @brief Try posting a function to be executed by the thread pool in a worker thread and return immediately
     *
     * @param function The function to execute
     * @return true The function was posted to the thread pool
     * @return false An error ocurred and the function was not posted
     */
    virtual bool TryPost(RR_MOVE_ARG(boost::function<void()>) function);

    virtual void Shutdown();

    /**
     * @brief Get the boost::asio::io_context object
     *
     * Use the returned boost::asio::io_context reference to initialize Boost.Asio
     * objects
     *
     * @return boost::asio::io_context&
     */
    virtual RR_BOOST_ASIO_IO_CONTEXT& get_io_context();

  protected:
    virtual void start_new_thread();

    virtual void thread_function();
};

/**
 * @brief ThreadPool factory for use with RobotRaconteurNode
 *
 * Use ThreadPoolFactory with RobotRaconteurNode::SetThreadPoolFactory()
 * to configure the thread pool. Must be done before RobotRaconteurNode::Init()
 * is called.
 *
 */
class ROBOTRACONTEUR_CORE_API ThreadPoolFactory : private boost::noncopyable
{
  public:
    /**
     * @brief Construct and return a new threadpool
     *
     * @param node The node owning the thread pool
     * @return RR_SHARED_PTR<ThreadPool>
     */
    virtual RR_SHARED_PTR<ThreadPool> NewThreadPool(RR_SHARED_PTR<RobotRaconteurNode> node)
    {
        return RR_MAKE_SHARED<ThreadPool>(node);
    }
    virtual ~ThreadPoolFactory();
};

/**
 * @brief Thread pool for use with an external boost::asio::io_context and thread pool
 *
 * Users may need to integrate Robot Raconteur with an external thread pool, single
 * threaded environment, or an existing boost::asio::io_context. The IOContextThreadPool
 * allows the use of the external thread pool. See \ref threading for more information
 * on the use of threading.
 *
 * The IOContextThreadPool is used either with a thread pool or a single thread. If used
 * with a single thread, the `multithreaded` argument in the constructor
 * must be false. Blocking functions may not be used when in single threaded operation.
 *
 */
class ROBOTRACONTEUR_CORE_API IOContextThreadPool : public ThreadPool
{

  protected:
    RR_BOOST_ASIO_IO_CONTEXT& _external_io_context;
    bool _multithreaded;

  public:
    /**
     * @brief Construct an IOContextThreadPool
     *
     * Must use boost::make_shared<IOContextThreadpool>()
     *
     * @param node The node that owns the thread pool
     * @param external_io_context
     * @param multithreaded
     */
    IOContextThreadPool(RR_SHARED_PTR<RobotRaconteurNode> node, RR_BOOST_ASIO_IO_CONTEXT& external_io_context,
                        bool multithreaded);
    virtual ~IOContextThreadPool();

    /**
     * @brief Returns 1 if single threaded, 2 if multithreaded
     *
     * @return size_t
     */
    virtual size_t GetThreadPoolCount();

    /**
     * @brief Invalid for IOContextThreadPool, throws InvalidOperationException
     *
     * The IOContextThreadPool does not control the number of threads
     *
     * @param count
     */
    virtual void SetThreadPoolCount(size_t count);

    virtual void Post(boost::function<void()> function);
    virtual bool TryPost(RR_MOVE_ARG(boost::function<void()>) function);

    virtual void Shutdown();

    virtual RR_BOOST_ASIO_IO_CONTEXT& get_io_context();
};

namespace detail
{
bool ThreadPool_IsNodeMultithreaded(RR_WEAK_PTR<RobotRaconteurNode> node);
}

namespace detail
{
template <typename T>
struct IOContextThreadPool_AsyncResultAdapter_traits
{
    typedef T result_type;
};

template <>
struct IOContextThreadPool_AsyncResultAdapter_traits<void>
{
    typedef int32_t result_type;
};

template <typename T>
struct IOContextThreadPool_AsyncResultAdapter_data
{
    typedef typename IOContextThreadPool_AsyncResultAdapter_traits<T>::result_type result_type;
    boost::initialized<result_type> _result;
    RR_SHARED_PTR<RobotRaconteurException> _exp;
    boost::initialized<bool> _complete;
};

ROBOTRACONTEUR_CORE_API RR_SHARED_PTR<RobotRaconteurNode> IOContextThreadPool_RobotRaconteurNode_sp();

ROBOTRACONTEUR_CORE_API void IOContextThreadPool_RobotRaconteurNode_DownCastAndThrowException(
    RR_SHARED_PTR<RobotRaconteurNode> node, RobotRaconteurException& exp);
} // namespace detail

/**
 * @brief Adapter for asynchronous operations for use with IOContextThreadPool
 *
 * See \ref threading for more information.
 *
 * Asynchronous functions require a handler function be specified to be invoked when the operation
 * is complete. The IOContextThreadPool_AsyncResultAdapter (result adapter) is a utility class for
 * single threaded applications to simplify the design of calling asynchronous functions. The result
 * adapter is passed the current boost::asio::io_context, and the asynchronous function is invoked
 * with the result adapter passed as the handler. Next, GetResult() is called. This function
 * will block until the result is available, or an error occurs. GetResult() will repeatedly call
 * boost::asio::io_context::run_one() while waiting for the result to be ready. This will
 * run the thread pool, allowing events to be dispatched while waiting for the result. This
 * design removes the need for chained callbacks.
 *
 * **Must only be used in single-threaded applications**
 *
 * @tparam T The return type, or void if no return
 */
template <typename T>
class IOContextThreadPool_AsyncResultAdapter
{
  private:
    RR_SHARED_PTR<RobotRaconteurNode> _node;
    RR_BOOST_ASIO_IO_CONTEXT& _io_context;
    RR_SHARED_PTR<detail::IOContextThreadPool_AsyncResultAdapter_data<T> > _data;

  public:
    typedef typename detail::IOContextThreadPool_AsyncResultAdapter_traits<T>::result_type result_type;

    /**
     * @brief Construct an IOContextThreadPool_AsyncResultAdapter
     *
     * Should be constructed on the stack
     *
     * @param node The node that will use the adapter
     * @param io_context The single threaded IO Context
     */
    IOContextThreadPool_AsyncResultAdapter(RR_SHARED_PTR<RobotRaconteurNode>& node,
                                           RR_BOOST_ASIO_IO_CONTEXT& io_context)
        : _node(node), _io_context(io_context),
          _data(RR_MAKE_SHARED<detail::IOContextThreadPool_AsyncResultAdapter_data<T> >())
    {}
    /**
     * @brief Construct an IOContextThreadPool_AsyncResultAdapter for the singleton RobotRaconteurNode
     *
     * Should be constructed on the stack
     *
     * @param io_context The single threaded IO Context
     */
    IOContextThreadPool_AsyncResultAdapter(RR_BOOST_ASIO_IO_CONTEXT& io_context)
        : _node(detail::IOContextThreadPool_RobotRaconteurNode_sp()), _io_context(io_context),
          _data(RR_MAKE_SHARED<detail::IOContextThreadPool_AsyncResultAdapter_data<T> >())
    {}

    void operator()(result_type res, RR_SHARED_PTR<RobotRaconteurException> exp)
    {
        _data->_complete.data() = true;
        _data->_result.data() = res;
        _data->_exp = exp;
    }

    void operator()(RR_SHARED_PTR<RobotRaconteurException> exp)
    {
        _data->_complete.data() = true;
        _data->_exp = exp;
    }

    void operator()(result_type res)
    {
        _data->_complete.data() = true;
        _data->_result.data() = res;
    }

    void operator()() { _data->_complete.data() = true; }

    /**
     * @brief Get the result of the asynchronous operation
     *
     * Will throw an exception if the operation fails, or the operation
     * returns an exception.
     *
     * Will block until the result is available or an error occurs. boost::asio::io_context::run()
     * is called while waiting to continue dispatching events.
     *
     * @return result_type The result of the operation
     */
    result_type GetResult()
    {
        while (!_data->_complete.data())
        {
            _io_context.run_one();
        }

        result_type res;
        RR_SHARED_PTR<RobotRaconteurException> exp;
        boost::swap(res, _data->_result.data());
        exp = _data->_exp;
        _data->_exp.reset();
        if (exp)
        {
            RobotRaconteurException* exp1 = exp.get();
            detail::IOContextThreadPool_RobotRaconteurNode_DownCastAndThrowException(_node, *exp1);
        }
        return res;
    }

    /**
     * @brief Polls for a result, nonblocking
     *
     * Nonblocking poll operation to check if results are ready, and retrieve the result.
     * Does not call any functions in boost::asio::io_context. User must call
     * boost::asio::io_context::poll() to dispatch waiting events. PollResult() will
     * not throw an exception if an error occurs, instead it will fill the exp
     * reference parameter.
     *
     * @param ret [out] The result of the operation, valid if return is true
     * @param exp [out] The exception returned by operation, or NULL if no exception, valid if return is true
     * @return true The result is ready, ret and exp are valid
     * @return false The result is not ready, ret and exp are invalid
     */
    bool PollResult(result_type& ret, RR_SHARED_PTR<RobotRaconteurException>& exp)
    {
        if (!_data->_complete.data())
        {
            return false;
        }

        boost::swap(ret, _data->_result.data());
        boost::swap(exp, _data->_exp);
        return true;
    }
};

#define ROBOTRACONTEUR_ASSERT_MULTITHREADED(node)                                                                      \
    BOOST_ASSERT_MSG(detail::ThreadPool_IsNodeMultithreaded(node), "multithreading required for requested operation")

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
/** @brief Convenience alias for ThreadPool shared_ptr */
using ThreadPoolPtr = RR_SHARED_PTR<ThreadPool>;
/** @brief Convenience alias for ThreadPoolFactory shared_ptr */
using ThreadPoolFactoryPtr = RR_SHARED_PTR<ThreadPoolFactory>;
/** @brief Convenience alias for IOContextThreadPool shared_ptr */
using IOContextThreadPoolPtr = RR_SHARED_PTR<IOContextThreadPool>;
#endif
} // namespace RobotRaconteur
