/**
 * @file Generator.h
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

#pragma once

#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/Endpoint.h"
#include "RobotRaconteur/Client.h"
#include "RobotRaconteur/Service.h"

namespace RobotRaconteur
{
/**
 * @brief Generator type for use with generator functions, with parameter and return
 *
 * Generators are used with generator functions to implement simple coroutines. They are
 * returned by function members with a parameter and/or return marked with the
 * generator container type. Robot Raconteur generators are modeled on Python generators,
 * and are intended to be used in two scenarios:
 * 1. Transfering large parameter values or return values that would be over the message
 * transfer limit (typically around 10 MB).
 * 2. Long running operations that return updates or require periodic input. Generators
 * are used to implement functionality similar to "actions" in ROS.
 *
 * Generators are a generalization of iterators, where a value is returned every time
 * the iterator is advanced until there are no more values. Python and Robot Raconteur iterators
 * add the option of passing a parameter every advance, allowing for simple coroutines. The
 * generator is advanced by calling the Next() or AsyncNext() functions. These functions
 * will either return a value or throw StopIterationException if there are no more values. Next()
 * and AsyncNext() may also throw any valid Robot Raconteur exception.
 *
 * Generators can be terminated with either the Close() or Abort() functions. Close() should be
 * used to cleanly close the generator, and is not considered an error condition. Next(), if called
 * after close, should throw StopIterationException. Abort() is considered an error condition, and
 * will cause any action assosciated with the generator to be aborted as quickly as possible (ie faulting
 * a robot). If Next() is called after Abort(), OperationAbortedException should be thrown.
 *
 * Robot Raconteur clients will return a populated stub generator that calls the service. Services
 * are expected to return a subclass of Generator that implements at a minimum Next(), Close(), and Abort().
 * AsyncNext(), AsyncAbort(), and AsyncClose() may optionally be implemented for asynchronous operation on the
 * service side.
 *
 * @tparam Return The type of value returned by Next() and AsyncNext()
 * @tparam Param The type of the parameter passed to Next() and AsyncNext()
 */

template <typename Return, typename Param>
class Generator : private boost::noncopyable
{
  public:
    /**
     * @brief Advance the generator
     *
     * Next() advances the generator to retrieve the next value. This version of
     * Generator includes passing a parameter v to the generator.
     *
     * @param v Parameter to pass to generator
     * @return Return Return value from generator
     */
    virtual Return Next(const Param& v) = 0;
    /**
     * @brief Try to advance the generator. Return false if no more values are available.
     *
     * TryNext() is similar to Next() but returns false if no more values are available.
     * This is useful for generators that may not have a fixed number of values.
     *
     * @param v Parameter to pass to generator
     * @param ret Return value from generator
     * @return true ret is valid
     * @return false ret is not valid. No more values are available.
     */
    virtual bool TryNext(const Param& v, Return& ret)
    {
        // TODO: This is not the most efficient implementation
        try
        {
            ret = Next(v);
            return true;
        }
        catch (StopIterationException&)
        {
            return false;
        }
    }
    /**
     * @brief Asynchronously advance the generator
     *
     * Same as Next() but returns asynchronously.
     *
     * @param v Parameter to pass to generator
     * @param handler A handler function to receive the return value or an exception
     * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout.
     */
    virtual void AsyncNext(
        const Param& v, boost::function<void(const Return&, const RR_SHARED_PTR<RobotRaconteurException>& err)> handler,
        int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
    /**
     * @brief Abort the generator
     *
     * Aborts and destroys the generator. This is assumed to be an error condition. Next() should throw
     * OperationAbortedException if called after Abort(). Any ongoing operations should be terminated with an error
     * condition, for example a moving robot should be immediately halted.
     */
    virtual void Abort() = 0;
    /**
     * @brief Asynchronously abort the generator
     *
     * Same as Abort() but returns asynchronously.
     *
     * @param handler The handler to call when abort is complete
     * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout.
     */
    virtual void AsyncAbort(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>& err)> handler,
                            int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
    /**
     * @brief Close the generator
     *
     * Closes the generator. Closing the generator terminates iteration and destroys the generator.
     * This operation cleanly closes the generator, and is not considered to be an error condition. Next()
     * should throw StopIterationException if called after Close().
     *
     */
    virtual void Close() = 0;
    /**
     * @brief Asynchronously closes the generator
     *
     * Same as Close() but returns asynchronously.
     *
     * @param handler The handler to call when close is complete
     * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout.
     */
    virtual void AsyncClose(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>& err)> handler,
                            int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
    virtual ~Generator() {}
};

/**
 * @brief Generator type for use with generator functions, with return
 *
 * @copydetails Generator
 *
 */
template <typename Return>
class Generator<Return, void> : private boost::noncopyable
{
  public:
    /**
     * @copybrief Generator::Next()
     *
     *  Next() advances the generator to retrieve the next value. This version of
     *  Generator does not include passing a parameter to the generator.
     *
     * @return Return Return value from generator
     */
    virtual Return Next() = 0;

