// Copyright 2011-2018 Wason Technology, LLC
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

namespace RobotRaconteur
{
    public interface Generator1<ReturnType, ParamType>
    {
        ReturnType Next(ParamType param);
        void AsyncNext(ParamType param, Action<ReturnType, Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Abort();
        void AsyncAbort(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Close();
        void AsyncClose(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
    }

    public interface Generator2<ReturnType>
    {
        ReturnType Next();
        void AsyncNext(Action<ReturnType, Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Abort();
        void AsyncAbort(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Close();
        void AsyncClose(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        ReturnType[] NextAll();
    }

    public interface Generator3<ParamType>
    {
        void Next(ParamType param);
        void AsyncNext(ParamType param, Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Abort();
        void AsyncAbort(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Close();
        void AsyncClose(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
    }

    public abstract class SyncGenerator1<ReturnType, ParamType> : Generator1<ReturnType, ParamType>
    {
        public abstract void Abort();
        public abstract void Close();
        public abstract ReturnType Next(ParamType param);

        public void AsyncAbort(Action<Exception> handler, int timeout = -1)
        {
            try
            {
                Abort();
                handler(null);
            }
            catch (Exception e)
            {
                handler(e);
                return;
            }
            return;
        }

        public void AsyncClose(Action<Exception> handler, int timeout = -1)
        {
            try
            {
                Close();
                handler(null);
            }
            catch (Exception e)
            {
                handler(e);
                return;
            }
            return;
        }

        public void AsyncNext(ParamType param, Action<ReturnType, Exception> handler, int timeout = -1)
        {
            try
            {
                ReturnType r = Next(param);
                handler(r, null);
            }
            catch (Exception e)
            {
                handler(default(ReturnType), e);
                return;
            }
            return;
        }
    }

    public abstract class SyncGenerator2<ReturnType> : Generator2<ReturnType>
    {
        public abstract void Abort();
        public abstract void Close();
        public abstract ReturnType Next();

        public void AsyncAbort(Action<Exception> handler, int timeout = -1)
        {
            try
            {
                Abort();
                handler(null);
            }
            catch (Exception e)
            {
                handler(e);
                return;
            }
            return;
        }

        public void AsyncClose(Action<Exception> handler, int timeout = -1)
        {
            try
            {
                Close();
                handler(null);
            }
            catch (Exception e)
            {
                handler(e);
                return;
            }
            return;
        }

        public void AsyncNext(Action<ReturnType, Exception> handler, int timeout = -1)
        {
            try
            {
                ReturnType r = Next();
                handler(r, null);
            }
            catch (Exception e)
            {
                handler(default(ReturnType), e);
                return;
            }
            return;
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

        public void AsyncAbort(Action<Exception> handler, int timeout = -1)
        {
            try
            {
                Abort();
                handler(null);
            }
            catch (Exception e)
            {
                handler(e);
                return;
            }
            return;
        }

        public void AsyncClose(Action<Exception> handler, int timeout = -1)
        {
            try
            {
                Close();
                handler(null);
            }
            catch (Exception e)
            {
                handler(e);
                return;
            }
            return;
        }

        public void AsyncNext(ParamType param, Action<Exception> handler, int timeout = -1)
        {
            try
            {
                Next(param);
                handler(null);
            }
            catch (Exception e)
            {
                handler(e);
                return;
            }
            return;
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
            object param1 = RobotRaconteurNode.s.PackVarType(param);
            try
            {
                using (MessageElement m = new MessageElement("parameter", param1))
                {
                    using (MessageElement m2 = inner_gen.Next(m))
                    {
                        return Wire<ReturnType>.UnpackData(m2);
                    }
                }
            }
            finally
            {
                IDisposable d = param1 as IDisposable;
                if (d != null) d.Dispose();
            }
        }
        public void AsyncNext(ParamType param, Action<ReturnType, Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            object param1 = RobotRaconteurNode.s.PackVarType(param);
            try
            {
                using (MessageElement m = new MessageElement("parameter", param1))
                {
                    AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl(EndAsyncNext, handler);
                    int id = RRObjectHeap.AddObject(d);
                    inner_gen.AsyncNext(m, timeout, d, id);
                }
            }
            finally
            {
                IDisposable d = param1 as IDisposable;
                if (d != null) d.Dispose();
            }
        }

        private static void EndAsyncNext(MessageElement m, Exception err, object p)
        {
            Action<ReturnType, Exception> h = (Action<ReturnType, Exception>)p;
            if (err != null)
            {
                h(default(ReturnType), err);
                return;
            }

            h(Wire<ReturnType>.UnpackData(m), null);
        }

        public void Abort()
        {
            inner_gen.Abort();
        }
        public void AsyncAbort(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate (Exception e1, object o1) { handler(e1); }, null);
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncAbort(timeout, h, id);
        }

        public void Close()
        {
            inner_gen.Close();
        }
        public void AsyncClose(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate (Exception e1, object o1) { handler(e1); }, null);
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncClose(timeout, h, id);
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
                return Wire<ReturnType>.UnpackData(m2);
            }
        }
        public void AsyncNext(Action<ReturnType, Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl(EndAsyncNext, handler);
            int id = RRObjectHeap.AddObject(d);
            inner_gen.AsyncNext(null, timeout, d, id);
        }

        private static void EndAsyncNext(MessageElement m, Exception err, object p)
        {
            Action<ReturnType, Exception> h = (Action<ReturnType, Exception>)p;
            if (err != null)
            {
                h(default(ReturnType), err);
                return;
            }

            h(Wire<ReturnType>.UnpackData(m), null);
        }

        public void Abort()
        {
            inner_gen.Abort();
        }
        public void AsyncAbort(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate (Exception e1, object o1) { handler(e1); }, null);
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncAbort(timeout, h, id);
        }

        public void Close()
        {
            inner_gen.Close();
        }
        public void AsyncClose(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate (Exception e1, object o1) { handler(e1); }, null);
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncClose(timeout, h, id);
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
            object param1 = RobotRaconteurNode.s.PackVarType(param);
            try
            {
                using (MessageElement m = new MessageElement("parameter", param1))
                {
                    inner_gen.Next(m);
                }
            }
            finally
            {
                IDisposable d = param1 as IDisposable;
                if (d != null) d.Dispose();
            }
        }
        public void AsyncNext(ParamType param, Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            object param1 = RobotRaconteurNode.s.PackVarType(param);
            try
            {
                using (MessageElement m = new MessageElement("parameter", param1))
                {
                    AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl(EndAsyncNext, handler);
                    int id = RRObjectHeap.AddObject(d);
                    inner_gen.AsyncNext(m, timeout, d, id);
                }
            }
            finally
            {
                IDisposable d = param1 as IDisposable;
                if (d != null) d.Dispose();
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
        public void AsyncAbort(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate (Exception e1, object o1) { handler(e1); }, null);
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncAbort(timeout, h, id);
        }

        public void Close()
        {
            inner_gen.Close();
        }
        public void AsyncClose(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate (Exception e1, object o1) { handler(e1); }, null);
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncClose(timeout, h, id);
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

        public override MessageElement Next(MessageElement m)
        {
            using (m)
            {
                try
                {
                    ParamType p = Wire<ParamType>.UnpackData(m);
                    ReturnType r = generator.Next(p);
                    object r1 = RobotRaconteurNode.s.PackVarType(r);
                    try
                    {
                        MessageElement m_r = new MessageElement("return", r1);
                        return m_r;
                    }
                    finally
                    {
                        IDisposable d = r1 as IDisposable;
                        if (d != null) d.Dispose();
                    }
                }
                catch (Exception e)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {
                        RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                        RRDirectorExceptionHelper.SetError(merr);
                        return null;
                    }
                }
            }
        }

        public override void Abort()
        {
            try
            {
                generator.Abort();
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return;
                }
            }
        }

        public override void Close()
        {
            try
            {
                generator.Close();
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
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

        public override MessageElement Next(MessageElement m)
        {
            using (m)
            {
                try
                {
                    ReturnType r = generator.Next();
                    object r1 = RobotRaconteurNode.s.PackVarType(r);
                    try
                    {
                        MessageElement m_r = new MessageElement("return", r1);
                        {
                            return m_r;
                        }
                    }
                    finally
                    {
                        IDisposable d = r1 as IDisposable;
                        if (d != null) d.Dispose();
                    }
                }
                catch (Exception e)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {
                        RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                        RRDirectorExceptionHelper.SetError(merr);
                        return null;
                    }
                }
            }
        }

        public override void Abort()
        {
            try
            {
                generator.Abort();
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return;
                }
            }
        }

        public override void Close()
        {
            try
            {
                generator.Close();
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
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

        public override MessageElement Next(MessageElement m)
        {
            using (m)
            {
                try
                {
                    ParamType p = Wire<ParamType>.UnpackData(m);
                    generator.Next(p);
                    return null;
                }
                catch (Exception e)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {
                        RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                        RRDirectorExceptionHelper.SetError(merr);
                        return null;
                    }
                }
            }
        }

        public override void Abort()
        {
            try
            {
                generator.Abort();
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return;
                }
            }
        }

        public override void Close()
        {
            try
            {
                generator.Close();
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return;
                }
            }
        }
    }

    internal class AsyncGeneratorClientReturnDirectorImpl : AsyncGeneratorClientReturnDirector
    {

        protected Action<WrappedGeneratorClient, Exception, object> handler_func;
        protected object param;

        public AsyncGeneratorClientReturnDirectorImpl(Action<WrappedGeneratorClient, Exception, object> handler_func, object param)
        {
            this.handler_func = handler_func;
            this.param = param;
        }

        public override void handler(WrappedGeneratorClient m, uint error_code, string errorname, string errormessage)
        {
            using (m)
            {
                try
                {
                    this.Dispose();

                    if (error_code != 0)
                    {
                        using (MessageEntry merr = new MessageEntry())
                        {

                            this.handler_func(null, RobotRaconteurExceptionUtil.ErrorCodeToException((RobotRaconteur.MessageErrorType)error_code, errorname, errormessage), param);
                            return;
                        }
                    }

                    handler_func(m, null, param);

                }
                catch (Exception e)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {
                        RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                        RRDirectorExceptionHelper.SetError(merr);
                    }
                }
            }

        }
    }
}
