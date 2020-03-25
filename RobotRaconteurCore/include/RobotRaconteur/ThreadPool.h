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

	class ROBOTRACONTEUR_CORE_API IOContextThreadPool : public ThreadPool
	{

	protected:
		
		RR_BOOST_ASIO_IO_CONTEXT& _external_io_context;
		bool _multithreaded;
		
	public:

		IOContextThreadPool(RR_SHARED_PTR<RobotRaconteurNode> node, RR_BOOST_ASIO_IO_CONTEXT& external_io_context, bool multithreaded);
        virtual ~IOContextThreadPool();
		
		virtual size_t GetThreadPoolCount();
		virtual void SetThreadPoolCount(size_t count);

		virtual void Post(boost::function<void()> function);
		virtual bool TryPost(RR_MOVE_ARG(boost::function<void()>) function);

		virtual void Shutdown();

		virtual RR_BOOST_ASIO_IO_CONTEXT& get_io_context();

	};

	namespace detail
	{
		bool ThreadPool_IsNodeMultithreaded(RR_WEAK_PTR<RobotRaconteurNode> node);
	}

	namespace detail
	{
		template<typename T>
		struct IOContextThreadPool_AsyncResultAdapter_traits
		{
			typedef T result_type;	
		};

		template<>
		struct IOContextThreadPool_AsyncResultAdapter_traits<void>
		{
			typedef int32_t result_type;	
		};

		template<typename T>
		struct IOContextThreadPool_AsyncResultAdapter_data
		{
			typedef typename IOContextThreadPool_AsyncResultAdapter_traits<T>::result_type result_type;
			boost::initialized<result_type> _result;
			RR_SHARED_PTR<RobotRaconteurException> _exp;
			boost::initialized<bool> _complete;
		};

		ROBOTRACONTEUR_CORE_API RR_SHARED_PTR<RobotRaconteurNode> IOContextThreadPool_RobotRaconteurNode_sp();

		ROBOTRACONTEUR_CORE_API void IOContextThreadPool_RobotRaconteurNode_DownCastAndThrowException(RR_SHARED_PTR<RobotRaconteurNode> node, RobotRaconteurException& exp);
	}

	template<typename T>
	class IOContextThreadPool_AsyncResultAdapter
	{
	private:
		RR_SHARED_PTR<RobotRaconteurNode> _node;
		RR_BOOST_ASIO_IO_CONTEXT& _io_context;
		RR_SHARED_PTR<detail::IOContextThreadPool_AsyncResultAdapter_data<T> > _data; 
	public:

		typedef typename detail::IOContextThreadPool_AsyncResultAdapter_traits<T>::result_type result_type;

		IOContextThreadPool_AsyncResultAdapter(RR_SHARED_PTR<RobotRaconteurNode>& node, RR_BOOST_ASIO_IO_CONTEXT& io_context)
			: _node(node), _io_context(io_context), _data(RR_MAKE_SHARED<detail::IOContextThreadPool_AsyncResultAdapter_data<T> >())  {}

		IOContextThreadPool_AsyncResultAdapter(RR_BOOST_ASIO_IO_CONTEXT& io_context)
			: _node(detail::IOContextThreadPool_RobotRaconteurNode_sp()), _io_context(io_context), _data(RR_MAKE_SHARED<detail::IOContextThreadPool_AsyncResultAdapter_data<T> >()) {}

		void operator () (result_type res, RR_SHARED_PTR<RobotRaconteurException> exp)
		{
			_data->_complete.data() = true;
			_data->_result.data() = res;
			_data->_exp = exp;
		}

		void operator () (RR_SHARED_PTR<RobotRaconteurException> exp)
		{
			_data->_complete.data() = true;
			_data->_exp = exp;
		}

		void operator () (result_type res)
		{
			_data->_complete.data() = true;
			_data->_result.data() = res;			
		}

		void operator () ()
		{
			_data->_complete.data() = true;			
		}

		result_type GetResult()
		{
			while (!_data->_complete.data())
			{
				_io_context.run_one();
			}

			result_type res;
			RR_SHARED_PTR<RobotRaconteurException> exp;
			boost::swap(res, _data->_result.data());
			exp = _data->_exp;
			_data->_exp.reset();
			if (exp)
			{
				RobotRaconteurException* exp1 = exp.get();
				detail::IOContextThreadPool_RobotRaconteurNode_DownCastAndThrowException(_node,*exp1);
			}			
			return res;
		}

		bool PollResult(result_type& ret, RR_SHARED_PTR<RobotRaconteurException>& exp)
		{
			if(!_data->_complete.data())
			{
				return false;
			}

			boost::swap(ret,_data->_result.data());
			boost::swap(exp,_data->_exp);
			return true;
		}
	};

#define ROBOTRACONTEUR_ASSERT_MULTITHREADED(node) BOOST_ASSERT_MSG(detail::ThreadPool_IsNodeMultithreaded(node)  ,"multithreading required for requested operation")

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
	using ThreadPoolPtr = RR_SHARED_PTR<ThreadPool>;
	using ThreadPoolFactoryPtr = RR_SHARED_PTR<ThreadPoolFactory>;
	using IOContextThreadPoolPtr = RR_SHARED_PTR<IOContextThreadPool>;	
#endif
}