    /**
     * @copybrief Generator::TryNext()
     *
     * TryNext() is similar to Next() but returns false if no more values are available. This version
     * of TryNext() does not include passing a parameter to the generator.
     *
     * @param ret Return value from generator
     * @return true ret is valid
     * @return false ret is not valid. No more values are available.
     */
    virtual bool TryNext(Return& ret)
    {
        try
        {
            ret = Next();
            return true;
        }
        catch (StopIterationException&)
        {
            return false;
        }
    }
    /**
     * @copybrief Generator::AsyncNext()
     *
     * Same as Next() but returns asynchronously.
     *
     * @param handler A handler function to receive the return value or an exception
     * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout.
     */
    virtual void AsyncNext(
        boost::function<void(const Return&, const RR_SHARED_PTR<RobotRaconteurException>& err)> handler,
        int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
    /** @copydoc Generator::Abort() */
    virtual void Abort() = 0;
    /** @copydoc Generator::AsyncAbort() */
    virtual void AsyncAbort(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>& err)> handler,
                            int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
    /** @copydoc Generator::Close() */
    virtual void Close() = 0;
    /** @copydoc Generator::AsyncClose() */
    virtual void AsyncClose(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>& err)> handler,
                            int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
    /**
     * @brief Automatically call Next() repeatedly and return std::vector of results
     *
     * @return std::vector<Return> All values returned by generator Next()
     */
    virtual std::vector<Return> NextAll()
    {
        std::vector<Return> ret;
        try
        {
            while (true)
            {
                ret.push_back(Next());
            }
        }
        catch (StopIterationException&)
        {}
        return ret;
    }
    virtual ~Generator() {}
};

/**
 * @brief Generator type for use with generator functions, with parameter
 *
 * @copydetails Generator
 *
 */
template <typename Param>
class Generator<void, Param> : private boost::noncopyable
{
  public:
    /**
     * @copybrief Generator::Next()
     *
     *  Next() advances the generator to retrieve the next value. This version of
     *  Generator includes passing a parameter to the generator.
     *
     * @param v Parameter to pass to generator
     */
    virtual void Next(const Param& v) = 0;

    /**
     * @copybrief Generator::TryNext()
     *
     * TryNext() is similar to Next() but returns false if no more values are available. This version
     * of TryNext() includes passing a parameter to the generator.
     *
     * @param v The parameter to pass to the generator
     * @return true Valid return
     * @return false No more values are available
     */
    virtual bool TryNext(const Param& v)
    {
        try
        {
            Next(v);
        }
        catch (StopIterationException&)
        {
            return false;
        }
        return true;
    }
    /**
     * @copybrief Generator::AsyncNext()
     *
     * Same as Next() but returns asynchronously.
     *
     * @param v Parameter to pass to generator
     * @param handler The handler to call when next is complete
     * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout.
     */
    virtual void AsyncNext(const Param& v,
                           boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>& err)> handler,
                           int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
    /** @copydoc Generator::Abort() */
    virtual void Abort() = 0;
    /** @copydoc Generator::AsyncAbort() */
    virtual void AsyncAbort(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>& err)> handler,
                            int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
    /** @copydoc Generator::Close() */
    virtual void Close() = 0;
    /** @copydoc Generator::AsyncClose() */
    virtual void AsyncClose(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                            int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
    virtual ~Generator() {}
};

class ROBOTRACONTEUR_CORE_API GeneratorClientBase
{
  protected:
    std::string name;
    int32_t id;
    RR_WEAK_PTR<ServiceStub> stub;
    RR_WEAK_PTR<RobotRaconteurNode> node;
    uint32_t endpoint;
    std::string service_path;

    GeneratorClientBase(boost::string_ref name, int32_t id, const RR_SHARED_PTR<ServiceStub>& stub);

