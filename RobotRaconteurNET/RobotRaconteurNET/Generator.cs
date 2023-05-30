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
/// <summary>
/// Generator type for use with generator functions, with parameter and return
/// </summary>
/// <remarks>
/// <para>
/// Generators are used with generator functions to implement simple coroutines. They are
/// returned by function members with a parameter and/or return marked with the
/// generator container type. Robot Raconteur generators are modeled on Python generators,
/// and are intended to be used in two scenarios:
/// 1. Transfering large parameter values or return values that would be over the message
/// transfer limit (typically around 10 MB).
/// 2. Long running operations that return updates or require periodic input. Generators
/// are used to implement functionality similar to "actions" in ROS.
/// </para>
/// <para>
/// Generators are a generalization of iterators, where a value is returned every time
/// the iterator is advanced until there are no more values. Python and Robot Raconteur iterators
/// add the option of passing a parameter every advance, allowing for simple coroutines. The
/// generator is advanced by calling the Next() or AsyncNext() functions. These functions
/// will either return a value or throw StopIterationException if there are no more values. Next()
/// and AsyncNext() may also throw any valid Robot Raconteur exception.
/// </para>
/// <para>
/// Generators can be terminated with either the Close() or Abort() functions. Close() should be
/// used to cleanly close the generator, and is not considered an error condition. Next(), if called
/// after close, should throw StopIterationException. Abort() is considered an error condition, and
/// will cause any action assosciated with the generator to be aborted as quickly as possible (ie faulting
/// a robot). If Next() is called after Abort(), OperationAbortedException should be thrown.
/// </para>
/// <para>
/// Robot Raconteur clients will return a populated stub generator that calls the service. Services
/// are expected to return a subclass of Generator. The service will call AsyncNext(), AsyncAbort(), and AsyncClose().
/// Inherit from SyncGenerator to use Next(), Abort(), and Close().
/// </para>
/// </remarks>
/// <typeparam name="ReturnType">The type of value returned by Next() and AsyncNext()</typeparam>
/// <typeparam name="ParamType">The type of the parameter passed to Next() and AsyncNext()</typeparam>
public interface Generator1<ReturnType, ParamType>
{
    /// <summary>
    /// Advance the generator
    /// </summary>
    /// <remarks>
    /// Next() advances the generator to retrieve the next value. This version of
    /// Generator includes passing a parameter v to the generator.
    /// </remarks>
    /// <param name="param">Parameter to pass to generator</param>
    /// <returns>Return value from generator</returns>
    ReturnType Next(ParamType param);
    /// <summary>
    /// Try to advance the generator. Returns false if there are no more values.
    /// </summary>
    /// <remarks>
    /// Same as Next() but returns false if there are no more values.
    /// </remarks>
    /// <param name="param">Parameter to pass to generator</param>
    /// <param name="value">Return value from generator</param>
    /// <returns>True if more values are available, otherwise false</returns>
    bool TryNext(ParamType param, out ReturnType value);

