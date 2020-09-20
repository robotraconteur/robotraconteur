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
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace RobotRaconteur
{
    public interface Generator1<ReturnType, ParamType>
    {
        ReturnType Next(ParamType param);
        Task<ReturnType> AsyncNext(ParamType param, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Abort();
        Task AsyncAbort(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Close();
        Task AsyncClose(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
    }

    public interface Generator2<ReturnType>
    {
        ReturnType Next();
        Task<ReturnType> AsyncNext(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Abort();
        Task AsyncAbort(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Close();
        Task AsyncClose(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        ReturnType[] NextAll();
    }

    public interface Generator3<ParamType>
    {
        void Next(ParamType param);
        Task AsyncNext(ParamType param, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Abort();
        Task AsyncAbort(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Close();
        Task AsyncClose(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
    }

    public abstract class SyncGenerator1<ReturnType, ParamType> : Generator1<ReturnType, ParamType>
    {
        public abstract void Abort();
        public abstract void Close();
        public abstract ReturnType Next(ParamType param);

        public Task AsyncAbort(int timeout = -1)
        {            
            Abort();
            return Task.FromResult(0);
        }

        public Task AsyncClose(int timeout = -1)
        {
            Close();
            return Task.FromResult(0);
        }

        public Task<ReturnType> AsyncNext(ParamType param, int timeout = -1)
        {            
            ReturnType r = Next(param);
            return Task.FromResult(r);                     
        }
    }

    public abstract class SyncGenerator2<ReturnType> : Generator2<ReturnType>
    {
        public abstract void Abort();
        public abstract void Close();
        public abstract ReturnType Next();

        public Task AsyncAbort(int timeout = -1)
        {
            Abort();
            return Task.FromResult(0);
        }

        public Task AsyncClose(int timeout = -1)
        {           
            Close();
            return Task.FromResult(0);
        }

        public Task<ReturnType> AsyncNext(int timeout = -1)
        {
            ReturnType r = Next();
            return Task.FromResult(r);
        }

        public ReturnType[] NextAll()
        {
            List<ReturnType> o = new List<ReturnType>();
            try
            {
                while (true)
                {
                    o.Add(Next());
                }
            }
            catch (StopIterationException) { }
            return o.ToArray();
        }
    }

    public abstract class SyncGenerator3<ParamType> : Generator3<ParamType>
    {
        public abstract void Abort();
        public abstract void Close();
        public abstract void Next(ParamType param);

        public Task AsyncAbort(int timeout = -1)
        {
            Abort();
            return Task.FromResult(0);
        }

        public Task AsyncClose(int timeout = -1)
        {
            Close();
            return Task.FromResult(0);
        }

        public Task AsyncNext(ParamType param, int timeout = -1)
        {
            Next(param);
            return Task.FromResult(0);
        }
    }

    public class EnumeratorGenerator<T> : SyncGenerator2<T>
    {
        bool aborted = false;
        bool closed = false;
        IEnumerator<T> enumerator;

        public EnumeratorGenerator(IEnumerable<T> enumerable)
            : this(enumerable.GetEnumerator())
        { }

        public EnumeratorGenerator(IEnumerator<T> enumerator)
        {
            this.enumerator = enumerator;
        }

        public override void Abort()
        {
            lock (this)
            {
                aborted = true;
            }

        }

        public override void Close()
        {
            lock (this)
            {
                closed = true;
            }

        }

        public override T Next()
        {
            lock (this)
            {
                if (aborted) throw new OperationAbortedException("Generator aborted");
                if (closed) throw new StopIterationException("");
                if (!enumerator.MoveNext()) throw new StopIterationException("");
                return enumerator.Current;
            }
        }
    }

    public class Generator1Client<ReturnType, ParamType> : Generator1<ReturnType, ParamType>
    {
        protected WrappedGeneratorClient inner_gen;

        public Generator1Client(WrappedGeneratorClient inner_gen)
        {
            this.inner_gen = inner_gen;
        }

        public ReturnType Next(ParamType param)
        {
            using(MessageElement m = RobotRaconteurNode.s.PackAnyType<ParamType>("parameter", ref param))
            {
                
                using (MessageElement m2 = inner_gen.Next(m))
                {
                    return RobotRaconteurNode.s.UnpackAnyType<ReturnType>(m2);
                }
                
            }            
        }
        public async Task<ReturnType> AsyncNext(ParamType param, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            using(MessageElement m= RobotRaconteurNode.s.PackAnyType<ParamType>("parameter", ref param))
            {
                
                    AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl();
                    int id = RRObjectHeap.AddObject(d);
                    inner_gen.AsyncNext(m, timeout, d, id);
                    var mret = await d.Task;
                    return RobotRaconteurNode.s.UnpackAnyType<ReturnType>(mret);                
            }
            
        }

                public void Abort()
        {
            inner_gen.Abort();
        }
        public async Task AsyncAbort(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl();
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncAbort(timeout, h, id);
            await h.Task;
        }

        public void Close()
        {
            inner_gen.Close();
        }
        public async Task AsyncClose(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl();
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncClose(timeout, h, id);
            await h.Task;
        }
    }

    public class Generator2Client<ReturnType> : Generator2<ReturnType>
    {
        protected WrappedGeneratorClient inner_gen;

        public Generator2Client(WrappedGeneratorClient inner_gen)
        {
            this.inner_gen = inner_gen;
        }

        public ReturnType Next()
        {
            using (MessageElement m2 = inner_gen.Next(null))
            {
                return RobotRaconteurNode.s.UnpackAnyType<ReturnType>(m2);
            }
        }
        public async Task<ReturnType> AsyncNext(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl();
            int id = RRObjectHeap.AddObject(d);
            inner_gen.AsyncNext(null, timeout, d, id);
            var mret = await d.Task;
            return RobotRaconteurNode.s.UnpackAnyType<ReturnType>(mret);
        }

        
        public void Abort()
        {
            inner_gen.Abort();
        }
        public async Task AsyncAbort(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl();
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncAbort(timeout, h, id);
            await h.Task;
        }

        public void Close()
        {
            inner_gen.Close();
        }
        public async Task AsyncClose(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl();
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncClose(timeout, h, id);
            await h.Task;
        }

        public ReturnType[] NextAll()
        {
            List<ReturnType> o = new List<ReturnType>();
            try
            {
                while (true)
                {
                    o.Add(Next());
                }
            }
            catch (StopIterationException) { }
            return o.ToArray();
        }
    }

    public class Generator3Client<ParamType> : Generator3<ParamType>
    {
        protected WrappedGeneratorClient inner_gen;

        public Generator3Client(WrappedGeneratorClient inner_gen)
        {
            this.inner_gen = inner_gen;
        }

        public void Next(ParamType param)
        {
            using(MessageElement m = RobotRaconteurNode.s.PackAnyType<ParamType>("parameter", ref param))
            {            
                inner_gen.Next(m);
            }
                
        }
        public async Task AsyncNext(ParamType param, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            using(MessageElement m = RobotRaconteurNode.s.PackAnyType<ParamType>("parameter", ref param))            
            {
                
                AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl();
                int id = RRObjectHeap.AddObject(d);
                inner_gen.AsyncNext(m, timeout, d, id);
                var mret = await d.Task;      
                
            }
            
        }

        private static void EndAsyncNext(MessageElement m, Exception err, object p)
        {
            Action<Exception> h = (Action<Exception>)p;
            h(err);
        }

        public void Abort()
        {
            inner_gen.Abort();
        }
        public async Task AsyncAbort(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl();
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncAbort(timeout, h, id);
            await h.Task;
        }

        public void Close()
        {
            inner_gen.Close();
        }
        public async Task AsyncClose(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl();
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncClose(timeout, h, id);
            await h.Task;
        }
    }

    public class WrappedGenerator1ServerDirectorNET<ReturnType, ParamType> : WrappedGeneratorServerDirector
    {
        Generator1<ReturnType, ParamType> generator;
        public WrappedGenerator1ServerDirectorNET(Generator1<ReturnType, ParamType> generator)
        {
            if (generator == null) throw new NullReferenceException("Generator must not be null");
            this.generator = generator;
            this.objectheapid = RRObjectHeap.AddObject(this);
        }

        public override MessageElement Next(MessageElement m, WrappedServiceSkelAsyncAdapter async_adapter)
        {
            using (m)
            {
                try
                {
                    async_adapter.MakeAsync();
                    ParamType p = RobotRaconteurNode.s.UnpackAnyType<ParamType>(m);
                    generator.AsyncNext(p).ContinueWith(t => async_adapter.EndTask<ReturnType>(t, async_ret => RobotRaconteurNode.s.PackAnyType<ReturnType>("return", ref async_ret)));
                    return null;              
                }
                catch (Exception e)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {
                        RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                        RRDirectorExceptionHelper.SetError(merr,e.ToString());
                        return null;
                    }
                }
            }
        }

        public override void Abort(WrappedServiceSkelAsyncAdapter async_adapter)
        {
            try
            {
                async_adapter.MakeAsync();
                generator.AsyncAbort().ContinueWith(t => async_adapter.EndTask(t));
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr,e.ToString());
                    return;
                }
            }
        }

        public override void Close(WrappedServiceSkelAsyncAdapter async_adapter)
        {
            try
            {
                async_adapter.MakeAsync();
                generator.AsyncClose().ContinueWith(t => async_adapter.EndTask(t));
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr,e.ToString());
                    return;
                }
            }
        }
    }

    public class WrappedGenerator2ServerDirectorNET<ReturnType> : WrappedGeneratorServerDirector
    {
        Generator2<ReturnType> generator;
        public WrappedGenerator2ServerDirectorNET(Generator2<ReturnType> generator)
        {
            if (generator == null) throw new NullReferenceException("Generator must not be null");
            this.generator = generator;
            this.objectheapid = RRObjectHeap.AddObject(this);
        }

        public override MessageElement Next(MessageElement m, WrappedServiceSkelAsyncAdapter async_adapter)
        {
            using (m)
            {
                try
                {
                    async_adapter.MakeAsync();                    
                    generator.AsyncNext().ContinueWith(t => async_adapter.EndTask<ReturnType>(t, async_ret => RobotRaconteurNode.s.PackAnyType<ReturnType>("return", ref async_ret)));
                    return null;
                }
                catch (Exception e)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {
                        RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                        RRDirectorExceptionHelper.SetError(merr,e.ToString());
                        return null;
                    }
                }
            }
        }

        public override void Abort(WrappedServiceSkelAsyncAdapter async_adapter)
        {
            try
            {
                async_adapter.MakeAsync();
                generator.AsyncAbort().ContinueWith(t => async_adapter.EndTask(t));
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr,e.ToString());
                    return;
                }
            }
        }

        public override void Close(WrappedServiceSkelAsyncAdapter async_adapter)
        {
            try
            {
                async_adapter.MakeAsync();
                generator.AsyncClose().ContinueWith(t => async_adapter.EndTask(t));
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr,e.ToString());
                    return;
                }
            }
        }
    }

    public class WrappedGenerator3ServerDirectorNET<ParamType> : WrappedGeneratorServerDirector
    {
        Generator3<ParamType> generator;
        public WrappedGenerator3ServerDirectorNET(Generator3<ParamType> generator)
        {
            if (generator == null) throw new NullReferenceException("Generator must not be null");
            this.generator = generator;
            this.objectheapid = RRObjectHeap.AddObject(this);
        }

        public override MessageElement Next(MessageElement m,WrappedServiceSkelAsyncAdapter async_adapter)
        {
            using (m)
            {
                try
                {
                    async_adapter.MakeAsync();                
                    ParamType p = RobotRaconteurNode.s.UnpackAnyType<ParamType>(m);
                    generator.AsyncNext(p).ContinueWith(t => async_adapter.EndTask(t));
                    return null;
                }
                catch (Exception e)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {
                        RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                        RRDirectorExceptionHelper.SetError(merr,e.ToString());
                        return null;
                    }
                }
            }
        }

        public override void Abort(WrappedServiceSkelAsyncAdapter async_adapter)
        {
            try
            {
                async_adapter.MakeAsync();
                generator.AsyncAbort().ContinueWith(t => async_adapter.EndTask(t));
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr,e.ToString());
                    return;
                }
            }
        }

        public override void Close(WrappedServiceSkelAsyncAdapter async_adapter)
        {
            try
            {
                async_adapter.MakeAsync();
                generator.AsyncClose().ContinueWith(t => async_adapter.EndTask(t));
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr,e.ToString());
                    return;
                }
            }
        }
    }

    internal class AsyncGeneratorClientReturnDirectorImpl : AsyncGeneratorClientReturnDirector
    {        
        protected TaskCompletionSource<WrappedGeneratorClient> handler_task = new TaskCompletionSource<WrappedGeneratorClient>(TaskContinuationOptions.ExecuteSynchronously);

        public Task<WrappedGeneratorClient> Task { get { return handler_task.Task; } }

        public AsyncGeneratorClientReturnDirectorImpl()
        {
            
        }

        public override void handler(WrappedGeneratorClient m, HandlerErrorInfo error)
        {
            //using (m)
            {
                try
                {
                    this.Dispose();

                    if (error.error_code != 0)
                    {
                        using (MessageEntry merr = new MessageEntry())
                        {

                            handler_task.SetException(RobotRaconteurExceptionUtil.ErrorInfoToException(error));
                            return;
                        }
                    }

                    handler_task.SetResult(m);

                }
                catch (Exception e)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {
                        RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                        RRDirectorExceptionHelper.SetError(merr,e.ToString());
                    }
                }
            }

        }
    }
}