    virtual RR_INTRUSIVE_PTR<MessageElement> NextBase(const RR_INTRUSIVE_PTR<MessageElement>& v);
    virtual void AsyncNextBase(
        const RR_INTRUSIVE_PTR<MessageElement>& v,
        boost::function<void(const RR_INTRUSIVE_PTR<MessageElement>&, const RR_SHARED_PTR<RobotRaconteurException>&,
                             const RR_SHARED_PTR<RobotRaconteurNode>&)>
            handler,
        int32_t timeout);

    static void AsyncNextBase1(
        const RR_INTRUSIVE_PTR<MessageEntry>& ret, const RR_SHARED_PTR<RobotRaconteurException>& err,
        boost::function<void(const RR_INTRUSIVE_PTR<MessageElement>&, const RR_SHARED_PTR<RobotRaconteurException>&,
                             const RR_SHARED_PTR<RobotRaconteurNode>&)>
            handler,
        const RR_WEAK_PTR<RobotRaconteurNode>& node);

  public:
    RR_SHARED_PTR<ServiceStub> GetStub();

    virtual void Abort();
    virtual void AsyncAbort(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                            int32_t timeout = RR_TIMEOUT_INFINITE);
    virtual void Close();
    virtual void AsyncClose(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                            int32_t timeout = RR_TIMEOUT_INFINITE);
    std::string GetMemberName();

    virtual ~GeneratorClientBase() {}
};

namespace detail
{
template <typename Return>
static void GeneratorClient_AsyncNext1(
    const RR_INTRUSIVE_PTR<MessageElement>& v2, const RR_SHARED_PTR<RobotRaconteurException>& err,
    const RR_SHARED_PTR<RobotRaconteurNode>& node,
    boost::function<void(const Return&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler)
{
    if (err)
    {
        detail::InvokeHandlerWithException(node, handler, err);
        return;
    }
    Return ret;
    try
    {
        ret = RRPrimUtil<Return>::PreUnpack(node->UnpackAnyType<typename RRPrimUtil<Return>::BoxedType>(v2));
    }
    catch (std::exception& e)
    {
        detail::InvokeHandlerWithException(node, handler, e);
        return;
    }
    detail::InvokeHandler<Return>(node, handler, ret);
}

ROBOTRACONTEUR_CORE_API void GeneratorClient_AsyncNext2(
    const RR_INTRUSIVE_PTR<MessageElement>& v2, const RR_SHARED_PTR<RobotRaconteurException>& err,
    const RR_SHARED_PTR<RobotRaconteurNode>& node,
    boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler);
} // namespace detail

template <typename Return, typename Param>
class GeneratorClient : public Generator<Return, Param>, public GeneratorClientBase
{
  public:
    GeneratorClient(boost::string_ref name, int32_t id, const RR_SHARED_PTR<ServiceStub>& stub)
        : GeneratorClientBase(name, id, stub)
    {}

    RR_OVIRTUAL Return Next(const Param& v) RR_OVERRIDE
    {
        RR_INTRUSIVE_PTR<MessageElement> v1 = CreateMessageElement(
            "", GetStub()->RRGetNode()->template PackAnyType<typename RRPrimUtil<Param>::BoxedType>(
                    RRPrimUtil<Param>::PrePack(v)));
        RR_INTRUSIVE_PTR<MessageElement> v2 = NextBase(v1);
        return RRPrimUtil<Return>::PreUnpack(
            GetStub()->RRGetNode()->template UnpackAnyType<typename RRPrimUtil<Return>::BoxedType>(v2));
    }
    RR_OVIRTUAL void AsyncNext(
        const Param& v, boost::function<void(const Return&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
        int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        RR_INTRUSIVE_PTR<MessageElement> v1 = CreateMessageElement(
            "", GetStub()->RRGetNode()->template PackAnyType<typename RRPrimUtil<Param>::BoxedType>(
                    RRPrimUtil<Param>::PrePack(v)));
        AsyncNextBase(v1,
                      boost::bind<void>(&detail::GeneratorClient_AsyncNext1<Return>, RR_BOOST_PLACEHOLDERS(_1),
                                        RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3), handler),
                      timeout);
    }
    RR_OVIRTUAL void Abort() RR_OVERRIDE { GeneratorClientBase::Abort(); }
    RR_OVIRTUAL void AsyncAbort(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                                int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        GeneratorClientBase::AsyncAbort(handler, timeout);
    }
    RR_OVIRTUAL void Close() RR_OVERRIDE { GeneratorClientBase::Close(); }
    RR_OVIRTUAL void AsyncClose(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                                int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        GeneratorClientBase::AsyncAbort(handler, timeout);
    }
};

template <typename Return>
class GeneratorClient<Return, void> : public Generator<Return, void>, public GeneratorClientBase
{
  public:
    GeneratorClient(boost::string_ref name, int32_t id, const RR_SHARED_PTR<ServiceStub>& stub)
        : GeneratorClientBase(name, id, stub)
    {}

