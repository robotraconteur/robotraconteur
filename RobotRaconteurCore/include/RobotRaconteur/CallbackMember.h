/**
 * @file CallbackMember.h
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
#include "RobotRaconteur/Service.h"

namespace RobotRaconteur
{
/**
 * @brief `callback` member type interface
 *
 * The Callback class implements the `callback` member type. Callbacks are declared in service definition
 * files using the `callback` keyword within object declarations. They provide functionality similar to the
 * `function` member, but the direction is reversed, allowing the service to call a function on a specified
 * client. The desired client is specified using the Robot Raconteur endpoint identifier. Clients must
 * configure the callback to use using SetFunction().
 *
 * On the client side,
 * the client specifies a function for the callback using the SetFunction() function.
 * On the service side, the function GetFunction(RR_SHARED_PTR<Endpoint> endpoint) is used to retrieve
 * the proxy function to call a client callback.
 *
 * This class is instantiated by the node. It should not be instantiated by the user.
 *
 * @tparam T The type of the callback function. This is determined by the thunk source generator.
 */
template <typename T>
class Callback : private boost::noncopyable
{

  protected:
    std::string m_MemberName;

  public:
    Callback(boost::string_ref name) { m_MemberName = RR_MOVE(name.to_string()); }

    virtual ~Callback() {}

    /**
     * @brief Get the currently configured callback function on client side
     *
     * @return T The currently configured function
     */
    virtual T GetFunction() = 0;

    /**
     * @brief Set the callback function reference on the client side
     *
     * The callback function set will be made available to be called by
     * the service using a function proxy.
     *
     * Use lambda function or boost::bind to create the callback function reference
     *
     * @param value The callback function.
     */
    virtual void SetFunction(T value) = 0;

    /**
     * @brief Get the proxy function to call the callback for the specified client on
     * the service side
     *
     * This function returns a proxy to the callback on a specified client. The proxy
     * operates as a reverse function, sending parameters, executing the callback,
     * and receiving the results.
     *
     * Because services can have multiple clients, it is necessary to specify which client
     * to call. This is done by passing the endpoint of the client connection to the
     * endpoint parameter.
     *
     * The endpoint of a client can be determined using the ServerEndpoint::CurrentEndpoint()
     * function during a `function` or `property` member call. The service can store this
     * value, and use it to retrieve the callback proxy.
     *
     * @param endpoint The endpoint of the client connection
     * @return T The callback proxy function
     */
    virtual T GetClientFunction(RR_SHARED_PTR<Endpoint> endpoint) = 0;

    /**
     * @brief Get the proxy function to call the callback for the specified client
     * on the service side
     *
     * Same as GetClientFunction(RR_SHARED_PTR<Endpoint> endpoint), except the endpoint
     * is specified using the uint32_t LocalEndpoint id instead of the Endpoint
     * object.
     *
     * @param endpoint The uint32_t LocalEndpoint id
     * @return T The callback proxy function
     */
    virtual T GetClientFunction(uint32_t endpoint) = 0;

    /**
     * @brief Get the member name of the callback
     *
     * @return std::string
     */
    virtual std::string GetMemberName() { return m_MemberName; }

    virtual void Shutdown() {}
};

template <typename T>
class CallbackClient : public Callback<T>
{
  public:
    CallbackClient(boost::string_ref name) : Callback<T>(name) { InitializeInstanceFields(); }

    virtual ~CallbackClient() {}

  private:
    T function;
    boost::mutex function_lock;

  public:
    virtual T GetFunction()
    {
        boost::mutex::scoped_lock lock(function_lock);
        if (!function)
            throw InvalidOperationException("Callback function not set");
        return function;
    }
    virtual void SetFunction(T value)
    {
        boost::mutex::scoped_lock lock(function_lock);
        function = value;
    }

    virtual T GetClientFunction(RR_SHARED_PTR<Endpoint> e)
    {
        throw InvalidOperationException("Invalid for client side of callback");
    }

    virtual T GetClientFunction(uint32_t e) { throw InvalidOperationException("Invalid for client side of callback"); }

    virtual void Shutdown()
    {
        boost::mutex::scoped_lock lock(function_lock);
        function.clear();
    }

  private:
    void InitializeInstanceFields() { function = T(); }
};

class ROBOTRACONTEUR_CORE_API ServiceSkel;

class ROBOTRACONTEUR_CORE_API CallbackServerBase
{

  public:
    virtual ~CallbackServerBase() {}

  protected:
    RR_WEAK_PTR<ServiceSkel> skel;

    virtual RR_SHARED_PTR<void> GetClientFunction_internal(uint32_t e);

    virtual std::string GetMemberName() = 0;
};

template <typename T>
class CallbackServer : public Callback<T>, public CallbackServerBase
{
  public:
    CallbackServer(boost::string_ref name, RR_SHARED_PTR<ServiceSkel> skel) : Callback<T>(name) { this->skel = skel; }

    virtual ~CallbackServer() {}

    virtual T GetFunction() { throw InvalidOperationException("Invalid for server side of callback"); }
    virtual void SetFunction(T value) { throw InvalidOperationException("Invalid for server side of callback"); }

    virtual T GetClientFunction(RR_SHARED_PTR<Endpoint> e) { return GetClientFunction(e->GetLocalEndpoint()); }

    virtual T GetClientFunction(uint32_t e)
    {
        RR_SHARED_PTR<ServiceSkel> s = skel.lock();
        if (!s)
            throw InvalidOperationException("Callback server has been closed");
        return *RR_STATIC_POINTER_CAST<T>(s->GetCallbackFunction(e, GetMemberName()));
    }

    virtual std::string GetMemberName() { return Callback<T>::GetMemberName(); }

    virtual void Shutdown() {}
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
/** @brief Convenience alias for Callback shared_ptr */
template <typename T>
using CallbackPtr = RR_SHARED_PTR<Callback<T> >;
/** @brief Convenience alias for Callback const shared_ptr */
template <typename T>
using CallbackConstPtr = RR_SHARED_PTR<const Callback<T> >;
#endif
} // namespace RobotRaconteur