    /// <summary>
    /// Asynchronously advance the generator
    /// </summary>
    /// <remarks>
    /// Same as Next() but async.
    /// </remarks>
    /// <param name="param">Parameter to pass to generator</param>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout.</param>
    /// <returns>A task that returns the return value upon completion</returns>
    Task<ReturnType> AsyncNext(ParamType param, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
    /// <summary>
    /// Abort the generator
    /// </summary>
    /// <remarks>
    /// Aborts and destroys the generator. This is assumed to be an error condition. Next() should throw
    /// OperationAbortedException if called after Abort(). Any ongoing operations should be terminated with an error
    /// condition, for example a moving robot should be immediately halted.
    /// </remarks>
    void Abort();
    /// <summary>
    /// Asynchronously abort the generator
    /// </summary>
    /// <remarks>
    /// Same as Abort() but async.
    /// </remarks>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout.</param>
    Task AsyncAbort(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
    /// <summary>
    /// Close the generator
    /// </summary>
    /// <remarks>
    /// Closes the generator. Closing the generator terminates iteration and destroys the generator.
    /// This operation cleanly closes the generator, and is not considered to be an error condition. Next()
    /// should throw StopIterationException if called after Close().
    /// </remarks>
    void Close();
    /// <summary>
    /// Asynchronously closes the generator
    /// </summary>
    /// <remarks>
    /// Same as Close() but returns async.
    /// </remarks>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout.</param>
    Task AsyncClose(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
}

/// <summary>
/// Generator type for use with generator functions, with return
/// </summary>
/// <remarks>
/// <para>
/// Generators are used with generator functions to implement simple coroutines. They are
/// returned by function members with a parameter and/or return marked with the
/// generator container type. Robot Raconteur generators are modeled on Python generators,
/// and are intended to be used in two scenarios:
/// 1. Transfering large parameter values or return values that would be over the message
/// transfer limit (typically around 10 MB).
/// 2. Long running operations that return updates or require periodic input. Generators
/// are used to implement functionality similar to "actions" in ROS.
/// </para>
/// <para>
/// Generators are a generalization of iterators, where a value is returned every time
/// the iterator is advanced until there are no more values. Python and Robot Raconteur iterators
/// add the option of passing a parameter every advance, allowing for simple coroutines. The
/// generator is advanced by calling the Next() or AsyncNext() functions. These functions
/// will either return a value or throw StopIterationException if there are no more values. Next()
/// and AsyncNext() may also throw any valid Robot Raconteur exception.
/// </para>
/// <para>
/// Generators can be terminated with either the Close() or Abort() functions. Close() should be
/// used to cleanly close the generator, and is not considered an error condition. Next(), if called
/// after close, should throw StopIterationException. Abort() is considered an error condition, and
/// will cause any action assosciated with the generator to be aborted as quickly as possible (ie faulting
/// a robot). If Next() is called after Abort(), OperationAbortedException should be thrown.
/// </para>
/// <para>
/// Robot Raconteur clients will return a populated stub generator that calls the service. Services
/// are expected to return a subclass of Generator. The service will call AsyncNext(), AsyncAbort(), and AsyncClose().
/// Inherit from SyncGenerator to use Next(), Abort(), and Close().
/// </para>
/// </remarks>
/// <typeparam name="ReturnType">Return The type of value returned by Next() and AsyncNext()</typeparam>
public interface Generator2<ReturnType>
{
    /// <summary>
    /// Advance the generator
    /// </summary>
    /// <remarks>
    /// Next() advances the generator to retrieve the next value. This version of
    ///  Generator does not include passing a parameter to the generator.
    /// </remarks>
    /// <returns>Return Return value from generator</returns>
    ReturnType Next();
    /// <summary>
    /// Try to advance the generator. Returns false if there are no more values.
    /// </summary>
    /// <remarks>
    /// Same as Next() but returns false if there are no more values.
    /// </remarks>
    /// <param name="value">Return value from generator</param>
    /// <returns>True if more values are available, otherwise false</returns>
    bool TryNext(out ReturnType value);
    /// <summary>
    /// Asynchronously advance the generator
    /// </summary>
    /// <remarks>
    /// Same as Next() but async.
    /// </remarks>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout.</param>
    Task<ReturnType> AsyncNext(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
    /// <summary>
    /// Abort the generator
    /// </summary>
    /// <remarks>
    /// Aborts and destroys the generator. This is assumed to be an error condition. Next() should throw
    /// OperationAbortedException if called after Abort(). Any ongoing operations should be terminated with an error
    /// condition, for example a moving robot should be immediately halted.
    /// </remarks>
    void Abort();
    /// <summary>
    /// Asynchronously abort the generator
    /// </summary>
    /// <remarks>
    /// Same as Abort() but async.
    /// </remarks>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout.</param>
    Task AsyncAbort(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
    /// <summary>
    /// Close the generator
    /// </summary>
    /// <remarks>
    /// Closes the generator. Closing the generator terminates iteration and destroys the generator.
    /// This operation cleanly closes the generator, and is not considered to be an error condition. Next()
    /// should throw StopIterationException if called after Close().
    /// </remarks>
    void Close();
    /// <summary>
    /// Asynchronously closes the generator
    /// </summary>
    /// <remarks>
    /// Same as Close() but async.
    /// </remarks>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout.</param>
    Task AsyncClose(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
    /// <summary>
    /// Automatically call Next() repeatedly and return array of results
    /// </summary>
    /// <returns>All values returned by generator Next()</returns>
    ReturnType[] NextAll();
}

/// <summary>
/// Generator type for use with generator functions, with parameter
/// </summary>
/// <remarks>
/// <para>
/// Generators are used with generator functions to implement simple coroutines. They are
/// returned by function members with a parameter and/or return marked with the
/// generator container type. Robot Raconteur generators are modeled on Python generators,
/// and are intended to be used in two scenarios:
/// 1. Transfering large parameter values or return values that would be over the message
/// transfer limit (typically around 10 MB).
/// 2. Long running operations that return updates or require periodic input. Generators
/// are used to implement functionality similar to "actions" in ROS.
/// </para>
/// <para>
/// Generators are a generalization of iterators, where a value is returned every time
/// the iterator is advanced until there are no more values. Python and Robot Raconteur iterators
/// add the option of passing a parameter every advance, allowing for simple coroutines. The
/// generator is advanced by calling the Next() or AsyncNext() functions. These functions
/// will either return a value or throw StopIterationException if there are no more values. Next()
/// and AsyncNext() may also throw any valid Robot Raconteur exception.
/// </para>
/// <para>
/// Generators can be terminated with either the Close() or Abort() functions. Close() should be
/// used to cleanly close the generator, and is not considered an error condition. Next(), if called
/// after close, should throw StopIterationException. Abort() is considered an error condition, and
/// will cause any action assosciated with the generator to be aborted as quickly as possible (ie faulting
/// a robot). If Next() is called after Abort(), OperationAbortedException should be thrown.
/// </para>
/// <para>
/// Robot Raconteur clients will return a populated stub generator that calls the service. Services
/// are expected to return a subclass of Generator. The service will call AsyncNext(), AsyncAbort(), and AsyncClose().
/// Inherit from SyncGenerator to use Next(), Abort(), and Close().
/// </para>
/// </remarks>
/// <typeparam name="ParamType">The type of the parameter passed to Next() and AsyncNext()</typeparam>
public interface Generator3<ParamType>
{
    /// <summary>
    /// Advance the generator
    /// </summary>
    /// <remarks>
    /// Next() advances the generator to retrieve the next value. This version of
    /// Generator includes passing a parameter to the generator but no return.
    /// </remarks>
    /// <param name="param">Parameter to pass to generator</param>
    void Next(ParamType param);
    /// <summary>
    /// Asynchronously advance the generator
    /// </summary>
    /// <remarks>
    /// Same as Next() but async.
    /// </remarks>
    /// <param name="param">Parameter to pass to generator</param>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout.</param>

    /// <summary>
    /// Try to advance the generator. Returns false if there are no more values.
    /// </summary>
    /// <remarks>
    /// Same as Next() but returns false if there are no more values.
    /// </remarks>
    /// <param name="param">Parameter to pass to generator</param>
    /// <returns>True if more values are available, otherwise false</returns>
    bool TryNext(ParamType param);
    Task AsyncNext(ParamType param, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
    /// <summary>
    /// Abort the generator
    /// </summary>
    /// <remarks>
    /// Aborts and destroys the generator. This is assumed to be an error condition. Next() should throw
    /// OperationAbortedException if called after Abort(). Any ongoing operations should be terminated with an error
    /// condition, for example a moving robot should be immediately halted.
    /// </remarks>
    void Abort();
    /// <summary>
    /// Asynchronously abort the generator
    /// </summary>
    /// <remarks>
    /// Same as Abort() but async.
    /// </remarks>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout.</param>
    Task AsyncAbort(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
    /// <summary>
    /// Close the generator
    /// </summary>
    /// <remarks>
    /// Closes the generator. Closing the generator terminates iteration and destroys the generator.
    /// This operation cleanly closes the generator, and is not considered to be an error condition. Next()
    /// should throw StopIterationException if called after Close().
    /// </remarks>
    void Close();
    /// <summary>
    /// Asynchronously closes the generator
    /// </summary>
    /// <remarks>
    /// Same as Close() but returns asynchronously.
    /// </remarks>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout.</param>
    Task AsyncClose(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
}

/// <summary>
/// Adapter base class for synchronous return generators.
/// </summary>
/// <remarks>
/// By default, the service will call AsyncNext, AsyncAbort, and AsyncClose. Use the
/// SyncGenerator abstract base classes for synchronous generators.
/// </remarks>
/// <typeparam name="ReturnType">Return The type of value returned by Next()</typeparam>
/// <typeparam name="ParamType">The type of the parameter passed to Next()</typeparam>
public abstract class SyncGenerator1<ReturnType, ParamType> : Generator1<ReturnType, ParamType>
{
    /// <summary>
    /// Abstract function to receive abort requests
    /// </summary>
    /// <remarks>None</remarks>
    public abstract void Abort();
    /// <summary>
    /// Abstract function to receive close requests
    /// </summary>
    /// <remarks>None</remarks>
    public abstract void Close();
    /// <summary>
    /// Abstract function te receive next requests
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="param">Next param</param>
    /// <returns>Next return</returns>
    public abstract ReturnType Next(ParamType param);

    public bool TryNext(ParamType param, out ReturnType ret)
    {
        try
        {
            ret = Next(param);
            return true;
        }
        catch (StopIterationException)
        {
            ret = default(ReturnType);
            return false;
        }
    }

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

/// <summary>
/// Adapter base class for synchronous return generators.
/// </summary>
/// <remarks>
/// By default, the service will call AsyncNext, AsyncAbort, and AsyncClose. Use the
/// SyncGenerator abstract base classes for synchronous generators.
/// </remarks>
/// <typeparam name="ReturnType">Return The type of value returned by Next()</typeparam>
public abstract class SyncGenerator2<ReturnType> : Generator2<ReturnType>
{
    /// <summary>
    /// Abstract function to receive abort requests
    /// </summary>
    /// <remarks>None</remarks>
    public abstract void Abort();
    /// <summary>
    /// Abstract function to receive close requests
    /// </summary>
    /// <remarks>None</remarks>
    public abstract void Close();
    /// <summary>
    /// Abstract function te receive next requests
    /// </summary>
    /// <remarks>None</remarks>
    /// <returns>Next return</returns>
    public abstract ReturnType Next();

    public bool TryNext(out ReturnType ret)
    {
        try
        {
            ret = Next();
            return true;
        }
        catch (StopIterationException)
        {
            ret = default(ReturnType);
            return false;
        }
    }

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
        catch (StopIterationException)
        {}
        return o.ToArray();
    }
}

/// <summary>
/// Adapter base class for synchronous return generators.
/// </summary>
/// <remarks>
/// By default, the service will call AsyncNext, AsyncAbort, and AsyncClose. Use the
/// SyncGenerator abstract base classes for synchronous generators.
/// </remarks>
/// <typeparam name="ParamType">The type of the parameter passed to Next()</typeparam>
public abstract class SyncGenerator3<ParamType> : Generator3<ParamType>
{
    /// <summary>
    /// Abstract function to receive abort requests
    /// </summary>
    /// <remarks>None</remarks>
    public abstract void Abort();
    /// <summary>
    /// Abstract function to receive close requests
    /// </summary>
    /// <remarks>None</remarks>
    public abstract void Close();
    /// <summary>
    /// Abstract function te receive next requests
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="param">Next param</param>
    public abstract void Next(ParamType param);

    public bool TryNext(ParamType param)
    {
        try
        {
            Next(param);
            return true;
        }
        catch (StopIterationException)
        {
            return false;
        }
    }

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

/// <summary>
/// Adapter class to create a generator from an enumerator
/// </summary>
/// <remarks>
/// Next calls will be mapped to the supplied enumerator
/// </remarks>
/// <typeparam name="T">The enumerator value type</typeparam>
public class EnumeratorGenerator<T> : SyncGenerator2<T>
{
    bool aborted = false;
    bool closed = false;
    IEnumerator<T> enumerator;

    /// <summary>
    /// Construct a generator from an IEnumerable
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="enumerable"></param>
    /// <returns></returns>
    public EnumeratorGenerator(IEnumerable<T> enumerable) : this(enumerable.GetEnumerator())
    {}

    /// <summary>
    /// Construct a generator from an IEnumerator
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="enumerator"></param>
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
            if (aborted)
                throw new OperationAbortedException("Generator aborted");
            if (closed)
                throw new StopIterationException("");
            if (!enumerator.MoveNext())
                throw new StopIterationException("");
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
        using (MessageElement m = RobotRaconteurNode.s.PackAnyType<ParamType>("parameter", ref param))
        {

            using (MessageElement m2 = inner_gen.Next(m))
            {
                return RobotRaconteurNode.s.UnpackAnyType<ReturnType>(m2);
            }
        }
    }

    public bool TryNext(ParamType param, out ReturnType ret)
    {
        using (MessageElement m = RobotRaconteurNode.s.PackAnyType<ParamType>("parameter", ref param))
        {
            using (WrappedGeneratorClient_TryGetNextResult res1 = inner_gen.TryNext(m))
            {
                if (!res1.res)
                {
                    ret = default(ReturnType);
                    return false;
                }
                using (MessageElement m2 = res1.value)
                {
                    ret = RobotRaconteurNode.s.UnpackAnyType<ReturnType>(m2);
                    return true;
                }
            }
        }
    }
    public async Task<ReturnType> AsyncNext(ParamType param, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        using (MessageElement m = RobotRaconteurNode.s.PackAnyType<ParamType>("parameter", ref param))
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

    public bool TryNext(out ReturnType ret)
    {
        using (WrappedGeneratorClient_TryGetNextResult res1 = inner_gen.TryNext(null))
        {
            if (!res1.res)
            {
                ret = default(ReturnType);
                return false;
            }
            using (MessageElement m2 = res1.value)
            {
                ret = RobotRaconteurNode.s.UnpackAnyType<ReturnType>(m2);
                return true;
            }
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
        catch (StopIterationException)
        {}
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
        using (MessageElement m = RobotRaconteurNode.s.PackAnyType<ParamType>("parameter", ref param))
        {
            inner_gen.Next(m);
        }
    }

    public bool TryNext(ParamType param)
    {
        using (MessageElement m = RobotRaconteurNode.s.PackAnyType<ParamType>("parameter", ref param))
        {
            using (WrappedGeneratorClient_TryGetNextResult res1 = inner_gen.TryNext(m))
            {
                return res1.res;
            }
        }
    }
    public async Task AsyncNext(ParamType param, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        using (MessageElement m = RobotRaconteurNode.s.PackAnyType<ParamType>("parameter", ref param))
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
        if (generator == null)
            throw new NullReferenceException("Generator must not be null");
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
                generator.AsyncNext(p).ContinueWith(
                    t => async_adapter.EndTask<ReturnType>(
                        t, async_ret => RobotRaconteurNode.s.PackAnyType<ReturnType>("return", ref async_ret)));
                return null;
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr, e.ToString());
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
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
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
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
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
        if (generator == null)
            throw new NullReferenceException("Generator must not be null");
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
                generator.AsyncNext().ContinueWith(
                    t => async_adapter.EndTask<ReturnType>(
                        t, async_ret => RobotRaconteurNode.s.PackAnyType<ReturnType>("return", ref async_ret)));
                return null;
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr, e.ToString());
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
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
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
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
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
        if (generator == null)
            throw new NullReferenceException("Generator must not be null");
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
                generator.AsyncNext(p).ContinueWith(t => async_adapter.EndTask(t));
                return null;
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr, e.ToString());
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
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
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
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return;
            }
        }
    }
}

internal class AsyncGeneratorClientReturnDirectorImpl : AsyncGeneratorClientReturnDirector
{
    protected TaskCompletionSource<WrappedGeneratorClient> handler_task =
        new TaskCompletionSource<WrappedGeneratorClient>(TaskContinuationOptions.ExecuteSynchronously);

    public Task<WrappedGeneratorClient> Task
    {
        get {
            return handler_task.Task;
        }
    }

    public AsyncGeneratorClientReturnDirectorImpl()
    {}

    public override void handler(WrappedGeneratorClient m, HandlerErrorInfo error)
    {
        // using (m)
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
                    RRDirectorExceptionHelper.SetError(merr, e.ToString());
                }
            }
        }
    }
}
}