    RR_OVIRTUAL Return Next() RR_OVERRIDE
    {
        RR_INTRUSIVE_PTR<MessageElement> v2 = NextBase(RR_INTRUSIVE_PTR<MessageElement>());
        return RRPrimUtil<Return>::PreUnpack(
            GetStub()->RRGetNode()->template UnpackAnyType<typename RRPrimUtil<Return>::BoxedType>(v2));
    }
    RR_OVIRTUAL void AsyncNext(
        boost::function<void(const Return&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
        int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        AsyncNextBase(RR_INTRUSIVE_PTR<MessageElement>(),
                      boost::bind<void>(&detail::GeneratorClient_AsyncNext1<Return>, RR_BOOST_PLACEHOLDERS(_1),
                                        RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3), handler),
                      timeout);
    }
    RR_OVIRTUAL void Abort() RR_OVERRIDE { GeneratorClientBase::Abort(); }
    RR_OVIRTUAL void AsyncAbort(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                                int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        GeneratorClientBase::AsyncAbort(handler, timeout);
    }
    RR_OVIRTUAL void Close() RR_OVERRIDE { GeneratorClientBase::Close(); }
    RR_OVIRTUAL void AsyncClose(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                                int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        GeneratorClientBase::AsyncAbort(handler, timeout);
    }
};

template <typename Param>
class GeneratorClient<void, Param> : public Generator<void, Param>, public GeneratorClientBase
{
  public:
    GeneratorClient(boost::string_ref name, int32_t id, const RR_SHARED_PTR<ServiceStub>& stub)
        : GeneratorClientBase(name, id, stub)
    {}

    RR_OVIRTUAL void Next(const Param& v) RR_OVERRIDE
    {
        RR_INTRUSIVE_PTR<MessageElement> v1 = CreateMessageElement(
            "", GetStub()->RRGetNode()->template PackAnyType<typename RRPrimUtil<Param>::BoxedType>(
                    RRPrimUtil<Param>::PrePack(v)));
        NextBase(v1);
    }
    RR_OVIRTUAL void AsyncNext(const Param& v,
                               boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                               int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        RR_INTRUSIVE_PTR<MessageElement> v1 = CreateMessageElement(
            "", GetStub()->RRGetNode()->template PackAnyType<typename RRPrimUtil<Param>::BoxedType>(
                    RRPrimUtil<Param>::PrePack(v)));
        AsyncNextBase(v1,
                      boost::bind<void>(&detail::GeneratorClient_AsyncNext2, RR_BOOST_PLACEHOLDERS(_1),
                                        RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3), RR_MOVE(handler)),
                      timeout);
    }
    RR_OVIRTUAL void Abort() RR_OVERRIDE { GeneratorClientBase::Abort(); }
    RR_OVIRTUAL void AsyncAbort(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                                int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        GeneratorClientBase::AsyncAbort(handler, timeout);
    }
    RR_OVIRTUAL void Close() RR_OVERRIDE { GeneratorClientBase::Close(); }
    RR_OVIRTUAL void AsyncClose(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                                int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        GeneratorClientBase::AsyncAbort(handler, timeout);
    }
};

class ServerEndpoint;

class ROBOTRACONTEUR_CORE_API GeneratorServerBase : private boost::noncopyable
{
  protected:
    std::string name;
    int32_t index;
    RR_WEAK_PTR<ServiceSkel> skel;
    RR_INTRUSIVE_PTR<MessageElement> m;
    RR_SHARED_PTR<ServerEndpoint> ep;
    RR_WEAK_PTR<RobotRaconteurNode> node;
    std::string service_path;

    boost::posix_time::ptime last_access_time;

    GeneratorServerBase(boost::string_ref name, int32_t index, const RR_SHARED_PTR<ServiceSkel>& skel,
                        const RR_SHARED_PTR<ServerEndpoint>& ep);

  public:
    friend class ServiceSkel;

    virtual void CallNext(const RR_INTRUSIVE_PTR<MessageEntry>& m) = 0;

    virtual uint32_t GetEndpoint();

    virtual ~GeneratorServerBase() {}

