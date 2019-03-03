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
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/AsyncUtils.h"
#include <boost/call_traits.hpp>


#pragma warning (push)
#pragma warning(disable: 4250)
#pragma warning(disable: 4996)
#include <boost/signals2.hpp>

namespace RobotRaconteur
{
	class ROBOTRACONTEUR_CORE_API  WireBase;
	class ROBOTRACONTEUR_CORE_API  WireConnectionBase;
	class ROBOTRACONTEUR_CORE_API WireConnectionBaseListener;
	namespace detail { class WireSubscription_connection;  }

	class ROBOTRACONTEUR_CORE_API  WireConnectionBase : public RR_ENABLE_SHARED_FROM_THIS<WireConnectionBase>, private boost::noncopyable
	{

		friend class WireBase;
		friend class WireClientBase;
		friend class WireServerBase;
		friend class WireBroadcasterBase;
		friend class WireSubscriptionBase;
		friend class detail::WireSubscription_connection;

	public:
		virtual uint32_t GetEndpoint();
		
		virtual TimeSpec GetLastValueReceivedTime();

		virtual TimeSpec GetLastValueSentTime();
		
		virtual void Close();

		virtual void AsyncClose(RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout);

		WireConnectionBase(RR_SHARED_PTR<WireBase> parent, uint32_t endpoint = 0, MemberDefinition_Direction direction = MemberDefinition_Direction_both, bool message3 = false);

		virtual ~WireConnectionBase() {}

		virtual void WirePacketReceived(TimeSpec timespec, RR_INTRUSIVE_PTR<RRValue> packet);

		virtual bool GetInValueValid();

		virtual bool GetOutValueValid();

		bool WaitInValueValid(int32_t timeout = RR_TIMEOUT_INFINITE);
		bool WaitOutValueValid(int32_t timeout = RR_TIMEOUT_INFINITE);

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		virtual bool GetIgnoreInValue();
		virtual void SetIgnoreInValue(bool ignore);
				
		virtual void AddListener(RR_SHARED_PTR<WireConnectionBaseListener> listener);

		MemberDefinition_Direction Direction();

	protected:

		virtual void RemoteClose();

		RR_INTRUSIVE_PTR<RRValue> inval;
		RR_INTRUSIVE_PTR<RRValue> outval;

		bool inval_valid;
		TimeSpec lasttime_send;

		bool  outval_valid;
		TimeSpec lasttime_recv;

		boost::condition_variable inval_wait;
		boost::condition_variable outval_wait;

		uint32_t endpoint;
		RR_WEAK_PTR<WireBase> parent;

		boost::mutex sendlock;
		boost::mutex recvlock;

		bool send_closed;
		bool recv_closed;

		RR_INTRUSIVE_PTR<RRValue> GetInValueBase();

		RR_INTRUSIVE_PTR<RRValue> GetOutValueBase();

		void SetOutValueBase(RR_INTRUSIVE_PTR<RRValue> value);
		
		bool TryGetInValueBase(RR_INTRUSIVE_PTR<RRValue>& value, TimeSpec& time);
		bool TryGetOutValueBase(RR_INTRUSIVE_PTR<RRValue>& value, TimeSpec& time);
		
		virtual void fire_WireValueChanged(RR_INTRUSIVE_PTR<RRValue> value, TimeSpec time)=0;
	
		virtual void fire_WireClosedCallback()=0;

		void Shutdown();
	

		RR_SHARED_PTR<WireBase> GetParent();

		boost::mutex inval_lock;
		boost::mutex outval_lock;

		bool ignore_inval;

		bool message3;
		
		boost::mutex listeners_lock;
		std::list<RR_WEAK_PTR<WireConnectionBaseListener> > listeners;

		detail::async_signal_semaphore wire_value_changed_semaphore;

		RR_WEAK_PTR<RobotRaconteurNode> node;

		MemberDefinition_Direction direction;
		
	};

	template <typename T>
	class WireConnection : public WireConnectionBase
	{
	private:
		boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> WireConnectionClosedCallback;
		boost::mutex WireConnectionClosedCallback_lock;

	public:
		boost::signals2::signal<void (RR_SHARED_PTR<WireConnection<T> > connection, T value, TimeSpec time)> WireValueChanged;
		boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> GetWireConnectionClosedCallback()
		{
			boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
			return WireConnectionClosedCallback;
		}

