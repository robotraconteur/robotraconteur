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


#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/Endpoint.h"
#include "RobotRaconteur/DataTypes.h"
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>
#include <boost/call_traits.hpp>
#include <list>

#pragma warning (push)
#pragma warning (disable: 4250)
#pragma warning (disable: 4996) 

#include <boost/signals2.hpp>

namespace RobotRaconteur
{
	
	class ROBOTRACONTEUR_CORE_API PipeBase;
	class ROBOTRACONTEUR_CORE_API PipeEndpointBaseListener;
	namespace detail { class PipeSubscription_connection; }
	
	class ROBOTRACONTEUR_CORE_API PipeEndpointBase : public RR_ENABLE_SHARED_FROM_THIS<PipeEndpointBase>, private boost::noncopyable
	{
		friend class PipeBase;
		friend class PipeClientBase;
		friend class PipeServerBase;
		friend class PipeBroadcasterBase;
		friend class PipeSubscriptionBase;
		friend class detail::PipeSubscription_connection;

	public:

		virtual ~PipeEndpointBase() {}

		virtual int32_t GetIndex();

		virtual uint32_t GetEndpoint();

		bool GetRequestPacketAck();

		void SetRequestPacketAck(bool ack);
		
		virtual void Close();

		virtual void AsyncClose(RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		virtual size_t Available();

		bool IsUnreliable();
		MemberDefinition_Direction Direction();

		bool GetIgnoreReceived();
		void SetIgnoreReceived(bool ignore);

		virtual void AddListener(RR_SHARED_PTR<PipeEndpointBaseListener> listener);

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

	protected:

		virtual void RemoteClose();

		PipeEndpointBase(RR_SHARED_PTR<PipeBase> parent, int32_t index, uint32_t endpoint=0, bool unreliable=false, MemberDefinition_Direction direction = MemberDefinition_Direction_both, bool message3=false);

		bool unreliable;
		MemberDefinition_Direction direction;

		bool RequestPacketAck;

		void AsyncSendPacketBase(RR_INTRUSIVE_PTR<RRValue> packet, RR_MOVE_ARG(boost::function<void(uint32_t,RR_SHARED_PTR<RobotRaconteurException>)>) handler);

		RR_INTRUSIVE_PTR<RRValue> ReceivePacketBase();
		RR_INTRUSIVE_PTR<RRValue> PeekPacketBase();

		RR_INTRUSIVE_PTR<RRValue> ReceivePacketBaseWait(int32_t timeout = RR_TIMEOUT_INFINITE);
		RR_INTRUSIVE_PTR<RRValue> PeekPacketBaseWait(int32_t timeout = RR_TIMEOUT_INFINITE);

		bool TryReceivePacketBaseWait(RR_INTRUSIVE_PTR<RRValue>& packet, int32_t timeout = RR_TIMEOUT_INFINITE, bool peek=false);
		
		boost::mutex sendlock;
		boost::mutex recvlock;

		std::deque<RR_INTRUSIVE_PTR<RRValue> > recv_packets;
		boost::condition_variable recv_packets_wait;

		uint32_t increment_packet_number(uint32_t packetnum);

	
		void PipePacketReceived(RR_INTRUSIVE_PTR<RRValue> packet, uint32_t packetnum);

		void PipePacketAckReceived(uint32_t packetnum);

		void Shutdown();

	protected:
		virtual void fire_PipeEndpointClosedCallback()=0;
	
		virtual void fire_PacketReceivedEvent()=0;
		
		virtual void fire_PacketAckReceivedEvent(uint32_t packetnum)=0;

		RR_SHARED_PTR<PipeBase> GetParent();

	protected:
		
		bool closed;

		uint32_t send_packet_number;
		uint32_t recv_packet_number;

		RR_WEAK_PTR<PipeBase> parent;
		int32_t index;
		uint32_t endpoint;

		RR_UNORDERED_MAP<uint32_t,RR_INTRUSIVE_PTR<RRValue> > out_of_order_packets;

		bool ignore_incoming_packets;
		bool message3;

		boost::mutex listeners_lock;
		std::list<RR_WEAK_PTR<PipeEndpointBaseListener> > listeners;

		detail::async_signal_semaphore pipe_packet_received_semaphore;

		RR_WEAK_PTR<RobotRaconteurNode> node;
	};

