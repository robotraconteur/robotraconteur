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

#include <boost/asio.hpp>
#include "RobotRaconteur/DataTypes.h"

#pragma once

namespace RobotRaconteur
{
	class ROBOTRACONTEUR_CORE_API RobotRaconteurNode;

	class ROBOTRACONTEUR_CORE_API ThreadPool : public RR_ENABLE_SHARED_FROM_THIS<ThreadPool>, private boost::noncopyable
	{

	protected:
		std::vector<RR_SHARED_PTR<boost::thread> > threads;

		boost::mutex queue_mutex;

		boost::asio::io_service _io_service;

		size_t thread_count;

		bool keepgoing;
		boost::mutex keepgoing_lock;

		RR_SHARED_PTR<boost::asio::io_service::work> _work;

		RR_WEAK_PTR<RobotRaconteurNode> node;

	public:

		ThreadPool(RR_SHARED_PTR<RobotRaconteurNode> node);
        virtual ~ThreadPool();

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		virtual size_t GetThreadPoolCount();

		virtual void SetThreadPoolCount(size_t count);

		virtual void Post(boost::function<void()> function);
		virtual bool TryPost(RR_MOVE_ARG(boost::function<void()>) function);

		virtual void Shutdown();

		virtual boost::asio::io_service& get_io_service();


	protected:
		virtual void start_new_thread();

		virtual void thread_function();



	};

	class ROBOTRACONTEUR_CORE_API ThreadPoolFactory : private boost::noncopyable
	{
	public: 
		virtual RR_SHARED_PTR<ThreadPool> NewThreadPool(RR_SHARED_PTR<RobotRaconteurNode> node)
		{
			return RR_MAKE_SHARED<ThreadPool>(node);
		}
        virtual ~ThreadPoolFactory();
	};
#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
	using ThreadPoolPtr = RR_SHARED_PTR<ThreadPool>;
	using ThreadPoolFactoryPtr = RR_SHARED_PTR<ThreadPoolFactory>;
#endif
}
