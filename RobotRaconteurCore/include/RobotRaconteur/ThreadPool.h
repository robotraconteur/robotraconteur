/** 
 * @file ThreadPool.h
 * 
 * @author Dr. John Wason
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

		RR_BOOST_ASIO_IO_CONTEXT _io_context;

		size_t thread_count;

		bool keepgoing;
		boost::mutex keepgoing_lock;

#if BOOST_ASIO_VERSION < 101200
		RR_SHARED_PTR<RR_BOOST_ASIO_IO_CONTEXT::work> _work;
#else
		RR_SHARED_PTR <boost::asio::executor_work_guard<RR_BOOST_ASIO_IO_CONTEXT::executor_type> > _work;
#endif

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

		virtual RR_BOOST_ASIO_IO_CONTEXT& get_io_context();


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