  protected:
    static void EndAsyncCallNext(const RR_WEAK_PTR<ServiceSkel>& skel, const RR_INTRUSIVE_PTR<MessageElement>& ret,
                                 const RR_SHARED_PTR<RobotRaconteurException>& err, int32_t index,
                                 const RR_INTRUSIVE_PTR<MessageEntry>& m, const RR_SHARED_PTR<ServerEndpoint>& ep);

    template <typename Return>
    static void CallNext1(Return v2, const RR_SHARED_PTR<RobotRaconteurException>& err, int32_t index,
                          const RR_WEAK_PTR<ServiceSkel>& skel, const RR_INTRUSIVE_PTR<MessageEntry>& m,
                          const RR_SHARED_PTR<ServerEndpoint>& ep)
    {
        RR_SHARED_PTR<ServiceSkel> skel1 = skel.lock();
        RR_SHARED_PTR<RobotRaconteurException> err2 = err;
        // TODO: Improve null handling
        if (!skel1 && !err2)
        {
            err2 = RR_MAKE_SHARED<InvalidOperationException>("Service skel released");
        }
        if (err2)
        {
            GeneratorServerBase::EndAsyncCallNext(skel, RR_INTRUSIVE_PTR<MessageElement>(), err2, index, m, ep);
        }

        RR_INTRUSIVE_PTR<MessageElement> v3 =
            CreateMessageElement("", skel1->RRGetNode()->template PackAnyType<typename RRPrimUtil<Return>::BoxedType>(
                                         RRPrimUtil<Return>::PrePack(v2)));
        GeneratorServerBase::EndAsyncCallNext(skel, v3, err, index, m, ep);
    }

    static void CallNext2(const RR_SHARED_PTR<RobotRaconteurException>& err, int32_t index,
                          const RR_WEAK_PTR<ServiceSkel>& skel, const RR_INTRUSIVE_PTR<MessageEntry>& m,
                          const RR_SHARED_PTR<ServerEndpoint>& ep)
    {
        RR_SHARED_PTR<ServiceSkel> skel1 = skel.lock();
        RR_SHARED_PTR<RobotRaconteurException> err2 = err;
        // TODO: Improve null handling
        if (!skel1 && !err2)
        {
            err2 = RR_MAKE_SHARED<InvalidOperationException>("Service skel released");
        }
        if (err2)
        {
            GeneratorServerBase::EndAsyncCallNext(skel, RR_INTRUSIVE_PTR<MessageElement>(), err2, index, m, ep);
        }

        RR_INTRUSIVE_PTR<MessageElement> v3;
        GeneratorServerBase::EndAsyncCallNext(skel, v3, err2, index, m, ep);
    }
};

template <typename Return, typename Param>
class GeneratorServer : public GeneratorServerBase
{
  protected:
    RR_SHARED_PTR<Generator<Return, Param> > generator;

  public:
    GeneratorServer(const RR_SHARED_PTR<Generator<Return, Param> >& generator, boost::string_ref name, int32_t id,
                    const RR_SHARED_PTR<ServiceSkel>& skel, const RR_SHARED_PTR<ServerEndpoint>& ep)
        : GeneratorServerBase(name, id, skel, ep)
    {
        if (!generator)
            throw InvalidOperationException("Generator must not be null");
        this->generator = generator;
    }

    RR_OVIRTUAL void CallNext(const RR_INTRUSIVE_PTR<MessageEntry>& m) RR_OVERRIDE
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, ep->GetLocalEndpoint(), service_path, name,
                                                "Generator CallNext with id " << index);
        RR_SHARED_PTR<ServiceSkel> skel1 = skel.lock();
        if (!skel1)
        {
            throw InvalidOperationException("Skel has been released");
        }

        if (m->Error != MessageErrorType_None)
        {
            if (m->Error == MessageErrorType_StopIteration)
            {
                generator->AsyncClose(boost::bind<void>(&GeneratorServerBase::CallNext1<int32_t>, 0,
                                                        RR_BOOST_PLACEHOLDERS(_1), index, skel, m, ep));
            }
            else
            {
                generator->AsyncAbort(boost::bind<void>(&GeneratorServerBase::CallNext1<int32_t>, 0,
                                                        RR_BOOST_PLACEHOLDERS(_1), index, skel, m, ep));
            }
        }
        else
        {
            Param v = RRPrimUtil<Param>::PreUnpack(
                skel1->RRGetNode()->template UnpackAnyType<typename RRPrimUtil<Param>::BoxedType>(
                    m->FindElement("parameter")));
            generator->AsyncNext(v,
                                 boost::bind<void>(&GeneratorServerBase::CallNext1<Return>, RR_BOOST_PLACEHOLDERS(_1),
                                                   RR_BOOST_PLACEHOLDERS(_2), index, skel, m, ep));
        }
    }

