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

#include "RobotRaconteur/AsyncUtils.h"
#include "RobotRaconteur/RobotRaconteurNode.h"

#include <boost/scope_exit.hpp>

namespace RobotRaconteur
{
namespace detail
{
RR_SHARED_PTR<Timer> async_timeout_wrapper_CreateTimer(RR_SHARED_PTR<RobotRaconteurNode> node,
                                                       const boost::posix_time::time_duration& period,
                                                       RR_MOVE_ARG(boost::function<void(const TimerEvent&)>) handler,
                                                       bool oneshot)
{
    return node->CreateTimer(period, handler, oneshot);
}

void async_signal_pool_semaphore::do_post(RR_SHARED_PTR<RobotRaconteurNode> node1,
                                          RR_MOVE_ARG(boost::function<void()>) h)
{
    RobotRaconteurNode::TryPostToThreadPool(
        node1, boost::bind(&async_signal_pool_semaphore::do_fire_next, shared_from_this(), h));
}

void async_signal_pool_semaphore::handle_exception(std::exception* exp)
{
    RobotRaconteurNode::TryHandleException(node, exp);
}

ROBOTRACONTEUR_CORE_API void InvokeHandler_HandleException(RR_WEAK_PTR<RobotRaconteurNode> node, std::exception& exp)
{
    RobotRaconteurNode::TryHandleException(node, &exp);
}

ROBOTRACONTEUR_CORE_API void InvokeHandler_DoPost(RR_WEAK_PTR<RobotRaconteurNode> node, boost::function<void()>& h,
                                                  bool shutdown_op, bool throw_on_released)
{
    if (!RobotRaconteurNode::TryPostToThreadPool(node, h, shutdown_op))
    {
        if (!throw_on_released)
            return;
        throw InvalidOperationException("Node has been released");
    }
}

ROBOTRACONTEUR_CORE_API void InvokeHandler(RR_WEAK_PTR<RobotRaconteurNode> node, boost::function<void()>& handler)
{
    try
    {
        handler();
    }
    catch (std::exception& exp)
    {
        RobotRaconteurNode::TryHandleException(node, &exp);
    }
}

ROBOTRACONTEUR_CORE_API void InvokeHandler(RR_WEAK_PTR<RobotRaconteurNode> node,
                                           boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& handler)
{
    try
    {
        handler(RR_SHARED_PTR<RobotRaconteurException>());
    }
    catch (std::exception& exp)
    {
        InvokeHandler_HandleException(node, exp);
    }
}

ROBOTRACONTEUR_CORE_API void InvokeHandlerWithException(
    RR_WEAK_PTR<RobotRaconteurNode> node, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& handler,
    RR_SHARED_PTR<RobotRaconteurException> exp)
{
    try
    {
        handler(exp);
    }
    catch (std::exception& exp2)
    {
        InvokeHandler_HandleException(node, exp2);
    }
}

ROBOTRACONTEUR_CORE_API void InvokeHandlerWithException(
    RR_WEAK_PTR<RobotRaconteurNode> node, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& handler,
    std::exception& exp, MessageErrorType default_err)
{
    try
    {
        RR_SHARED_PTR<RobotRaconteurException> err =
            RobotRaconteurExceptionUtil::ExceptionToSharedPtr(exp, default_err);
        handler(err);
    }
    catch (std::exception& exp2)
    {
        InvokeHandler_HandleException(node, exp2);
    }
}

ROBOTRACONTEUR_CORE_API void PostHandler(RR_WEAK_PTR<RobotRaconteurNode> node, boost::function<void()>& handler,
                                         bool shutdown_op, bool throw_on_released)
{
    if (!RobotRaconteurNode::TryPostToThreadPool(node, handler, shutdown_op))
    {
        if (!throw_on_released)
            return;
        throw InvalidOperationException("Node has been released");
    }
}

ROBOTRACONTEUR_CORE_API void PostHandler(RR_WEAK_PTR<RobotRaconteurNode> node,
                                         boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& handler,
                                         bool shutdown_op, bool throw_on_released)
{

    if (!RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, RR_SHARED_PTR<RobotRaconteurException>()),
                                                 shutdown_op))
    {
        if (!throw_on_released)
            return;
        throw InvalidOperationException("Node has been released");
    }
}

ROBOTRACONTEUR_CORE_API void PostHandlerWithException(
    RR_WEAK_PTR<RobotRaconteurNode> node, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& handler,
    RR_SHARED_PTR<RobotRaconteurException> exp, bool shutdown_op, bool throw_on_released)
{
    if (!RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, exp), shutdown_op))
    {
        if (!throw_on_released)
            return;
        throw InvalidOperationException("Node has been released");
    }
}

ROBOTRACONTEUR_CORE_API void PostHandlerWithException(
    RR_WEAK_PTR<RobotRaconteurNode> node, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& handler,
    std::exception& exp, MessageErrorType default_err, bool shutdown_op, bool throw_on_released)
{
    RR_SHARED_PTR<RobotRaconteurException> err = RobotRaconteurExceptionUtil::ExceptionToSharedPtr(exp, default_err);

    if (!RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, err), shutdown_op))
    {
        if (!throw_on_released)
            return;
        throw InvalidOperationException("Node has been released");
    }
}

} // namespace detail
} // namespace RobotRaconteur