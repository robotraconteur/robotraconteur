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

#pragma once

#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/Endpoint.h"
#include "RobotRaconteur/Client.h"
#include "RobotRaconteur/Service.h"

namespace RobotRaconteur
{
	template <typename Return, typename Param>
	class Generator : private boost::noncopyable
	{
	public:
		virtual Return Next(const Param& v) = 0;
		virtual void AsyncNext(const Param& v, boost::function<void(const Return, RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
		virtual void Abort() = 0;
		virtual void AsyncAbort(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
		virtual void Close() = 0;
		virtual void AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
		virtual ~Generator() {}
	};

	template <typename Return>
	class Generator<Return, void> : private boost::noncopyable
	{
	public:
		virtual Return Next() = 0;
		virtual void AsyncNext(boost::function<void(const Return, RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
		virtual void Abort() = 0;
		virtual void AsyncAbort(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
		virtual void Close() = 0;
		virtual void AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
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
			catch (StopIterationException&) {}
			return ret;
		}
		virtual ~Generator() {}
	};

	template <typename Param>
	class Generator<void, Param> : private boost::noncopyable
	{
	public:
		virtual void Next(const Param& v) = 0;
		virtual void AsyncNext(const Param& v, boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
		virtual void Abort() = 0;
		virtual void AsyncAbort(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
		virtual void Close() = 0;
		virtual void AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
		virtual ~Generator() {}
	};

	class ROBOTRACONTEUR_CORE_API GeneratorClientBase
	{
	protected:

		std::string name;
		int32_t id;
		RR_WEAK_PTR<ServiceStub> stub;

		GeneratorClientBase(const std::string& name, int32_t id, RR_SHARED_PTR<ServiceStub> stub);

		virtual RR_INTRUSIVE_PTR<MessageElement> NextBase(RR_INTRUSIVE_PTR<MessageElement> v);
		virtual void AsyncNextBase(RR_INTRUSIVE_PTR<MessageElement> v, boost::function<void(RR_INTRUSIVE_PTR<MessageElement>, RR_SHARED_PTR<RobotRaconteurException>, RR_SHARED_PTR<RobotRaconteurNode>)> handler, int32_t timeout);
		
		static void AsyncNextBase1(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_INTRUSIVE_PTR<MessageElement>, RR_SHARED_PTR<RobotRaconteurException>, RR_SHARED_PTR<RobotRaconteurNode>)> handler, RR_WEAK_PTR<RobotRaconteurNode> node);

	public:
		RR_SHARED_PTR <ServiceStub> GetStub();	
		
		virtual void Abort();
		virtual void AsyncAbort(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE);
		virtual void Close();
		virtual void AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE);
		std::string GetMemberName();

		virtual ~GeneratorClientBase() {}
	};

	namespace detail
	{
		template <typename Return>
		static void GeneratorClient_AsyncNext1(RR_INTRUSIVE_PTR<MessageElement> v2, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<RobotRaconteurNode> node, boost::function<void(Return, RR_SHARED_PTR<RobotRaconteurException> err)> handler)
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

		ROBOTRACONTEUR_CORE_API void GeneratorClient_AsyncNext2(RR_INTRUSIVE_PTR<MessageElement> v2, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<RobotRaconteurNode> node, boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler);
	}

	template <typename Return, typename Param>
	class GeneratorClient : public Generator<Return,Param>, public GeneratorClientBase
	{
	public:
		GeneratorClient(const std::string& name, int32_t id, RR_SHARED_PTR<ServiceStub> stub)
			: GeneratorClientBase(name, id, stub)
		{
		}

		virtual Return Next(const Param& v)
		{
			RR_INTRUSIVE_PTR<MessageElement> v1 = CreateMessageElement("",GetStub()->RRGetNode()->template PackAnyType<typename RRPrimUtil<Param>::BoxedType>(RRPrimUtil<Param>::PrePack(v)));
			RR_INTRUSIVE_PTR<MessageElement> v2 = NextBase(v1);
			return RRPrimUtil<Return>::PreUnpack(GetStub()->RRGetNode()->template UnpackAnyType<typename RRPrimUtil<Return>::BoxedType>(v2));
		}
		virtual void AsyncNext(const Param& v, boost::function<void(Return, RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			RR_INTRUSIVE_PTR<MessageElement> v1 = CreateMessageElement("",GetStub()->RRGetNode()->template PackAnyType<typename RRPrimUtil<Param>::BoxedType>(RRPrimUtil<Param>::PrePack(v)));
			AsyncNextBase(v1, boost::bind<void>(&detail::GeneratorClient_AsyncNext1<Return>, _1, _2, _3, handler),timeout);
		}		
		virtual void Abort()
		{
			GeneratorClientBase::Abort();
		}
		virtual void AsyncAbort(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			GeneratorClientBase::AsyncAbort(handler, timeout);
		}
		virtual void Close()
		{
			GeneratorClientBase::Close();
		}
		virtual void AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			GeneratorClientBase::AsyncAbort(handler, timeout);
		}
	};
	
	template <typename Return>
	class GeneratorClient<Return,void> : public Generator<Return, void>, public GeneratorClientBase
	{
	public:
		GeneratorClient(const std::string& name, int32_t id, RR_SHARED_PTR<ServiceStub> stub)
			: GeneratorClientBase(name, id, stub)
		{
		}

		virtual Return Next()
		{			
			RR_INTRUSIVE_PTR<MessageElement> v2 = NextBase(RR_INTRUSIVE_PTR<MessageElement>());
			return RRPrimUtil<Return>::PreUnpack(GetStub()->RRGetNode()->template UnpackAnyType<typename RRPrimUtil<Return>::BoxedType>(v2));
		}
		virtual void AsyncNext(boost::function<void(Return, RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{			
			AsyncNextBase(RR_INTRUSIVE_PTR<MessageElement>(), boost::bind<void>(&detail::GeneratorClient_AsyncNext1<Return>, _1, _2, _3, handler), timeout);
		}		
		virtual void Abort()
		{
			GeneratorClientBase::Abort();
		}
		virtual void AsyncAbort(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			GeneratorClientBase::AsyncAbort(handler, timeout);
		}
		virtual void Close()
		{
			GeneratorClientBase::Close();
		}
		virtual void AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			GeneratorClientBase::AsyncAbort(handler, timeout);
		}
	};

	template <typename Param>
	class GeneratorClient<void,Param> : public Generator<void, Param>, public GeneratorClientBase
	{
	public:
		GeneratorClient(const std::string& name, int32_t id, RR_SHARED_PTR<ServiceStub> stub)
			: GeneratorClientBase(name, id, stub)
		{
		}

		virtual void Next(const Param& v)
		{
			RR_INTRUSIVE_PTR<MessageElement> v1 = CreateMessageElement("", GetStub()->RRGetNode()->template PackAnyType<typename RRPrimUtil<Param>::BoxedType>(RRPrimUtil<Param>::PrePack(v)));
			NextBase(v1);			
		}
		virtual void AsyncNext(const Param& v, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			RR_INTRUSIVE_PTR<MessageElement> v1 = CreateMessageElement("", GetStub()->RRGetNode()->template PackAnyType<typename RRPrimUtil<Param>::BoxedType>(RRPrimUtil<Param>::PrePack(v)));
			AsyncNextBase(v1, boost::bind<void>(&detail::GeneratorClient_AsyncNext2, _1, _2, _3, handler), timeout);
		}
		virtual void Abort()
		{
			GeneratorClientBase::Abort();
		}
		virtual void AsyncAbort(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			GeneratorClientBase::AsyncAbort(handler, timeout);
		}
		virtual void Close()
		{
			GeneratorClientBase::Close();
		}
		virtual void AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
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

		GeneratorServerBase(const std::string& name, int32_t index, RR_SHARED_PTR<ServiceSkel> skel, RR_SHARED_PTR<ServerEndpoint> ep);
		
	public:
		virtual void CallNext(RR_INTRUSIVE_PTR<MessageEntry> m) = 0;

		virtual uint32_t GetEndpoint();

		virtual ~GeneratorServerBase() {}

	protected:

		static void EndAsyncCallNext(RR_WEAK_PTR<ServiceSkel> skel, RR_INTRUSIVE_PTR<MessageElement> ret, RR_SHARED_PTR<RobotRaconteurException> err, int32_t index, RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep);
		
		template<typename Return>
		static void CallNext1(Return v2, RR_SHARED_PTR<RobotRaconteurException> err, int32_t index, RR_WEAK_PTR<ServiceSkel> skel, RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep)
		{
			RR_SHARED_PTR<ServiceSkel> skel1 = skel.lock();
			//TODO: Improve null handling
			if (!skel1 && !err)
			{
				err = RR_MAKE_SHARED<InvalidOperationException>("Service skel released");
			}
			if (err)
			{
				GeneratorServerBase::EndAsyncCallNext(skel, RR_INTRUSIVE_PTR<MessageElement>(), err, index, m, ep);
			}

			RR_INTRUSIVE_PTR<MessageElement> v3 = CreateMessageElement("", skel1->RRGetNode()->template PackAnyType<typename RRPrimUtil<Return>::BoxedType>(RRPrimUtil<Return>::PrePack(v2)));
			GeneratorServerBase::EndAsyncCallNext(skel, v3, err, index, m, ep);
		}	

		static void CallNext2(RR_SHARED_PTR<RobotRaconteurException> err, int32_t index, RR_WEAK_PTR<ServiceSkel> skel, RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep)
		{
			RR_SHARED_PTR<ServiceSkel> skel1 = skel.lock();
			//TODO: Improve null handling
			if (!skel1 && !err)
			{
				err = RR_MAKE_SHARED<InvalidOperationException>("Service skel released");
			}
			if (err)
			{
				GeneratorServerBase::EndAsyncCallNext(skel, RR_INTRUSIVE_PTR<MessageElement>(), err, index, m, ep);
			}

			RR_INTRUSIVE_PTR<MessageElement> v3;
			GeneratorServerBase::EndAsyncCallNext(skel, v3, err, index, m, ep);
		}
	};

	
	template <typename Return, typename Param>
	class GeneratorServer : public GeneratorServerBase
	{
	protected:
		RR_SHARED_PTR<Generator<Return, Param> > generator;
	
	public:

		GeneratorServer(RR_SHARED_PTR<Generator<Return, Param> > generator, const std::string& name, int32_t id, RR_SHARED_PTR<ServiceSkel> skel, RR_SHARED_PTR<ServerEndpoint> ep)
			: GeneratorServerBase(name, id, skel, ep)
		{
			if (!generator) throw InvalidOperationException("Generator must not be null");
			this->generator = generator;
		}	

		virtual void CallNext(RR_INTRUSIVE_PTR<MessageEntry> m)
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
					generator->AsyncClose(boost::bind<void>(&GeneratorServerBase::CallNext1<int32_t>, 0, _1, index, skel, m, ep));
				}
				else
				{
					generator->AsyncAbort(boost::bind<void>(&GeneratorServerBase::CallNext1<int32_t>, 0, _1, index, skel, m, ep));
				}
				
			}
			else
			{
				Param v = RRPrimUtil<Param>::PreUnpack(skel1->RRGetNode()->template UnpackAnyType<typename RRPrimUtil<Param>::BoxedType>(m->FindElement("parameter")));
				generator->AsyncNext(v, boost::bind<void>(&GeneratorServerBase::CallNext1<Return>, _1, _2, index, skel, m, ep));
			}
		}

		virtual ~GeneratorServer() {}
	};

	template <typename Return>
	class GeneratorServer<Return, void> : public GeneratorServerBase
	{
	protected:
		RR_SHARED_PTR<Generator<Return, void> > generator;

	public:

		GeneratorServer(RR_SHARED_PTR<Generator<Return, void> > generator, const std::string& name, int32_t id, RR_SHARED_PTR<ServiceSkel> skel, RR_SHARED_PTR<ServerEndpoint> ep)
			: GeneratorServerBase(name, id, skel, ep)
		{
			if (!generator) throw InvalidOperationException("Generator must not be null");
			this->generator = generator;
		}

		virtual void CallNext(RR_INTRUSIVE_PTR<MessageEntry> m)
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
					generator->AsyncClose(boost::bind<void>(&GeneratorServerBase::CallNext1<int32_t>, 0, _1, index, skel, m, ep));
				}
				else
				{
					generator->AsyncAbort(boost::bind<void>(&GeneratorServerBase::CallNext1<int32_t>, 0, _1, index, skel, m, ep));
				}
			}
			else
			{				
				generator->AsyncNext(boost::bind<void>(&GeneratorServerBase::CallNext1<Return>, _1, _2, index, skel, m, ep));
			}
		}
		virtual ~GeneratorServer() {}
	};