    RR_OVIRTUAL ~GeneratorServer() RR_OVERRIDE {}
};

template <typename Return>
class GeneratorServer<Return, void> : public GeneratorServerBase
{
  protected:
    RR_SHARED_PTR<Generator<Return, void> > generator;

  public:
    GeneratorServer(RR_SHARED_PTR<Generator<Return, void> > generator, boost::string_ref name, int32_t id,
                    const RR_SHARED_PTR<ServiceSkel>& skel, const RR_SHARED_PTR<ServerEndpoint>& ep)
        : GeneratorServerBase(name, id, skel, ep)
    {
        if (!generator)
            throw InvalidOperationException("Generator must not be null");
        this->generator = generator;
    }

    RR_OVIRTUAL void CallNext(const RR_INTRUSIVE_PTR<MessageEntry>& m) RR_OVERRIDE
    {
        RR_SHARED_PTR<ServiceSkel> skel1 = skel.lock();
        if (!skel1)
        {
            throw InvalidOperationException("Skel has been released");
        }

        if (m->Error != MessageErrorType_None)
        {
            if (m->Error == MessageErrorType_StopIteration)
            {
                generator->AsyncClose(boost::bind<void>(&GeneratorServerBase::CallNext1<int32_t>, 0,
                                                        RR_BOOST_PLACEHOLDERS(_1), index, skel, m, ep));
            }
            else
            {
                generator->AsyncAbort(boost::bind<void>(&GeneratorServerBase::CallNext1<int32_t>, 0,
                                                        RR_BOOST_PLACEHOLDERS(_1), index, skel, m, ep));
            }
        }
        else
        {
            generator->AsyncNext(boost::bind<void>(&GeneratorServerBase::CallNext1<Return>, RR_BOOST_PLACEHOLDERS(_1),
                                                   RR_BOOST_PLACEHOLDERS(_2), index, skel, m, ep));
        }
    }
    RR_OVIRTUAL ~GeneratorServer() RR_OVERRIDE {}
};

template <typename Param>
class GeneratorServer<void, Param> : public GeneratorServerBase
{
  protected:
    RR_SHARED_PTR<Generator<void, Param> > generator;

  public:
    GeneratorServer(RR_SHARED_PTR<Generator<void, Param> > generator, boost::string_ref name, int32_t id,
                    const RR_SHARED_PTR<ServiceSkel>& skel, const RR_SHARED_PTR<ServerEndpoint>& ep)
        : GeneratorServerBase(name, id, skel, ep)
    {
        if (!generator)
            throw InvalidOperationException("Generator must not be null");
        this->generator = generator;
    }

