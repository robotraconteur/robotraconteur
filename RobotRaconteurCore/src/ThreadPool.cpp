// Copyright 2011-2020 Wason Technology, LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "RobotRaconteur/ThreadPool.h"
#include "RobotRaconteur/RobotRaconteurNode.h"

#ifdef ROBOTRACONTEUR_APPLE
#include <TargetConditionals.h>
#endif

#include <boost/foreach.hpp>

namespace RobotRaconteur
{
// Thread pool

ThreadPool::ThreadPool(RR_SHARED_PTR<RobotRaconteurNode> node)
{
    keepgoing = true;
    this->node = node;

    thread_count = 0;
#if BOOST_ASIO_VERSION < 101200
    _work = RR_MAKE_SHARED<RR_BOOST_ASIO_IO_CONTEXT::work>(boost::ref(_io_context));
#else
    _work.reset(
        new boost::asio::executor_work_guard<RR_BOOST_ASIO_IO_CONTEXT::executor_type>(_io_context.get_executor()));
#endif
}

ThreadPool::~ThreadPool() {}

RR_SHARED_PTR<RobotRaconteurNode> ThreadPool::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

size_t ThreadPool::GetThreadPoolCount() { return thread_count; }

void ThreadPool::SetThreadPoolCount(size_t count)
{
    boost::mutex::scoped_lock lock(queue_mutex);

    if (count > thread_count)
    {
        for (size_t i = threads.size(); i < count; i++)
        {
            start_new_thread();
        }
    }
    thread_count = count;
}

void ThreadPool_post_wrapper(boost::function<void()> f, RR_WEAK_PTR<RobotRaconteurNode> node)
{
    try
    {
        if (f)
            f();
    }
    catch (std::exception& exp)
    {
        RobotRaconteurNode::TryHandleException(node, &exp);
    }
}

void ThreadPool::Post(boost::function<void()> function)
{
    if (!keepgoing)
        throw InvalidOperationException("Thread pool shutdown");

    RR_BOOST_ASIO_POST(_io_context, boost::bind(&ThreadPool_post_wrapper, function, GetNode()));
}

bool ThreadPool::TryPost(RR_MOVE_ARG(boost::function<void()>) function)
{
    if (!keepgoing)
        return false;

    RR_BOOST_ASIO_POST(_io_context, boost::bind(&ThreadPool_post_wrapper, function, GetNode()));

    return true;
}

void ThreadPool::start_new_thread()
{

    // boost::thread(boost::bind(&ThreadPool::thread_function,shared_from_this()));

    RR_SHARED_PTR<boost::thread> t =
        RR_MAKE_SHARED<boost::thread>(boost::bind(&ThreadPool::thread_function, shared_from_this()));
    // t->start_thread();
    threads.push_back(t);
}

void ThreadPool::thread_function()
{
    bool k;

    {
        boost::mutex::scoped_lock lock(keepgoing_lock);
        k = keepgoing;
    }

    while (k || !_io_context.stopped())
    {
        try
        {
            _io_context.run_one();
        }
        catch (std::exception& exp)
        {
            RobotRaconteurNode::TryHandleException(node, &exp);
        }

        {
            boost::mutex::scoped_lock lock(keepgoing_lock);
            k = keepgoing;
        }
    }
}

void ThreadPool::Shutdown()
{
    std::vector<RR_SHARED_PTR<boost::thread> > threads;
    {
        boost::mutex::scoped_lock lock(queue_mutex);
        {
            boost::mutex::scoped_lock lock(keepgoing_lock);
            keepgoing = false;
        }
        threads = this->threads;
        _work.reset();
    }

    _io_context.stop();

    {

        BOOST_FOREACH (RR_SHARED_PTR<boost::thread>& e, threads)
        {
#ifdef ROBOTRACONTEUR_IOS
            e->try_join_for(boost::chrono::seconds(1));
#else
            e->join();
#endif
        }

        boost::mutex::scoped_lock lock(queue_mutex);
        threads.clear();
    }
}

RR_BOOST_ASIO_IO_CONTEXT& ThreadPool::get_io_context() { return _io_context; }

ThreadPoolFactory::~ThreadPoolFactory() {}

IOContextThreadPool::IOContextThreadPool(RR_SHARED_PTR<RobotRaconteurNode> node,
                                         RR_BOOST_ASIO_IO_CONTEXT& external_io_context, bool multithreaded)
    : ThreadPool(node), _external_io_context(external_io_context), _multithreaded(multithreaded)
{}

IOContextThreadPool::~IOContextThreadPool() {}

size_t IOContextThreadPool::GetThreadPoolCount() { return _multithreaded ? 2 : 1; }

void IOContextThreadPool::SetThreadPoolCount(size_t count)
{
    throw InvalidOperationException("Cannot set thread count on IOContextThreadPool");
}

void IOContextThreadPool::Post(boost::function<void()> function)
{
    RR_BOOST_ASIO_POST(_external_io_context, boost::bind(&ThreadPool_post_wrapper, function, GetNode()));
}
bool IOContextThreadPool::TryPost(RR_MOVE_ARG(boost::function<void()>) function)
{
    RR_BOOST_ASIO_POST(_external_io_context, boost::bind(&ThreadPool_post_wrapper, function, GetNode()));
    return true;
}

void IOContextThreadPool::Shutdown() {}

RR_BOOST_ASIO_IO_CONTEXT& IOContextThreadPool::get_io_context() { return _external_io_context; }

namespace detail
{
bool ThreadPool_IsNodeMultithreaded(RR_WEAK_PTR<RobotRaconteurNode> node)
{
    RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
    if (!node1)
        throw InvalidOperationException("Node has been released");

    RR_SHARED_PTR<ThreadPool> p;
    if (!node1->TryGetThreadPool(p))
        throw InvalidOperationException("Node has been released");

    return p->GetThreadPoolCount() > 1;
}

RR_SHARED_PTR<RobotRaconteurNode> IOContextThreadPool_RobotRaconteurNode_sp() { return RobotRaconteurNode::sp(); }

void IOContextThreadPool_RobotRaconteurNode_DownCastAndThrowException(RR_SHARED_PTR<RobotRaconteurNode> node,
                                                                      RobotRaconteurException& exp)
{
    node->DownCastAndThrowException(exp);
}
} // namespace detail

} // namespace RobotRaconteur