	template <typename Param>
	class GeneratorServer<void, Param> : public GeneratorServerBase
	{
	protected:
		RR_SHARED_PTR<Generator<void, Param> > generator;

	public:

		GeneratorServer(RR_SHARED_PTR<Generator<void, Param> > generator, const std::string& name, int32_t id, RR_SHARED_PTR<ServiceSkel> skel, RR_SHARED_PTR<ServerEndpoint> ep)
			: GeneratorServerBase(name, id, skel, ep)
		{
			if (!generator) throw InvalidOperationException("Generator must not be null");
			this->generator = generator;
		}

		virtual void CallNext(RR_INTRUSIVE_PTR<MessageEntry> m)
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
					generator->AsyncClose(boost::bind<void>(&GeneratorServerBase::CallNext1<int32_t>, 0, _1, index, skel, m, ep));
				}
				else
				{
					generator->AsyncAbort(boost::bind<void>(&GeneratorServerBase::CallNext1<int32_t>, 0, _1, index, skel, m, ep));
				}
			}
			else
			{
				Param v = RRPrimUtil<Param>::PreUnpack(skel1->RRGetNode()->template UnpackAnyType<typename RRPrimUtil<Param>::BoxedType>(m->FindElement("parameter")));
				generator->AsyncNext(v, boost::bind<void>(&GeneratorServerBase::CallNext2, _1, index, skel, m, ep));
			}
		}
		virtual ~GeneratorServer() {}
	};

	template <typename Return, typename Param>
	class SyncGenerator : public Generator<Return, Param>
	{
	public:
		virtual Return Next(const Param& v) = 0;
		virtual void AsyncNext(const Param& v, boost::function<void(const Return, RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
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
		virtual void Abort() = 0;
		virtual void AsyncAbort(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{			
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
		virtual void Close() = 0;
		virtual void AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
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
		virtual ~SyncGenerator() {}
	};

	template <typename Return>
	class SyncGenerator<Return,void> : public Generator<Return, void>
	{
	public:
		virtual Return Next() = 0;
		virtual void AsyncNext(boost::function<void(const Return, RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
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
		virtual void Abort() = 0;
		virtual void AsyncAbort(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
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
		virtual void Close() = 0;
		virtual void AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
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
		virtual ~SyncGenerator() {}
	};

	template <typename Param>
	class SyncGenerator<void,Param> : public Generator<void, Param>
	{
	public:
		virtual void Next(const Param& v) = 0;
		virtual void AsyncNext(const Param& v, boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{			
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
		virtual void Abort() = 0;
		virtual void AsyncAbort(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
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
		virtual void Close() = 0;
		virtual void AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
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
		virtual ~SyncGenerator() {}
	};

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
			this->iter = this->range.begin();
			this->aborted = false;
		}

		virtual typename T::value_type Next()
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
		virtual void Abort()
		{
			boost::mutex::scoped_lock lock(range_lock);
			iter = range.end();
			aborted = true;
		}
		virtual void Close()
		{
			boost::mutex::scoped_lock lock(range_lock);
			iter = range.end();			
		}
		virtual ~RangeGenerator() {}
	};

	template<typename T>
	RR_SHARED_PTR<RangeGenerator<T> > CreateRangeGenerator(const T& range)
	{
		return RR_MAKE_SHARED<RangeGenerator<T> >(range);
	}

}