    RR_OVIRTUAL void CallNext(const RR_INTRUSIVE_PTR<MessageEntry>& m) RR_OVERRIDE
    {
        RR_SHARED_PTR<ServiceSkel> skel1 = skel.lock();
        if (!skel1)
        {
            throw InvalidOperationException("Skel has been released");
        }

        if (m->Error != MessageErrorType_None)
        {
            if (m->Error == MessageErrorType_StopIteration)
            {
                generator->AsyncClose(boost::bind<void>(&GeneratorServerBase::CallNext1<int32_t>, 0,
                                                        RR_BOOST_PLACEHOLDERS(_1), index, skel, m, ep));
            }
            else
            {
                generator->AsyncAbort(boost::bind<void>(&GeneratorServerBase::CallNext1<int32_t>, 0,
                                                        RR_BOOST_PLACEHOLDERS(_1), index, skel, m, ep));
            }
        }
        else
        {
            Param v = RRPrimUtil<Param>::PreUnpack(
                skel1->RRGetNode()->template UnpackAnyType<typename RRPrimUtil<Param>::BoxedType>(
                    m->FindElement("parameter")));
            generator->AsyncNext(
                v, boost::bind<void>(&GeneratorServerBase::CallNext2, RR_BOOST_PLACEHOLDERS(_1), index, skel, m, ep));
        }
    }
    RR_OVIRTUAL ~GeneratorServer() RR_OVERRIDE {}
};

// NOLINTBEGIN(cppcoreguidelines-virtual-class-destructor)

/**
 * @brief Helper class for synchronous service generators
 *
 * By default, services will call AsyncNext(), AsyncClose(), and AsyncAbort(). This
 * class will redirect these calls to synchronous Next(), Close, and Abort(). Inherit
 * from SyncGenerator for this behavior.
 *
 */
template <typename Return, typename Param>
class SyncGenerator : public Generator<Return, Param>
{
  public:
    /** @copydoc Generator<Return,Param>::Next() */
    RR_OVIRTUAL Return Next(const Param& v) RR_OVERRIDE = 0;
    /** @copydoc Generator<Return,Param>::AsyncNext() */
    RR_OVIRTUAL void AsyncNext(
        const Param& v, boost::function<void(const Return&, const RR_SHARED_PTR<RobotRaconteurException>& err)> handler,
        int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        RR_UNUSED(timeout);
        Return r;
        RR_WEAK_PTR<RobotRaconteurNode> node;
        try
        {
            r = Next(v);
        }
        catch (std::exception& exp)
        {
            detail::InvokeHandlerWithException(node, handler, exp);
            return;
        }
        detail::InvokeHandler(node, handler, r);
    }
    /** @copydoc Generator<Return,Param>::Abort() */
    RR_OVIRTUAL void Abort() RR_OVERRIDE = 0;
    /** @copydoc Generator<Return,Param>::AsyncAbort() */
    RR_OVIRTUAL void AsyncAbort(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                                int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        RR_UNUSED(timeout);
        RR_WEAK_PTR<RobotRaconteurNode> node;
        try
        {
            Abort();
        }
        catch (std::exception& exp)
        {
            detail::InvokeHandlerWithException(node, handler, exp);
            return;
        }
        detail::InvokeHandler(node, handler);
    }
    /** @copydoc Generator<Return,Param>::Close() */
    RR_OVIRTUAL void Close() RR_OVERRIDE = 0;
    /** @copydoc Generator<Return,Param>::AsyncClose() */
    RR_OVIRTUAL void AsyncClose(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                                int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        RR_UNUSED(timeout);
        RR_WEAK_PTR<RobotRaconteurNode> node;
        try
        {
            Close();
        }
        catch (std::exception& exp)
        {
            detail::InvokeHandlerWithException(node, handler, exp);
            return;
        }
        detail::InvokeHandler(node, handler);
    }
    RR_OVIRTUAL ~SyncGenerator() RR_OVERRIDE {}
};

/** @copydoc SyncGenerator */
template <typename Return>
class SyncGenerator<Return, void> : public Generator<Return, void>
{
  public:
    /** @copydoc Generator<Return,void>::Next() */
    RR_OVIRTUAL Return Next() RR_OVERRIDE = 0;
    /** @copydoc Generator<Return,void>::AsyncNext() */
    RR_OVIRTUAL void AsyncNext(
        boost::function<void(const Return&, const RR_SHARED_PTR<RobotRaconteurException>& err)> handler,
        int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        RR_UNUSED(timeout);
        Return r;
        RR_WEAK_PTR<RobotRaconteurNode> node;
        try
        {
            r = Next();
        }
        catch (std::exception& exp)
        {
            detail::InvokeHandlerWithException(node, handler, exp);
            return;
        }
        detail::InvokeHandler(node, handler, r);
    }
    /** @copydoc Generator<Return,void>::Abort() */
    RR_OVIRTUAL void Abort() RR_OVERRIDE = 0;
    /** @copydoc Generator<Return,void>::AsyncAbort() */
    RR_OVIRTUAL void AsyncAbort(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                                int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        RR_UNUSED(timeout);
        RR_WEAK_PTR<RobotRaconteurNode> node;
        try
        {
            Abort();
        }
        catch (std::exception& exp)
        {
            detail::InvokeHandlerWithException(node, handler, exp);
            return;
        }
        detail::InvokeHandler(node, handler);
    }
    /** @copydoc Generator<Return,void>::Close() */
    RR_OVIRTUAL void Close() RR_OVERRIDE = 0;
    /** @copydoc Generator<Return,void>::AsyncClose() */
    RR_OVIRTUAL void AsyncClose(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                                int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        RR_UNUSED(timeout);
        RR_WEAK_PTR<RobotRaconteurNode> node;
        try
        {
            Close();
        }
        catch (std::exception& exp)
        {
            detail::InvokeHandlerWithException(node, handler, exp);
            return;
        }
        detail::InvokeHandler(node, handler);
    }
    RR_OVIRTUAL ~SyncGenerator() RR_OVERRIDE {}
};

/** @copydoc SyncGenerator */
template <typename Param>
class SyncGenerator<void, Param> : public Generator<void, Param>
{
  public:
    /** @copydoc Generator<void,Param>::Next() */
    RR_OVIRTUAL void Next(const Param& v) RR_OVERRIDE = 0;
    /** @copydoc Generator<void,Param>::AsyncNext() */
    RR_OVIRTUAL void AsyncNext(const Param& v,
                               boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                               int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        RR_UNUSED(timeout);
        RR_WEAK_PTR<RobotRaconteurNode> node;
        try
        {
            Next(v);
        }
        catch (std::exception& exp)
        {
            detail::InvokeHandlerWithException(node, handler, exp);
            return;
        }
        detail::InvokeHandler(node, handler);
    }
    /** @copydoc Generator<void,Param>::Abort() */
    RR_OVIRTUAL void Abort() RR_OVERRIDE = 0;
    /** @copydoc Generator<void,Param>::AsyncAbort() */
    RR_OVIRTUAL void AsyncAbort(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                                int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        RR_UNUSED(timeout);
        RR_WEAK_PTR<RobotRaconteurNode> node;
        try
        {
            Abort();
        }
        catch (std::exception& exp)
        {
            detail::InvokeHandlerWithException(node, handler, exp);
            return;
        }
        detail::InvokeHandler(node, handler);
    }
    /** @copydoc Generator<void,Param>::Close() */
    RR_OVIRTUAL void Close() RR_OVERRIDE = 0;
    /** @copydoc Generator<void,Param>::AsyncClose() */
    RR_OVIRTUAL void AsyncClose(boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
                                int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE
    {
        RR_UNUSED(timeout);
        RR_WEAK_PTR<RobotRaconteurNode> node;
        try
        {
            Close();
        }
        catch (std::exception& exp)
        {
            detail::InvokeHandlerWithException(node, handler, exp);
            return;
        }
        detail::InvokeHandler(node, handler);
    }
    RR_OVIRTUAL ~SyncGenerator() RR_OVERRIDE {}
};

// NOLINTEND(cppcoreguidelines-virtual-class-destructor)

/**
 * @brief Utility class to use a C++ range with Generator<Return,void>.
 *
 * Examples of C++ ranges include std::vector<T> or std::list<T>
 *
 * Use CreateRangeGenerator() helper function to instantiate this type
 *
 * @tparam T The type contained in the range
 */
template <typename T>
class RangeGenerator : public SyncGenerator<typename T::value_type, void>
{
  protected:
    T range;
    typename T::iterator iter;
    boost::mutex range_lock;
    bool aborted;

