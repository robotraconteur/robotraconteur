/**
 * @file RobotRaconteurConfig.h
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

#ifndef ROBOTRACONTEUR_VERSION
// Boost Style Version Number
#define ROBOTRACONTEUR_VERSION 001504
#define ROBOTRACONTEUR_VERSION_TEXT "0.15.4"
#endif

#if (__GNUC__ == 4 && __GNUC_MINOR__ == 7)
#error GCC 4.7 is bug riddled and does not produce reliable executables.  Use GCC 4.6 or a newer version.
#endif

#ifndef RR_STD_SHARED_PTR
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/intrusive_ptr.hpp>

#define RR_SHARED_PTR boost::shared_ptr
#define RR_MAKE_SHARED boost::make_shared
#define RR_WEAK_PTR boost::weak_ptr
#define RR_ENABLE_SHARED_FROM_THIS boost::enable_shared_from_this
#define RR_DYNAMIC_POINTER_CAST boost::dynamic_pointer_cast
#define RR_STATIC_POINTER_CAST boost::static_pointer_cast

#else

#include <memory>

#define RR_SHARED_PTR std::shared_ptr
#define RR_MAKE_SHARED std::make_shared
#define RR_WEAK_PTR std::weak_ptr
#define RR_ENABLE_SHARED_FROM_THIS std::enable_shared_from_this
#define RR_DYNAMIC_POINTER_CAST std::dynamic_pointer_cast
#define RR_STATIC_POINTER_CAST std::static_pointer_cast

#endif

#define RR_INTRUSIVE_PTR boost::intrusive_ptr
#define RR_INTRUSIVE_PTR boost::intrusive_ptr

#define RR_UNORDERED_MAP boost::unordered_map

#include <boost/thread.hpp>
#include <boost/regex.hpp>

#ifdef BOOST_WINDOWS
#define ROBOTRACONTEUR_WINDOWS
#elif defined(__linux__)
#define ROBOTRACONTEUR_LINUX
#ifdef ANDROID
#define ROBOTRACONTEUR_ANDROID
#endif
#elif defined(__APPLE__)
#define ROBOTRACONTEUR_APPLE
#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
#define ROBOTRACONTEUR_IOS
#else
#define ROBOTRACONTEUR_OSX
#endif
#endif

#ifdef ROBOTRACONTEUR_WINDOWS
#define ROBOTRACONTEUR_PATHSEP "\\"
#else
#define ROBOTRACONTEUR_PATHSEP "/"
#endif

#ifdef ROBOTRACONTEUR_WINDOWS
#ifdef ROBOTRACONTEUR_CORE_EXPORTS
#define ROBOTRACONTEUR_CORE_API __declspec(dllexport)
#elif ROBOTRACONTEUR_CORE_IMPORTS
#define ROBOTRACONTEUR_CORE_API __declspec(dllimport)
#else
#define ROBOTRACONTEUR_CORE_API
#endif
#else
#define ROBOTRACONTEUR_CORE_API
#endif

// Small vector is relatively new
#if BOOST_VERSION > 105800
#define ROBOTRACONTEUR_USE_SMALL_VECTOR
#endif

// Use Boost ASIO move detection
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
#define RR_MOVE_ARG(type) type&&
#define RR_MOVE(x) std::move(x)
#else
#define RR_MOVE_ARG(type) type
#define RR_MOVE(x) x
#endif

#include <boost/asio/version.hpp>

#if BOOST_ASIO_VERSION < 101200
#define RR_BOOST_ASIO_IO_CONTEXT boost::asio::io_service
#define RR_BOOST_ASIO_STRAND boost::asio::io_service::strand
#define RR_BOOST_ASIO_POST(context, func) context.post(func)
#define RR_BOOST_ASIO_BUFFER_CAST(type, buf) boost::asio::buffer_cast<type>(buf)
#define RR_BOOST_ASIO_STRAND_WRAP(strand, f) (strand).wrap(f)
#define RR_BOOST_ASIO_NEW_STRAND(context) (new boost::asio::strand(context))
#else
#define RR_BOOST_ASIO_IO_CONTEXT boost::asio::io_context
#define RR_BOOST_ASIO_STRAND boost::asio::strand<boost::asio::io_context::executor_type>
#define RR_BOOST_ASIO_POST(context, func) boost::asio::post(context, func)
#define RR_BOOST_ASIO_BUFFER_CAST(type, buf) (type) buf.data()
#define RR_BOOST_ASIO_STRAND_WRAP(strand, f) boost::asio::bind_executor(strand, f)
#define RR_BOOST_ASIO_NEW_STRAND(context)                                                                              \
    (new boost::asio::strand<boost::asio::io_context::executor_type>(context.get_executor()))
#endif

#if BOOST_VERSION <= 105900
#define RR_BOOST_PLACEHOLDERS(arg) arg
#else
#define RR_BOOST_PLACEHOLDERS(arg) boost::placeholders::arg
#endif