		void SetWireConnectionClosedCallback(boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> callback)
		{
			boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
			WireConnectionClosedCallback=callback;
		}

		virtual ~WireConnection() {}

		virtual T GetInValue()
		{
			return RRPrimUtil<T>::PreUnpack(GetInValueBase());
		}

		virtual T GetOutValue()
		{
			return RRPrimUtil<T>::PreUnpack(GetOutValueBase());
		}

		virtual void SetOutValue(typename boost::call_traits<T>::param_type value)
		{
			SetOutValueBase(RRPrimUtil<T>::PrePack(value));
		}

		bool TryGetInValue(T& value, TimeSpec& time)
		{
			RR_INTRUSIVE_PTR<RRValue> o;
			if (!TryGetInValueBase(o, time)) return false;
			value = RRPrimUtil<T>::PreUnpack(o);
			return true;
		}

		bool TryGetOutValue(T& value, TimeSpec& time)
		{
			RR_INTRUSIVE_PTR<RRValue> o;
			if (!TryGetOutValueBase(o, time)) return false;
			value = RRPrimUtil<T>::PreUnpack(o);
			return true;
		}

		WireConnection(RR_SHARED_PTR<WireBase> parent, uint32_t endpoint=0, MemberDefinition_Direction direction = MemberDefinition_Direction_both, bool message3=false)
			: WireConnectionBase(parent,endpoint,direction,message3) {}

	protected:
		virtual void fire_WireValueChanged(RR_INTRUSIVE_PTR<RRValue> value, TimeSpec time)
		{
			WireValueChanged(RR_STATIC_POINTER_CAST<WireConnection<T> >(shared_from_this()),RRPrimUtil<T>::PreUnpack(value),time);
		}
	
		virtual void fire_WireClosedCallback()
		{
			boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> c=GetWireConnectionClosedCallback();
			if (!c) return;
			c(RR_STATIC_POINTER_CAST<WireConnection<T> >(shared_from_this()));
		}

	public:
		virtual void Close()
		{
			WireConnectionBase::Close();
			{
				boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
				WireConnectionClosedCallback.clear();
			}
			WireValueChanged.disconnect_all_slots();
		}

	protected:

		virtual void AsyncClose1(RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& handler)
		{
			try
			{
				{
					boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
					WireConnectionClosedCallback.clear();
				}
				WireValueChanged.disconnect_all_slots();
			}
			catch (std::exception&) {}

			
			handler(err);
		}

	public:
		virtual void AsyncClose(RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout=2000)
		{
			WireConnectionBase::AsyncClose(boost::bind(&WireConnection<T>::AsyncClose1,RR_STATIC_POINTER_CAST<WireConnection<T> >(shared_from_this()),_1,handler),timeout);
			
		}

	protected:
		virtual void RemoteClose()
		{
			WireConnectionBase::RemoteClose();
			{
				boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
				WireConnectionClosedCallback.clear();
			}
			WireValueChanged.disconnect_all_slots();
		}


	};


	class ROBOTRACONTEUR_CORE_API  WireBase : public RR_ENABLE_SHARED_FROM_THIS<WireBase>, private boost::noncopyable
	{
		

	public:

		friend class WireConnectionBase;
		
		virtual ~WireBase() {}

		virtual std::string GetMemberName()=0;

		virtual void WirePacketReceived(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e=0)=0;

		virtual void Shutdown()=0;


		

		virtual void AsyncClose(RR_SHARED_PTR<WireConnectionBase> endpoint, bool remote, uint32_t ee, RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)=0;

	protected:

		virtual void SendWirePacket(RR_INTRUSIVE_PTR<RRValue> data, TimeSpec time, uint32_t endpoint, bool message3)=0;

		bool rawelements;		

		void DispatchPacket (RR_INTRUSIVE_PTR<MessageEntry> me, RR_SHARED_PTR<WireConnectionBase> e);

		RR_INTRUSIVE_PTR<RRValue> UnpackPacket(RR_INTRUSIVE_PTR<MessageEntry> me, TimeSpec& ts);

		RR_INTRUSIVE_PTR<MessageEntry> PackPacket(RR_INTRUSIVE_PTR<RRValue> data, TimeSpec time, bool message3);

		virtual RR_INTRUSIVE_PTR<MessageElementData> PackData(RR_INTRUSIVE_PTR<RRValue> data)
		{
			return GetNode()->PackVarType(data);
		}