  public:
    RangeGenerator(const T& range)
    {
        this->range = range;
        aborted = false;
        this->iter = this->range.begin();
    }

    /** @copydoc SyncGenerator<void,Param>::Next() */
    RR_OVIRTUAL typename T::value_type Next() RR_OVERRIDE
    {
        boost::mutex::scoped_lock lock(range_lock);

        if (aborted)
        {
            throw OperationAbortedException("");
        }

        if (iter == range.end())
        {
            throw StopIterationException("");
        }

        return *iter++;
    }
    /** @copydoc SyncGenerator<void,Param>::Abort() */
    RR_OVIRTUAL void Abort() RR_OVERRIDE
    {
        boost::mutex::scoped_lock lock(range_lock);
        iter = range.end();
        aborted = true;
    }
    /** @copydoc SyncGenerator<void,Param>::Close() */
    RR_OVIRTUAL void Close() RR_OVERRIDE
    {
        boost::mutex::scoped_lock lock(range_lock);
        iter = range.end();
    }
    RR_OVIRTUAL ~RangeGenerator() RR_OVERRIDE {}
};

/**
 * @brief Create a RangeGenerator from a range
 *
 * @tparam T The type contained in the range
 * @param range The range object to use with generator. Examples include std::vector<T> and std::list<T>
 * @return RR_SHARED_PTR<RangeGenerator<T> > The created RangeGenerator
 */
template <typename T>
RR_SHARED_PTR<RangeGenerator<T> > CreateRangeGenerator(const T& range)
{
    return RR_MAKE_SHARED<RangeGenerator<T> >(range);
}

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
template <typename Return, typename Param>
using GeneratorPtr = RR_SHARED_PTR<Generator<Return, Param> >;
template <typename Return, typename Param>
using GeneratorConstPtr = RR_SHARED_PTR<const Generator<Return, Param> >;
template <typename T>
using RangeGeneratorPtr = RR_SHARED_PTR<RangeGenerator<T> >;
template <typename T>
using RangeGeneratorConstPtr = RR_SHARED_PTR<const RangeGenerator<T> >;
#endif

} // namespace RobotRaconteur
