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

using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.IO;
using System.Reflection;
using System.Linq;
using System.Threading.Tasks;

namespace RobotRaconteur
{
internal class AsyncRequestDirectorImpl : AsyncRequestDirector
{

    protected TaskCompletionSource<MessageElement> handler_task =
        new TaskCompletionSource<MessageElement>(TaskContinuationOptions.ExecuteSynchronously);

    public Task<MessageElement> Task
    {
        get {
            return handler_task.Task;
        }
    }

    public AsyncRequestDirectorImpl()
    {}

    public override void handler(MessageElement m, HandlerErrorInfo error)
    {
        // using (m)
        {
            try
            {
                this.Dispose();

                if (error.error_code != 0)
                {
                    using (m) using (MessageEntry merr = new MessageEntry())
                    {

                        this.handler_task.SetException(RobotRaconteurExceptionUtil.ErrorInfoToException(error));
                        return;
                    }
                }

                this.handler_task.SetResult(m);
            }
            catch (Exception e)
            {
                using (m) using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr, e.ToString());
                }
            }
        }
    }
}

internal class AsyncStubReturnDirectorImpl<T> : AsyncStubReturnDirector
{
    protected TaskCompletionSource<T> handler_task =
        new TaskCompletionSource<T>(TaskContinuationOptions.ExecuteSynchronously);

    public Task<T> Task
    {
        get {
            return handler_task.Task;
        }
    }

    protected ServiceFactory factory;

    public AsyncStubReturnDirectorImpl(ServiceFactory f = null)
    {

        this.factory = f;
    }

    public override void handler(WrappedServiceStub innerstub, HandlerErrorInfo error)
    {
        try
        {
            if (error.error_code != 0)
            {
                using (MessageEntry merr = new MessageEntry())
                {

                    this.handler_task.SetException(RobotRaconteurExceptionUtil.ErrorInfoToException(error));
                    return;
                }
            }

            object s = null;
            T s1 = default(T);
            try
            {
                int id = innerstub.GetObjectHeapID();
                if (id != 0)
                {
                    handler_task.SetResult((T)RRObjectHeap.GetObject(id));
                    return;
                }

                ServiceFactory f;
                if (factory == null)
                {
                    f = RobotRaconteurNode.s.GetServiceType(innerstub.RR_objecttype.GetServiceDefinition().Name);
                }
                else
                {
                    f = factory;
                }
                s = f.CreateStub(innerstub);
                s1 = (T)s;
            }
            catch (Exception e)
            {
                handler_task.SetException(e);
                return;
            }

            handler_task.SetResult(s1);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
    }
}

internal class AsyncVoidReturnDirectorImpl : AsyncVoidReturnDirector
{
    protected TaskCompletionSource<int> handler_task =
        new TaskCompletionSource<int>(TaskContinuationOptions.ExecuteSynchronously);
    public Task Task
    {
        get {
            return handler_task.Task;
        }
    }

    public AsyncVoidReturnDirectorImpl()
    {}

    public override void handler(HandlerErrorInfo error)
    {
        try
        {
            if (error.error_code != 0)
            {
                using (MessageEntry merr = new MessageEntry())
                {

                    handler_task.SetException(RobotRaconteurExceptionUtil.ErrorInfoToException(error));
                    return;
                }
            }

            handler_task.SetResult(0);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
    }
}

internal class AsyncVoidNoErrReturnDirectorImpl : AsyncVoidNoErrReturnDirector
{
    protected TaskCompletionSource<int> handler_task =
        new TaskCompletionSource<int>(TaskContinuationOptions.ExecuteSynchronously);

    public Task Task
    {
        get {
            return handler_task.Task;
        }
    }

    public AsyncVoidNoErrReturnDirectorImpl()
    {}

    public override void handler()
    {
        try
        {
            handler_task.SetResult(0);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
    }
}

internal class AsyncStringReturnDirectorImpl : AsyncStringReturnDirector
{

    protected TaskCompletionSource<string> handler_task =
        new TaskCompletionSource<string>(TaskContinuationOptions.ExecuteSynchronously);

    public Task<string> Task
    {
        get {
            return handler_task.Task;
        }
    }

    public AsyncStringReturnDirectorImpl()
    {}

    public override void handler(string s, HandlerErrorInfo error)
    {
        try
        {
            if (error.error_code != 0)
            {
                using (MessageEntry merr = new MessageEntry())
                {

                    handler_task.SetException(RobotRaconteurExceptionUtil.ErrorInfoToException(error));
                    return;
                }
            }

            handler_task.SetResult(s);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
    }
}

internal class AsyncUInt32ReturnDirectorImpl : AsyncUInt32ReturnDirector
{

    protected TaskCompletionSource<uint> handler_task =
        new TaskCompletionSource<uint>(TaskContinuationOptions.ExecuteSynchronously);

    public Task<uint> Task
    {
        get {
            return handler_task.Task;
        }
    }

    public AsyncUInt32ReturnDirectorImpl()
    {}

    public override void handler(uint v, HandlerErrorInfo error)
    {

        try
        {
            if (error.error_code != 0)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    this.handler_task.SetException(RobotRaconteurExceptionUtil.ErrorInfoToException(error));
                    return;
                }
            }

            this.handler_task.SetResult(v);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
    }
}

internal class AsyncExceptionDirectorImpl : AsyncVoidReturnDirector
{

    protected Action<Exception> handler_func;

    public AsyncExceptionDirectorImpl(Action<Exception> handler_func)
    {
        this.handler_func = handler_func;
    }

    public override void handler(HandlerErrorInfo error)
    {
        try
        {
            if (error.error_code != 0)
            {
                using (MessageEntry merr = new MessageEntry())
                {

                    this.handler_func(RobotRaconteurExceptionUtil.ErrorInfoToException(error));
                    return;
                }
            }

            handler_func(new Exception("Unknown exception"));
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
    }
}

internal class AsyncTimerEventReturnDirectorImpl : AsyncTimerEventReturnDirector
{
    protected Action<TimerEvent> handler_func;

    public AsyncTimerEventReturnDirectorImpl(Action<TimerEvent> handler_func)
    {
        this.handler_func = handler_func;
    }

    public override void handler(TimerEvent ev, HandlerErrorInfo error)
    {
        try
        {
            handler_func(ev);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
    }
}

}