	template <typename T>
	class PipeEndpoint : public PipeEndpointBase
	{
	private:
		boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >)> PipeEndpointClosedCallback;
		boost::mutex PipeEndpointClosedCallback_lock;
	public:
		
		boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >)> GetPipeEndpointClosedCallback()
		{
			boost::mutex::scoped_lock lock(PipeEndpointClosedCallback_lock);
			return PipeEndpointClosedCallback;
		}

		void SetPipeEndpointClosedCallback(boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >)> callback)
		{
			boost::mutex::scoped_lock lock(PipeEndpointClosedCallback_lock);
			PipeEndpointClosedCallback=callback;
		}
				
		boost::signals2::signal<void (RR_SHARED_PTR<PipeEndpoint<T> >)> PacketReceivedEvent;
		boost::signals2::signal<void (RR_SHARED_PTR<PipeEndpoint<T> >,uint32_t)> PacketAckReceivedEvent;

		virtual uint32_t SendPacket(typename boost::call_traits<T>::param_type packet)
		{
			 RR_SHARED_PTR<detail::sync_async_handler<uint32_t> > t=RR_MAKE_SHARED<detail::sync_async_handler<uint32_t> >();
			 boost::function<void(RR_SHARED_PTR<uint32_t>, RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&detail::sync_async_handler<uint32_t>::operator(), t, _1, _2);
			 AsyncSendPacket(packet,boost::bind(&PipeEndpoint::send_handler,_1,_2,h));
			 return *t->end();
		}

		virtual void AsyncSendPacket(typename boost::call_traits<T>::param_type packet, RR_MOVE_ARG(boost::function<void(uint32_t,RR_SHARED_PTR<RobotRaconteurException>)>) handler)
		{
			 AsyncSendPacketBase(RRPrimUtil<T>::PrePack(packet), RR_MOVE(handler));
		}

		virtual T ReceivePacket()
		{
			return RRPrimUtil<T>::PreUnpack(ReceivePacketBase());
		}

		virtual T PeekNextPacket()
		{
			return RRPrimUtil<T>::PreUnpack(PeekPacketBase());
		}

		virtual T ReceivePacketWait(int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			return RRPrimUtil<T>::PreUnpack(ReceivePacketBaseWait(timeout));
		}

		virtual T PeekNextPacketWait(int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			return RRPrimUtil<T>::PreUnpack(PeekPacketBaseWait(timeout));
		}

		virtual bool TryReceivePacketWait(T& val, int32_t timeout = RR_TIMEOUT_INFINITE, bool peek = false)
		{
			RR_INTRUSIVE_PTR<RRValue> o;
			if (!TryReceivePacketBaseWait(o, timeout, peek)) return false;
			val = RRPrimUtil<T>::PreUnpack(o);
			return true;
		}
		

		PipeEndpoint(RR_SHARED_PTR<PipeBase> parent, int32_t index, uint32_t endpoint=0, bool unreliable=false, MemberDefinition_Direction direction = MemberDefinition_Direction_both, bool message3=false) 
			: PipeEndpointBase(parent,index,endpoint,unreliable,direction,message3) {};

		

	protected:
		virtual void fire_PipeEndpointClosedCallback()
		{
			boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >)> c=GetPipeEndpointClosedCallback();
			if (!c) return;
			c(RR_STATIC_POINTER_CAST<PipeEndpoint<T> >(shared_from_this()));
		}
	
	
		virtual void fire_PacketReceivedEvent()
		{
			PacketReceivedEvent(RR_STATIC_POINTER_CAST<PipeEndpoint<T> >(shared_from_this()));
		}
		
		virtual void fire_PacketAckReceivedEvent(uint32_t packetnum)
		{
			PacketAckReceivedEvent(RR_STATIC_POINTER_CAST<PipeEndpoint<T> >(shared_from_this()),packetnum);
		}

		static void send_handler(uint32_t packetnumber, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<uint32_t>,RR_SHARED_PTR<RobotRaconteurException>)>& handler)
		{
			handler(RR_MAKE_SHARED<uint32_t>(packetnumber),err);
		}

	public:
		virtual void Close()
		{
			PipeEndpointBase::Close();
			{
			
				boost::mutex::scoped_lock lock(PipeEndpointClosedCallback_lock);
				PipeEndpointClosedCallback.clear();

			}
			PacketReceivedEvent.disconnect_all_slots();
			PacketAckReceivedEvent.disconnect_all_slots();
		}

	protected:

		virtual void AsyncClose1(RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& handler)
		{
			try
			{
				{
					boost::mutex::scoped_lock lock(PipeEndpointClosedCallback_lock);
					PipeEndpointClosedCallback.clear();
				}
			PacketReceivedEvent.disconnect_all_slots();
			PacketAckReceivedEvent.disconnect_all_slots();
			}
			catch (std::exception&) {}

			
			handler(err);
		}

	public:
		virtual void AsyncClose(RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout=2000)
		{
			PipeEndpointBase::AsyncClose(boost::bind(&PipeEndpoint<T>::AsyncClose1,RR_STATIC_POINTER_CAST<PipeEndpoint<T> >(shared_from_this()),_1,handler),timeout);
			
		}

	protected:
		


		virtual void RemoteClose()
		{
			PipeEndpointBase::RemoteClose();
			{
			boost::mutex::scoped_lock lock(PipeEndpointClosedCallback_lock);
			PipeEndpointClosedCallback.clear();
			}
			PacketReceivedEvent.disconnect_all_slots();
			PacketAckReceivedEvent.disconnect_all_slots();
		}

	};

	class ROBOTRACONTEUR_CORE_API PipeBase : public RR_ENABLE_SHARED_FROM_THIS<PipeBase>, private boost::noncopyable
	{
		friend class PipeEndpointBase;

	public:

		virtual ~PipeBase() {}

		static const int32_t ANY_INDEX=-1;

		virtual std::string GetMemberName()=0;

		virtual void PipePacketReceived(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e=0)=0;

		virtual void Shutdown()=0;


		

		virtual void AsyncClose(RR_SHARED_PTR<PipeEndpointBase> endpoint, bool remote, uint32_t ee, RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)=0;

	protected:

		bool unreliable;

		virtual void AsyncSendPipePacket(RR_INTRUSIVE_PTR<RRValue> data, int32_t index, uint32_t packetnumber, bool requestack, uint32_t endpoint, bool unreliable, bool message3, RR_MOVE_ARG(boost::function<void(uint32_t,RR_SHARED_PTR<RobotRaconteurException>)>) handler)=0;

		bool rawelements;

		void DispatchPacketAck (RR_INTRUSIVE_PTR<MessageElement> me, RR_SHARED_PTR<PipeEndpointBase> e);

		bool DispatchPacket(RR_INTRUSIVE_PTR<MessageElement> me, RR_SHARED_PTR<PipeEndpointBase> e, uint32_t& packetnumber);

		RR_INTRUSIVE_PTR<MessageElement> PackPacket(RR_INTRUSIVE_PTR<RRValue> data, int32_t index, uint32_t packetnumber, bool requestack, bool message3);

		virtual void DeleteEndpoint(RR_SHARED_PTR<PipeEndpointBase> e)=0;

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
		bool IsUnreliable();
	};


	template <typename T>
	class Pipe : public virtual PipeBase
	{
	public:
		
		friend class PipeEndpointBase;

		Pipe(boost::function<void(RR_INTRUSIVE_PTR<RRValue>&)> verify)
		{
			this->verify = verify;
		}

		virtual ~Pipe() {}

		virtual boost::function<void(RR_SHARED_PTR<PipeEndpoint<T> >)> GetPipeConnectCallback()=0;
		
		virtual void SetPipeConnectCallback(boost::function<void(RR_SHARED_PTR<PipeEndpoint<T> >)> function)=0;
		
		virtual RR_SHARED_PTR<PipeEndpoint<T> > Connect(int32_t index)=0;

		virtual void AsyncConnect(int32_t index, RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout=RR_TIMEOUT_INFINITE)=0;

		virtual void AsyncConnect(RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			AsyncConnect(-1,RR_MOVE(handler),timeout);
		}

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
				RR_INTRUSIVE_PTR<RRValue> ret= GetNode()->template UnpackAnyType<typename RRPrimUtil<T>::BoxedType>(mdata);
				verify(ret);
				return ret;
			}
		}
		
	protected:
		boost::function<void(RR_INTRUSIVE_PTR<RRValue>&)> verify;
	};

	
	class ROBOTRACONTEUR_CORE_API ServiceStub;
	
	class ROBOTRACONTEUR_CORE_API PipeClientBase : public virtual PipeBase
	{
	public:

		friend class PipeSubscriptionBase;

		virtual ~PipeClientBase() {}

		virtual std::string GetMemberName();

		virtual void PipePacketReceived(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e=0);

		virtual void Shutdown();
		
		

		virtual void AsyncClose(RR_SHARED_PTR<PipeEndpointBase> endpoint, bool remote, uint32_t ee, RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout);

		RR_SHARED_PTR<ServiceStub> GetStub();

	protected:

		virtual void AsyncSendPipePacket(RR_INTRUSIVE_PTR<RRValue> data, int32_t index, uint32_t packetnumber, bool requestack, uint32_t endpoint, bool unreliable, bool message3, RR_MOVE_ARG(boost::function<void(uint32_t,RR_SHARED_PTR<RobotRaconteurException>)>) handler);

		
		std::string m_MemberName;

		RR_UNORDERED_MAP<int32_t,RR_SHARED_PTR<PipeEndpointBase> > pipeendpoints;
		boost::mutex pipeendpoints_lock;

		RR_WEAK_PTR<ServiceStub> stub;
		
		std::list<boost::tuple<int32_t, int32_t> > connecting_endpoints;
		int32_t connecting_key_count;
		RR_UNORDERED_MAP<int32_t, RR_SHARED_PTR<PipeEndpointBase> > early_endpoints;

		void AsyncConnect_internal(int32_t index, RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<PipeEndpointBase>,RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout);

		void AsyncConnect_internal1(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, int32_t index, int32_t key, boost::function<void (RR_SHARED_PTR<PipeEndpointBase>,RR_SHARED_PTR<RobotRaconteurException>)>& handler);

		PipeClientBase(const std::string& name, RR_SHARED_PTR<ServiceStub> stub, bool unreliable, MemberDefinition_Direction direction);

		virtual RR_SHARED_PTR<PipeEndpointBase> CreateNewPipeEndpoint(int32_t index, bool unreliable, MemberDefinition_Direction direction, bool message3)=0;

		virtual void DeleteEndpoint(RR_SHARED_PTR<PipeEndpointBase> e);

		

	};


	template <typename T>
	class PipeClient : public virtual Pipe<T>, public virtual PipeClientBase
	{
	public:

		virtual ~PipeClient() {}
		
		virtual boost::function<void(RR_SHARED_PTR<PipeEndpoint<T> >)> GetPipeConnectCallback()
		{
			throw InvalidOperationException("Not valid for client");
		}
		
		virtual void SetPipeConnectCallback(boost::function<void(RR_SHARED_PTR<PipeEndpoint<T> >)> function)
		{
			throw InvalidOperationException("Not valid for client");
		}
		
		virtual void AsyncConnect(int32_t index, RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			
			AsyncConnect_internal(index,boost::bind(handler,boost::bind(&PipeClient<T>::AsyncConnect_cast,_1),_2),timeout); 
		}

		virtual RR_SHARED_PTR<PipeEndpoint<T> > Connect(int32_t index)
		{
			RR_SHARED_PTR<detail::sync_async_handler<PipeEndpoint<T> > > t=RR_MAKE_SHARED<detail::sync_async_handler<PipeEndpoint<T> > >();
			AsyncConnect(index,boost::bind(&detail::sync_async_handler<PipeEndpoint<T> >::operator(),t,_1,_2),GetNode()->GetRequestTimeout());
			return t->end();
		}

		PipeClient(const std::string& name, RR_SHARED_PTR<ServiceStub> stub, bool unreliable=false, MemberDefinition_Direction direction = MemberDefinition_Direction_both, boost::function<void(RR_INTRUSIVE_PTR<RRValue>&)> verify=NULL) : PipeClientBase(name,stub,unreliable,direction), Pipe<T>(verify)
		{
			if (boost::is_same<T,RR_INTRUSIVE_PTR<MessageElement> >::value)
			{
				rawelements=true;
			}
			else
			{
				rawelements=false;
			}
		
		}		

		using PipeClientBase::GetMemberName;
		using PipeClientBase::PipePacketReceived;
		using PipeClientBase::Shutdown;
		using PipeClientBase::AsyncSendPipePacket;
		using PipeClientBase::AsyncClose;
		

	protected:

		static RR_SHARED_PTR<PipeEndpoint<T> > AsyncConnect_cast(RR_SHARED_PTR<PipeEndpointBase>& b)
		{
			return rr_cast<PipeEndpoint<T> >(b);
		}

		virtual RR_SHARED_PTR<PipeEndpointBase> CreateNewPipeEndpoint(int32_t index, bool unreliable, MemberDefinition_Direction direction, bool message3)
		{
			return RR_MAKE_SHARED<PipeEndpoint<T> >(RR_STATIC_POINTER_CAST<PipeBase>(shared_from_this()),index,0,unreliable,direction,message3);
		}

	};


	class ROBOTRACONTEUR_CORE_API ServiceSkel;
	class ROBOTRACONTEUR_CORE_API PipeServerBase : public virtual PipeBase
	{
	public:

		virtual ~PipeServerBase() {}

		virtual std::string GetMemberName();

		virtual void PipePacketReceived(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e=0);

		virtual void Shutdown();
		
		virtual void AsyncSendPipePacket(RR_INTRUSIVE_PTR<RRValue> data, int32_t index, uint32_t packetnumber, bool requestack, uint32_t endpoint, bool unreliable, bool message3, RR_MOVE_ARG(boost::function<void(uint32_t,RR_SHARED_PTR<RobotRaconteurException>)>) handler);

		virtual void AsyncClose(RR_SHARED_PTR<PipeEndpointBase> endpoint, bool remote, uint32_t ee, RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout);

		virtual RR_INTRUSIVE_PTR<MessageEntry> PipeCommand(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

		RR_SHARED_PTR<ServiceSkel> GetSkel();

	protected:
		
		std::string m_MemberName;
				
		struct pipe_endpoint_server_id
		{
			pipe_endpoint_server_id(uint32_t endpoint, int32_t index)
			{
				this->endpoint=endpoint;
				this->index=index;
			}

			uint32_t endpoint;
			int32_t index;

			bool operator==(const pipe_endpoint_server_id& rhs) const
			{
				return (endpoint==rhs.endpoint && index==rhs.index);
			}
						
		};
		

		struct hash_value
		{
		 std::size_t operator()(pipe_endpoint_server_id const& e) const {
		  std::size_t seed = 0;
		  boost::hash_combine( seed, e.endpoint );
		  boost::hash_combine( seed, e.index );
		  return seed;
		}
		};

		RR_UNORDERED_MAP<pipe_endpoint_server_id,RR_SHARED_PTR<PipeEndpointBase> , hash_value > pipeendpoints;
		boost::mutex pipeendpoints_lock;

		RR_WEAK_PTR<ServiceSkel> skel;

		
		PipeServerBase(const std::string& name, RR_SHARED_PTR<ServiceSkel> skel, bool unreliable, MemberDefinition_Direction direction);

		virtual RR_SHARED_PTR<PipeEndpointBase> CreateNewPipeEndpoint(int32_t index,uint32_t endpoint, bool unreliable, MemberDefinition_Direction direction, bool message3)=0;

		void DeleteEndpoint(RR_SHARED_PTR<PipeEndpointBase> e);

		virtual void fire_PipeConnectCallback(RR_SHARED_PTR<PipeEndpointBase> e)=0;

		
		bool init;
		boost::signals2::connection listener_connection;
		
	public:
		void ClientDisconnected(RR_SHARED_PTR<ServerContext> context, ServerServiceListenerEventType ev, RR_SHARED_PTR<void> param);

	};

	template <typename T>
	class PipeServer : public virtual PipeServerBase, public virtual Pipe<T>
	{

	public:

		virtual ~PipeServer() {}

		virtual boost::function<void(RR_SHARED_PTR<PipeEndpoint<T> >)> GetPipeConnectCallback()
		{
			return callback;
		}
		
		virtual void SetPipeConnectCallback(boost::function<void(RR_SHARED_PTR<PipeEndpoint<T> >)> function)
		{
			callback=function;
		}
		
		virtual RR_SHARED_PTR<PipeEndpoint<T> > Connect(int32_t index)
		{
			throw InvalidOperationException("Not valid for server");
		}

		virtual void AsyncConnect(int32_t index, RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			throw InvalidOperationException("Not valid for server");
		}

		PipeServer(const std::string& name, RR_SHARED_PTR<ServiceSkel> skel, bool unreliable=false, MemberDefinition_Direction direction=MemberDefinition_Direction_both, boost::function<void(RR_INTRUSIVE_PTR<RRValue>&)> verify = NULL) : PipeServerBase(name,skel,unreliable,direction), Pipe<T>(verify)
		{
			if (boost::is_same<T, RR_INTRUSIVE_PTR<MessageElement> >::value)
			{
				rawelements=true;
			}
			else
			{
				rawelements=false;
			}			
		}


	protected:
		virtual RR_SHARED_PTR<PipeEndpointBase> CreateNewPipeEndpoint(int32_t index, uint32_t endpoint, bool unreliable, MemberDefinition_Direction direction, bool message3)
		{
			return RR_MAKE_SHARED<PipeEndpoint<T> >(RR_STATIC_POINTER_CAST<PipeBase>(shared_from_this()),index,endpoint,unreliable,direction,message3);
		}

		boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >)> callback;

		virtual void fire_PipeConnectCallback(RR_SHARED_PTR<PipeEndpointBase> e)
		{
			if (!callback) return;
			callback(RR_STATIC_POINTER_CAST<PipeEndpoint<T> >(e));
		}

		boost::function<void(RR_INTRUSIVE_PTR<RRValue>&)> verify;

	public:

		virtual void Shutdown()
		{
			PipeServerBase::Shutdown();

			callback.clear();
		}


	};

	namespace detail
	{
		class PipeBroadcasterBase_connected_endpoint;
		struct PipeBroadcasterBase_async_send_operation;
	}

	class ROBOTRACONTEUR_CORE_API PipeBroadcasterBase : public RR_ENABLE_SHARED_FROM_THIS<PipeBroadcasterBase>, private boost::noncopyable
	{
	public:

		virtual ~PipeBroadcasterBase();
				
		size_t GetActivePipeEndpointCount();

		boost::function<bool(RR_SHARED_PTR<PipeBroadcasterBase>&, uint32_t, int32_t) > GetPredicate();
		void SetPredicate(boost::function<bool(RR_SHARED_PTR<PipeBroadcasterBase>&,  uint32_t, int32_t)> f);

	protected:

		PipeBroadcasterBase();

		void InitBase(RR_SHARED_PTR<PipeBase> pipe, int32_t maximum_backlog = -1);

		void EndpointConnectedBase(RR_SHARED_PTR<PipeEndpointBase > ep);

		void EndpointClosedBase(RR_SHARED_PTR<detail::PipeBroadcasterBase_connected_endpoint> ep);

		void PacketAckReceivedBase(RR_SHARED_PTR<detail::PipeBroadcasterBase_connected_endpoint> ep, uint32_t id);

		void handle_send(int32_t id, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<detail::PipeBroadcasterBase_connected_endpoint> ep, RR_SHARED_PTR<detail::PipeBroadcasterBase_async_send_operation> op, int32_t key, int32_t send_key, boost::function<void()>& handler);

		void SendPacketBase(RR_INTRUSIVE_PTR<RRValue> packet);

		void AsyncSendPacketBase(RR_INTRUSIVE_PTR<RRValue> packet, RR_MOVE_ARG(boost::function<void()>) handler);

		virtual void AttachPipeServerEvents(RR_SHARED_PTR<PipeServerBase> p);

		virtual void AttachPipeEndpointEvents(RR_SHARED_PTR<PipeEndpointBase> p, RR_SHARED_PTR<detail::PipeBroadcasterBase_connected_endpoint> cep);

		RR_SHARED_PTR<PipeBase> GetPipeBase();

		std::list<RR_SHARED_PTR<detail::PipeBroadcasterBase_connected_endpoint> > endpoints;
		boost::mutex endpoints_lock;

		RR_WEAK_PTR<PipeServerBase> pipe;
		RR_WEAK_PTR<RobotRaconteurNode> node;
		int32_t maximum_backlog;

		bool copy_element;

		boost::function<bool(RR_SHARED_PTR<PipeBroadcasterBase>&, uint32_t, int32_t)> predicate;

	};



	template <typename T>
	class PipeBroadcaster : public PipeBroadcasterBase
	{

	public:
		PipeBroadcaster() {}

		void Init(RR_SHARED_PTR<Pipe<T> > pipe, int32_t maximum_backlog = -1)
		{
			InitBase(pipe, maximum_backlog);
		}

		void SendPacket(T packet)
		{
			SendPacketBase(RRPrimUtil<T>::PrePack(packet));
		}

		void AsyncSendPacket(T packet, RR_MOVE_ARG(boost::function<void()>) handler)
		{
			AsyncSendPacketBase(RRPrimUtil<T>::PrePack(packet), RR_MOVE(handler));
		}

		RR_SHARED_PTR<Pipe<T> > GetPipe()
		{
			return rr_cast<Pipe<T> >(GetPipeBase());
		}

	protected:
		virtual void AttachPipeServerEvents(RR_SHARED_PTR<PipeServerBase> p)
		{
			RR_SHARED_PTR<PipeServer<T> > p_T = rr_cast<PipeServer<T> >(p);
			
			p_T->SetPipeConnectCallback(boost::bind(&PipeBroadcaster::EndpointConnectedBase, shared_from_this(), _1));			
		}

		virtual void AttachPipeEndpointEvents(RR_SHARED_PTR<PipeEndpointBase> ep, RR_SHARED_PTR<detail::PipeBroadcasterBase_connected_endpoint> cep)
		{
			RR_SHARED_PTR<PipeEndpoint<T> > ep_T = rr_cast<PipeEndpoint<T> >(ep);

			ep_T->SetPipeEndpointClosedCallback(boost::bind(&PipeBroadcaster::EndpointClosedBase, shared_from_this(), cep));
			ep_T->PacketAckReceivedEvent.connect(boost::bind(&PipeBroadcaster::PacketAckReceivedBase, shared_from_this(), cep, _2));
		}

	};

}

#pragma warning(pop)
