// Copyright 2011-2019 Wason Technology, LLC
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
#include "RobotRaconteur/Service.h"

namespace RobotRaconteur
{
	template<typename T>
	class Callback : private boost::noncopyable
	{

	protected:
		std::string m_MemberName;

	public:
		Callback(boost::string_ref name)
		{
			m_MemberName = RR_MOVE(name.to_string());
		}

		virtual ~Callback() {}

		virtual T GetFunction() = 0;
		virtual void SetFunction(T value) = 0;

		virtual T GetClientFunction(RR_SHARED_PTR<Endpoint> e) = 0;

		virtual T GetClientFunction(uint32_t e) = 0;

		virtual std::string GetMemberName()
		{
			return m_MemberName;
		}

		virtual void Shutdown()
		{
			
		}


	};

	template<typename T>
	class CallbackClient : public Callback<T>
	{
	public:
		CallbackClient(boost::string_ref name) : Callback<T>(name)
		{
			InitializeInstanceFields();
		}

		virtual ~CallbackClient() {}


	private:
		T function;
		boost::mutex function_lock;
	public:
		virtual T GetFunction() 
		{
			boost::mutex::scoped_lock lock(function_lock);
			if (!function) throw InvalidOperationException("Callback function not set");
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

		virtual T GetClientFunction(uint32_t e)
		{
			throw InvalidOperationException("Invalid for client side of callback");
		}

		virtual void Shutdown()
		{
			boost::mutex::scoped_lock lock(function_lock);
			function.clear();
		}



	private:
		void InitializeInstanceFields()
		{
			function = T();
		}
	};

	class ROBOTRACONTEUR_CORE_API ServiceSkel;


	class ROBOTRACONTEUR_CORE_API CallbackServerBase
	{

	public:
		virtual ~CallbackServerBase() {}

	protected:
		RR_WEAK_PTR<ServiceSkel> skel;
		

		virtual RR_SHARED_PTR<void> GetClientFunction_internal(uint32_t e);

		virtual std::string GetMemberName()=0;
	};

	template<typename T>
	class CallbackServer : public Callback<T>, public CallbackServerBase
	{
	public:
		CallbackServer(boost::string_ref name, RR_SHARED_PTR<ServiceSkel> skel) : Callback<T>(name)
		{
			this->skel = skel;
		}

		virtual ~CallbackServer() {}

		virtual T GetFunction()
		{
			throw InvalidOperationException("Invalid for server side of callback");
		}
		virtual void SetFunction(T value)
		{
			throw InvalidOperationException("Invalid for server side of callback");
		}

		virtual T GetClientFunction(RR_SHARED_PTR<Endpoint> e)
		{
			return GetClientFunction(e->GetLocalEndpoint());
		}

		virtual T GetClientFunction(uint32_t e)
		{
			RR_SHARED_PTR<ServiceSkel> s=skel.lock();
			if (!s) throw InvalidOperationException("Callback server has been closed");
			return *RR_STATIC_POINTER_CAST<T>(s->GetCallbackFunction(e,GetMemberName()));
		}

	
		virtual std::string GetMemberName()
		{
			return Callback<T>::GetMemberName();
		}

		virtual void Shutdown()
		{
			
		}	

	};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
	template<typename T> using CallbackPtr = RR_SHARED_PTR<Callback<T> >;
	template<typename T> using CallbackConstPtr = RR_SHARED_PTR<const Callback<T> >;
#endif
}