		virtual RR_INTRUSIVE_PTR<RRValue> UnpackData(RR_INTRUSIVE_PTR<MessageElement> mdata)
		{
			return GetNode()->UnpackVarType(mdata);
		}

		
		RR_WEAK_PTR<RobotRaconteurNode> node;

		MemberDefinition_Direction direction;

	public:

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		MemberDefinition_Direction Direction();	

	};


	template <typename T>
	class Wire : public virtual WireBase
	{

		friend class WireConnectionBase;

	public:

		Wire(boost::function<void(RR_INTRUSIVE_PTR<RRValue>&)> verify)
		{
			this->verify = verify;
		}

		virtual ~Wire() {}
		
		//Client side functions
		virtual RR_SHARED_PTR<WireConnection<T> > Connect() = 0;
		virtual void AsyncConnect(RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<WireConnection<T> >, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;

		virtual T PeekInValue(TimeSpec& ts) = 0;
		virtual T PeekOutValue(TimeSpec& ts) = 0;
		virtual void PokeOutValue(const T& value) = 0;
		virtual void AsyncPeekInValue(RR_MOVE_ARG(boost::function<void(const T&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
		virtual void AsyncPeekOutValue(RR_MOVE_ARG(boost::function<void(const T&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
		virtual void AsyncPokeOutValue(const T& value, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
		
		//Server side functions
		virtual boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> GetWireConnectCallback()=0;		
		virtual void SetWireConnectCallback(boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> function)=0;
		
		virtual boost::function<T(const uint32_t&)> GetPeekInValueCallback() = 0;
		virtual void SetPeekInValueCallback(boost::function<T(const uint32_t&)> function) = 0;
		virtual boost::function<T(const uint32_t&)> GetPeekOutValueCallback() = 0;
		virtual void SetPeekOutValueCallback(boost::function<T(const uint32_t&)> function) = 0;
		virtual boost::function<void(const T&, const TimeSpec&, const uint32_t&)> GetPokeOutValueCallback() = 0;
		virtual void SetPokeOutValueCallback(boost::function<void(const T&, const TimeSpec&, const uint32_t&)> function) = 0;
		
		
	protected:

		virtual RR_INTRUSIVE_PTR<MessageElementData> PackData(RR_INTRUSIVE_PTR<RRValue> data)
		{
			if (verify)
			{
				verify(data);
			}
			return GetNode()->template PackAnyType<typename RRPrimUtil<T>::BoxedType>(data);
		}

		virtual RR_INTRUSIVE_PTR<RRValue> UnpackData(RR_INTRUSIVE_PTR<MessageElement> mdata)
		{
			if (!verify)
			{
				return GetNode()->template UnpackAnyType<typename RRPrimUtil<T>::BoxedType>(mdata);
			}
			else
			{
				RR_INTRUSIVE_PTR<RRValue> ret = GetNode()->template UnpackAnyType<typename RRPrimUtil<T>::BoxedType>(mdata);
				verify(ret);
				return ret;
			}
		}

		boost::function<void(RR_INTRUSIVE_PTR<RRValue>&)> verify;
	};


	class ROBOTRACONTEUR_CORE_API  ServiceStub;
	
	class ROBOTRACONTEUR_CORE_API  WireClientBase : public virtual WireBase
	{
		friend class WireConnectionBase;
		friend class WireSubscriptionBase;

	public:

		virtual ~WireClientBase() {}

		virtual std::string GetMemberName();

		virtual void WirePacketReceived(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e = 0);

		virtual void Shutdown();



		virtual void AsyncClose(RR_SHARED_PTR<WireConnectionBase> endpoint, bool remote, uint32_t ee, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout);

		RR_SHARED_PTR<ServiceStub> GetStub();

	protected:

		virtual void SendWirePacket(RR_INTRUSIVE_PTR<RRValue> packet, TimeSpec time, uint32_t endpoint, bool message3);

		std::string m_MemberName;

		RR_SHARED_PTR<WireConnectionBase> connection;
		boost::mutex connection_lock;

		RR_WEAK_PTR<ServiceStub> stub;

		void AsyncConnect_internal(RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<WireConnectionBase>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout);

		void AsyncConnect_internal1(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<WireConnectionBase>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);


		WireClientBase(const std::string& name, RR_SHARED_PTR<ServiceStub> stub, MemberDefinition_Direction direction);

		virtual RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(MemberDefinition_Direction direction, bool message3) = 0;

		RR_INTRUSIVE_PTR<RRValue> PeekInValueBase(TimeSpec& ts);
		RR_INTRUSIVE_PTR<RRValue> PeekOutValueBase(TimeSpec& ts);
		void PokeOutValueBase(RR_INTRUSIVE_PTR<RRValue> value);

		void AsyncPeekInValueBase(RR_MOVE_ARG(boost::function<void(const RR_INTRUSIVE_PTR<RRValue>&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);
		void AsyncPeekOutValueBase(RR_MOVE_ARG(boost::function<void(const RR_INTRUSIVE_PTR<RRValue>&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);
		void AsyncPokeOutValueBase(const RR_INTRUSIVE_PTR<RRValue>& value, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);

		void AsyncPeekValueBaseEnd1(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<RobotRaconteurException> err,
			boost::function< void(const RR_INTRUSIVE_PTR<RRValue>&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>) >& handler);
				
	};

	template <typename T>
	class WireClient : public virtual Wire<T>, public virtual WireClientBase
	{
	public:

		WireClient(const std::string& name, RR_SHARED_PTR<ServiceStub> stub, MemberDefinition_Direction direction = MemberDefinition_Direction_both, boost::function<void(RR_INTRUSIVE_PTR<RRValue>&)> verify = NULL) : WireClientBase(name, stub, direction), Wire<T>(verify)
		{
			if (boost::is_same<T, RR_INTRUSIVE_PTR<MessageElement> >::value)
			{
				rawelements = true;
			}
			else
			{
				rawelements = false;
			}

		}

		virtual ~WireClient() {}
				
		virtual void AsyncConnect(RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<WireConnection<T> >, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			AsyncConnect_internal(boost::bind(handler,boost::bind(&WireClient<T>::AsyncConnect_cast,_1),_2),timeout);
		}

		virtual RR_SHARED_PTR<WireConnection<T> > Connect()
		{
			RR_SHARED_PTR<detail::sync_async_handler<WireConnection<T> > > t=RR_MAKE_SHARED<detail::sync_async_handler<WireConnection<T> > >();
			AsyncConnect(boost::bind(&detail::sync_async_handler<WireConnection<T> >::operator(),t,_1,_2),GetNode()->GetRequestTimeout());
			return t->end(); 
		}

	protected:

		static RR_SHARED_PTR<WireConnection<T> > AsyncConnect_cast(RR_SHARED_PTR<WireConnectionBase>& b)
		{
			return rr_cast<WireConnection<T> >(b);
		}

		void AsyncPeekValueBaseEnd2(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts, RR_SHARED_PTR<RobotRaconteurException> err,
			boost::function<void(const T&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
		{			

			if (err)
			{
				typename boost::initialized<T> err_value;
				handler(err_value, ts, err);
				return;
			}
			
			T value2;
			try
			{
				value2 = RRPrimUtil<T>::PreUnpack(value);				
			}
			catch (std::exception& exp)
			{
				typename boost::initialized<T> err_value;
				RR_SHARED_PTR<RobotRaconteurException> err = RobotRaconteurExceptionUtil::ExceptionToSharedPtr(exp);
				handler(err_value, ts, err);
				return;
			}

			handler(value2, ts, err);
		}

	public:

		virtual T PeekInValue(TimeSpec& ts) { return RRPrimUtil<T>::PreUnpack(PeekInValueBase(ts)); }
		virtual T PeekOutValue(TimeSpec& ts) { return RRPrimUtil<T>::PreUnpack(PeekOutValueBase(ts)); }
		virtual void PokeOutValue(const T& value) { return PokeOutValueBase(RRPrimUtil<T>::PrePack(value)); }
		virtual void AsyncPeekInValue(RR_MOVE_ARG(boost::function<void(const T&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			AsyncPeekInValueBase(boost::bind(&WireClient::AsyncPeekValueBaseEnd2, RR_DYNAMIC_POINTER_CAST<WireClient>(shared_from_this()), _1, _2, _3, RR_MOVE(handler)), timeout);
		}
		virtual void AsyncPeekOutValue(RR_MOVE_ARG(boost::function<void(const T&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			AsyncPeekOutValueBase(boost::bind(&WireClient::AsyncPeekValueBaseEnd2, RR_DYNAMIC_POINTER_CAST<WireClient>(shared_from_this()), _1, _2, _3, RR_MOVE(handler)), timeout);
		}
		virtual void AsyncPokeOutValue(const T& value, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			AsyncPokeOutValueBase(RRPrimUtil<T>::PrePack(value), RR_MOVE(handler), timeout);
		}
		
		//Unused service-side functions
		virtual boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> GetWireConnectCallback() { throw InvalidOperationException("Not valid for client"); }
		virtual void SetWireConnectCallback(boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> function) { throw InvalidOperationException("Not valid for client"); }
		virtual boost::function<T(const uint32_t&)> GetPeekInValueCallback() { throw InvalidOperationException("Not valid for client"); }
		virtual void SetPeekInValueCallback(boost::function<T(const uint32_t&)> function) { throw InvalidOperationException("Not valid for client"); }
		virtual boost::function<T(const uint32_t&)> GetPeekOutValueCallback() { throw InvalidOperationException("Not valid for client"); }
		virtual void SetPeekOutValueCallback(boost::function<T(const uint32_t&)> function) { throw InvalidOperationException("Not valid for client"); }
		virtual boost::function<void(const T&, const TimeSpec&, const uint32_t&)> GetPokeOutValueCallback() { throw InvalidOperationException("Not valid for client"); }
		virtual void SetPokeOutValueCallback(boost::function<void(const T&, const TimeSpec&, const uint32_t&)> function) { throw InvalidOperationException("Not valid for client"); }
		

	protected:
		virtual RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(MemberDefinition_Direction direction, bool message3)
		{
			return RR_MAKE_SHARED<WireConnection<T> >(RR_STATIC_POINTER_CAST<WireBase>(shared_from_this()), 0, direction, message3);
		}

	};


	class ROBOTRACONTEUR_CORE_API  ServiceSkel;
	class ROBOTRACONTEUR_CORE_API  WireServerBase : public virtual WireBase
	{
		friend class WireConnectionBase;

	public:

		virtual ~WireServerBase() {}

		virtual std::string GetMemberName();

		virtual void WirePacketReceived(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e=0);

		virtual void Shutdown();
		
		virtual void AsyncClose(RR_SHARED_PTR<WireConnectionBase> endpoint, bool remote, uint32_t ee, RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout);

		virtual RR_INTRUSIVE_PTR<MessageEntry> WireCommand(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

		RR_SHARED_PTR<ServiceSkel> GetSkel();

	protected:

		virtual void SendWirePacket(RR_INTRUSIVE_PTR<RRValue> data, TimeSpec time, uint32_t endpoint, bool message3);

		std::string m_MemberName;

		RR_UNORDERED_MAP<uint32_t,RR_SHARED_PTR<WireConnectionBase> > connections;
		boost::mutex connections_lock;

		RR_WEAK_PTR<ServiceSkel> skel;

		
		WireServerBase(const std::string& name, RR_SHARED_PTR<ServiceSkel> skel, MemberDefinition_Direction direction);

		virtual RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(uint32_t e, MemberDefinition_Direction direction, bool message3)=0;
				
		virtual void fire_WireConnectCallback(RR_SHARED_PTR<WireConnectionBase> e)=0;

		

		bool init;
		boost::signals2::connection listener_connection;
	public:
		void ClientDisconnected(RR_SHARED_PTR<ServerContext> context, ServerServiceListenerEventType ev, RR_SHARED_PTR<void> param);

	protected:
		virtual RR_INTRUSIVE_PTR<RRValue> do_PeekInValue(const uint32_t&) = 0;
		virtual RR_INTRUSIVE_PTR<RRValue> do_PeekOutValue(const uint32_t&) = 0;
		virtual void do_PokeOutValue(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec&, const uint32_t& ep) = 0;
		
	};

	template <typename T>
	class WireServer : public virtual WireServerBase, public virtual Wire<T>
	{

	public:

		WireServer(const std::string& name, RR_SHARED_PTR<ServiceSkel> skel, MemberDefinition_Direction direction = MemberDefinition_Direction_both, boost::function<void(RR_INTRUSIVE_PTR<RRValue>&)> verify = NULL) : WireServerBase(name, skel, direction), Wire<T>(verify)
		{
			if (boost::is_same<T, RR_INTRUSIVE_PTR<MessageElement> >::value)
			{
				rawelements = true;
			}
			else
			{
				rawelements = false;
			}
		}

		virtual ~WireServer() {}

		virtual void AsyncConnect(RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<WireConnection<T> >, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE)	{throw InvalidOperationException("Not valid for server");}
		virtual RR_SHARED_PTR<WireConnection<T> > Connect()	{ throw InvalidOperationException("Not valid for server"); }
		virtual T PeekInValue(TimeSpec& ts) { throw InvalidOperationException("Not valid for server"); }
		virtual T PeekOutValue(TimeSpec& ts) { throw InvalidOperationException("Not valid for server"); }
		virtual void PokeOutValue(const T& value) { throw InvalidOperationException("Not valid for server"); }
		virtual void AsyncPeekInValue(RR_MOVE_ARG(boost::function<void(const T&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE) { throw InvalidOperationException("Not valid for server"); }
		virtual void AsyncPeekOutValue(RR_MOVE_ARG(boost::function<void(const T&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE) { throw InvalidOperationException("Not valid for server"); }
		virtual void AsyncPokeOutValue(const T& value, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE) { throw InvalidOperationException("Not valid for server"); }


		virtual boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> GetWireConnectCallback() {	return callback; }
		virtual void SetWireConnectCallback(boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> function) { callback=function; }
		virtual boost::function<T(const uint32_t&)> GetPeekInValueCallback() { return peek_in_callback; }
		virtual void SetPeekInValueCallback(boost::function<T(const uint32_t&)> function) { peek_in_callback = function; }
		virtual boost::function<T(const uint32_t&)> GetPeekOutValueCallback() { return peek_out_callback; }
		virtual void SetPeekOutValueCallback(boost::function<T(const uint32_t&)> function) { peek_out_callback = function; }
		virtual boost::function<void(const T&, const TimeSpec&, const uint32_t&)> GetPokeOutValueCallback() { return poke_out_callback; }
		virtual void SetPokeOutValueCallback(boost::function<void(const T&, const TimeSpec&, const uint32_t&)> function) { poke_out_callback = function; }
		
	protected:
		virtual RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(uint32_t e, MemberDefinition_Direction direction, bool message3)
		{
			return RR_MAKE_SHARED<WireConnection<T> >(RR_STATIC_POINTER_CAST<WireBase>(shared_from_this()), e, direction, message3);
		}

		boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> callback;
		boost::function<T(const uint32_t&)> peek_in_callback;
		boost::function<T(const uint32_t&)> peek_out_callback;
		boost::function<void(const T&, const TimeSpec&, const uint32_t&)> poke_out_callback;

		virtual void fire_WireConnectCallback(RR_SHARED_PTR<WireConnectionBase> e)
		{
			if (!callback) return;
			callback(RR_STATIC_POINTER_CAST<WireConnection<T> >(e));
		}

		virtual RR_INTRUSIVE_PTR<RRValue> do_PeekInValue(const uint32_t& ep)
		{
			if (!peek_in_callback) throw InvalidOperationException("Invalid operation");
			return RRPrimUtil<T>::PrePack(peek_in_callback(ep));
		}
		
		virtual RR_INTRUSIVE_PTR<RRValue> do_PeekOutValue(const uint32_t& ep)
		{
			if (!peek_out_callback) throw InvalidOperationException("Invalid operation");
			return RRPrimUtil<T>::PrePack(peek_out_callback(ep));
		}

		virtual void do_PokeOutValue(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts, const uint32_t& ep)
		{
			if (!poke_out_callback) throw InvalidOperationException("Invalid operation");
			return poke_out_callback(RRPrimUtil<T>::PreUnpack(value), ts, ep);
		}

	public:
		virtual void Shutdown()
		{
			WireServerBase::Shutdown();
			callback.clear();
			peek_in_callback.clear();
			peek_out_callback.clear();
			poke_out_callback.clear();
		}

	};

	namespace detail
	{
		template<typename T>
		class Wire_traits;

		template<typename T>
		class Wire_traits<Wire<T> >
		{
		public:
			typedef WireConnection<T> wireconnection_type;
			typedef WireClient<T> wireclient_type;
			typedef WireServer<T> wireserver_type;			
		};

	}

	namespace detail
	{
		class WireBroadcaster_connected_connection;
	}

	class ROBOTRACONTEUR_CORE_API WireBroadcasterBase : public RR_ENABLE_SHARED_FROM_THIS<WireBroadcasterBase>
	{
	public:

		size_t GetActiveWireConnectionCount();

		virtual ~WireBroadcasterBase();

		boost::function<bool(RR_SHARED_PTR<WireBroadcasterBase>&, uint32_t)> GetPredicate();
		void SetPredicate(boost::function<bool(RR_SHARED_PTR<WireBroadcasterBase>&, uint32_t)> f);

	protected:

		WireBroadcasterBase();		

		void InitBase(RR_SHARED_PTR<WireBase> wire);

		void ConnectionClosedBase(RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> ep);

		void ConnectionConnectedBase(RR_SHARED_PTR<WireConnectionBase> ep);

		void SetOutValueBase(RR_INTRUSIVE_PTR<RRValue> value);

		virtual void AttachWireServerEvents(RR_SHARED_PTR<WireServerBase> w);

		virtual void AttachWireConnectionEvents(RR_SHARED_PTR<WireConnectionBase> w, RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> cep);

		RR_INTRUSIVE_PTR<RRValue> ClientPeekInValueBase();
		
		std::list<RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> > connected_wires;
		boost::mutex connected_wires_lock;
		RR_WEAK_PTR<WireServerBase> wire;
		RR_WEAK_PTR<RobotRaconteurNode> node;

		bool copy_element;

		boost::function<bool (RR_SHARED_PTR<WireBroadcasterBase>&, uint32_t)> predicate;

		RR_INTRUSIVE_PTR<RRValue> out_value;

		void ServiceEvent(ServerServiceListenerEventType evt);

		RR_SHARED_PTR<WireBase> GetWireBase();
	};



	template <typename T>
	class WireBroadcaster : public WireBroadcasterBase
	{		
	public:

		WireBroadcaster() {}

		void Init(RR_SHARED_PTR<Wire<T> > wire)
		{			
			InitBase(wire);
		}

		void SetOutValue(T value)
		{
			SetOutValueBase(RRPrimUtil<T>::PrePack(value));
		}

		RR_SHARED_PTR<Wire<T> > GetWire()
		{
			return rr_cast<Wire<T> >(GetWireBase());
		}

	protected:

		T ClientPeekInValue()
		{
			return RRPrimUtil<T>::PreUnpack(ClientPeekInValueBase());
		}

		static T ClientPeekOutValue() { throw ReadOnlyMemberException("Read only wire"); }
		static T ClientPokeOutValue() { throw ReadOnlyMemberException("Read only wire"); }

		virtual void AttachWireServerEvents(RR_SHARED_PTR<WireServerBase> w)
		{
			RR_SHARED_PTR < WireServer<T> > w_T= rr_cast<WireServer<T> >(w);
			w_T->SetWireConnectCallback(boost::bind(&WireBroadcaster::ConnectionConnectedBase, this->shared_from_this(), _1));
			w_T->SetPeekInValueCallback(boost::bind(&WireBroadcaster<T>::ClientPeekInValue, RR_STATIC_POINTER_CAST<WireBroadcaster<T> >(this->shared_from_this())));
			w_T->SetPeekOutValueCallback(boost::bind(&WireBroadcaster<T>::ClientPeekOutValue));
			w_T->SetPokeOutValueCallback(boost::bind(&WireBroadcaster<T>::ClientPokeOutValue));
		}

		virtual void AttachWireConnectionEvents(RR_SHARED_PTR<WireConnectionBase> w, RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> c)
		{
			RR_SHARED_PTR<WireConnection<T> > w_T = rr_cast<WireConnection<T> >(w);
			w_T->SetWireConnectionClosedCallback(boost::bind(&WireBroadcaster::ConnectionClosedBase, this->shared_from_this(), c));
		}


	};

	
	template <typename T, typename U>
	class WireUnicastReceiverBase : public RR_ENABLE_SHARED_FROM_THIS<WireUnicastReceiverBase<T,U> >
	{
	public:

		typedef typename detail::Wire_traits<T>::wireserver_type wireserver_type;
		typedef typename detail::Wire_traits<T>::wireconnection_type wireconnection_type;

		WireUnicastReceiverBase() {}
		virtual ~WireUnicastReceiverBase() {}

		void Init(RR_SHARED_PTR<T> wire)
		{
			RR_SHARED_PTR<wireserver_type> wire_server = RR_DYNAMIC_POINTER_CAST<wireserver_type>(wire);
			if (!wire_server) throw InvalidOperationException("WireServer required for WireUnicastReceiver");
			this->wire = wire_server;
			wire_server->SetWireConnectCallback(boost::bind(&WireUnicastReceiverBase<T,U>::ConnectionConnected, this->shared_from_this(), _1));
			wire_server->SetPeekInValueCallback(boost::bind(&WireUnicastReceiverBase<T,U>::ClientPeekInValue));
			wire_server->SetPeekOutValueCallback(boost::bind(&WireUnicastReceiverBase<T,U>::ClientPeekOutValue, this->shared_from_this()));
			wire_server->SetPokeOutValueCallback(boost::bind(&WireUnicastReceiverBase<T,U>::ClientPokeOutValue, this->shared_from_this(), _1, _2, _3));

			wire_server->GetSkel()->GetContext()->ServerServiceListener.connect(
				boost::signals2::signal<void(RR_SHARED_PTR<ServerContext>, ServerServiceListenerEventType, RR_SHARED_PTR<void>)>::slot_type(
					boost::bind(&WireUnicastReceiverBase::ServiceEvent, this, _2)
				).track(this->shared_from_this())
			);
		}

		U GetInValue(TimeSpec& ts, uint32_t& ep)
		{
			boost::mutex::scoped_lock lock(this_lock);
			if (!in_value_valid) throw InvalidOperationException("Value not set");
			ts = in_value_ts;
			ep = in_value_ep;
			return in_value;
		}

		boost::signals2::signal<void(const U&, const TimeSpec&, const uint32_t&)> InValueChanged;
		
		RR_SHARED_PTR<T> GetWire()
		{
			return wire;
		}

	protected:

		void ConnectionConnected(RR_SHARED_PTR<wireconnection_type > connection)
		{
			boost::mutex::scoped_lock lock(this_lock);
			if (active_connection)
			{
				try
				{
					active_connection->Close();
				}
				catch (std::exception&) {}
				active_connection.reset();
			}
			active_connection = connection;
			connection->SetWireConnectionClosedCallback(boost::bind(&WireUnicastReceiverBase<T,U>::ConnectionClosed, this->shared_from_this(), _1));			
			connection->WireValueChanged.connect(boost::bind(&WireUnicastReceiverBase<T,U>::ConnectionInValueChanged, this->shared_from_this(), _1, _2, _3));			
		}

		void ConnectionClosed(RR_SHARED_PTR<wireconnection_type> connection)
		{
			boost::mutex::scoped_lock lock(this_lock);
			if (active_connection == connection)
			{
				active_connection.reset();				
			}
		}

		void ConnectionInValueChanged(RR_SHARED_PTR<wireconnection_type> connection, const U& value, const TimeSpec& time)
		{
			ClientPokeOutValue(value, time, connection->GetEndpoint());
		}

		static U ClientPeekInValue()
		{
			throw WriteOnlyMemberException("Write only wire");
		}

		U ClientPeekOutValue()
		{
			boost::mutex::scoped_lock lock(this_lock);
			if (!in_value_valid) throw InvalidOperationException("Value not set");
			return in_value;
		}

		void ClientPokeOutValue(const U& value, const TimeSpec& ts, const uint32_t& ep)
		{
			boost::mutex::scoped_lock lock(this_lock);
			in_value = value;
			in_value_ts = ts;
			in_value_valid.data() = true;
			in_value_ep.data() = ep;

			InValueChanged(value, ts, ep);
		}

		void ServiceEvent(ServerServiceListenerEventType evt)
		{
			if (evt != ServerServiceListenerEventType_ServiceClosed) return;
			boost::mutex::scoped_lock lock(this_lock);
			InValueChanged.disconnect_all_slots();			
		}

		RR_SHARED_PTR<wireserver_type> wire;
		RR_SHARED_PTR<wireconnection_type> active_connection;
		boost::mutex this_lock;
		U in_value;
		TimeSpec in_value_ts;
		boost::initialized<bool> in_value_valid;
		boost::initialized<uint32_t> in_value_ep;

	};

	template <typename T>
	class WireUnicastReceiver : public WireUnicastReceiverBase<Wire<T>, T>
	{

	};


}

#pragma warning(pop)
