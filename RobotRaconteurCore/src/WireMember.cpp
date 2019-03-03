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

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "RobotRaconteur/WireMember.h"
#include "RobotRaconteur/Client.h"
#include "RobotRaconteur/Service.h"
#include "RobotRaconteur/DataTypes.h"

#include "WireMember_private.h"

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>

#define RR_WIRE_CONNECTION_LISTENER_ITER(command) \
	try \
	{ \
		boost::mutex::scoped_lock listen_lock(listeners_lock); \
		for (std::list<RR_WEAK_PTR<WireConnectionBaseListener> >::iterator e = listeners.begin(); e != listeners.end();) \
		{ \
			RR_SHARED_PTR<WireConnectionBaseListener> w1 = e->lock(); \
			if (!w1) \
			{ \
				e = listeners.erase(e); \
				continue; \
			} \
			command; \
			e++; \
		} \
	} \
	catch (std::exception& exp) \
	{ \
		RobotRaconteurNode::TryHandleException(node, &exp); \
	} \

namespace RobotRaconteur
{	
	uint32_t WireConnectionBase::GetEndpoint()
	{
		return endpoint;
	}

	TimeSpec WireConnectionBase::GetLastValueReceivedTime()
	{
		boost::mutex::scoped_lock lock (inval_lock);
		if (!inval_valid)
			throw ValueNotSetException("No value received");
		return lasttime_recv;
	}

	TimeSpec WireConnectionBase::GetLastValueSentTime()
	{
		boost::mutex::scoped_lock lock (outval_lock);
		if (!outval_valid)
			throw ValueNotSetException("No value sent");
		return lasttime_send;
	}

	RR_SHARED_PTR<WireBase> WireConnectionBase::GetParent()
	{
		RR_SHARED_PTR<WireBase> out=parent.lock();
		if (!out) throw InvalidOperationException("Wire connection has been closed");
		return out;
	}

	void WireConnectionBase::Close()
	{
		RR_SHARED_PTR<detail::sync_async_handler<void> > t=RR_MAKE_SHARED<detail::sync_async_handler<void > >();
		AsyncClose(boost::bind(&detail::sync_async_handler<void>::operator(),t,_1),GetNode()->GetRequestTimeout());
		t->end_void();
	}

	void WireConnectionBase::AsyncClose(RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		{
			boost::mutex::scoped_lock lock(outval_lock);
			send_closed = true;
			outval_wait.notify_all();
			GetParent()->AsyncClose(shared_from_this(), false, endpoint, RR_MOVE(handler), timeout);
		}

		{
			boost::mutex::scoped_lock lock(inval_lock);
			recv_closed = true;
			inval_wait.notify_all();
		}		
	}

	WireConnectionBase::WireConnectionBase(RR_SHARED_PTR<WireBase> parent, uint32_t endpoint, MemberDefinition_Direction direction, bool message3)
	{
		this->parent=parent;
		this->endpoint=endpoint;
		outval_valid=false;
		inval_valid=false;
		ignore_inval = false;
		this->message3 = message3;
		send_closed = false;
		recv_closed = false;
		node = parent->GetNode();
		this->direction = direction;
	}

	RR_SHARED_PTR<RobotRaconteurNode> WireConnectionBase::GetNode()
	{
		RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
		if (!n) throw InvalidOperationException("Node has been released");
		return n;
	}

	void WireConnectionBase::WirePacketReceived(TimeSpec timespec, RR_INTRUSIVE_PTR<RRValue> packet)
	{		
		{
			boost::mutex::scoped_lock lock (recvlock);

			if (ignore_inval)
			{
				return;
			}

			if (lasttime_recv == TimeSpec(0,0) || timespec > lasttime_recv)
			{
				{
					boost::mutex::scoped_lock lock2(inval_lock);
					inval = packet;
					lasttime_recv = timespec;
					inval_valid = true;
					inval_wait.notify_all();
				}

				
				RR_WIRE_CONNECTION_LISTENER_ITER(w1->WireValueChanged(shared_from_this(), packet, timespec));
						

				lock.unlock();

				try
				{
					this->wire_value_changed_semaphore.try_fire_next(
					boost::bind(&WireConnectionBase::fire_WireValueChanged, this, packet, timespec));
				}
				catch (std::exception& exp)
				{
					RobotRaconteurNode::TryHandleException(node, &exp);
				}

			}
			
		}
	}

	void WireConnectionBase_RemoteClose_emptyhandler(RR_SHARED_PTR<RobotRaconteurException> err) {}

	void WireConnectionBase::RemoteClose()
	{
		{
			boost::mutex::scoped_lock lock(outval_lock);
			send_closed = true;
			outval_wait.notify_all();
		}

		{
			boost::mutex::scoped_lock lock(inval_lock);
			recv_closed = true;
			inval_wait.notify_all();
		}

		try
		{
			fire_WireClosedCallback();
		}
		catch (std::exception& exp)
		{
			RobotRaconteurNode::TryHandleException(node, &exp);
		}

		
		RR_WIRE_CONNECTION_LISTENER_ITER(w1->WireConnectionClosed(shared_from_this()));
		
		try
		{
			boost::mutex::scoped_lock lock (sendlock);
			//if (parent.expired()) return;
			//boost::mutex::scoped_lock lock2 (recvlock);
			GetParent()->AsyncClose(shared_from_this(),true,endpoint,&WireConnectionBase_RemoteClose_emptyhandler,1000);
		}
		catch (std::exception&) {}
		
	}

	RR_INTRUSIVE_PTR<RRValue> WireConnectionBase::GetInValueBase()
	{
		if (direction == MemberDefinition_Direction_writeonly)
			throw WriteOnlyMemberException("Write only member");
		RR_INTRUSIVE_PTR<RRValue> val;
		{
			boost::mutex::scoped_lock lock2(inval_lock);
		if (!inval_valid)
			throw ValueNotSetException("Value not set");
		val=inval;
		}
		return val;
	}

	RR_INTRUSIVE_PTR<RRValue> WireConnectionBase::GetOutValueBase() 
	{
		if (direction == MemberDefinition_Direction_readonly)
			throw ReadOnlyMemberException("Read only member");
		RR_INTRUSIVE_PTR<RRValue> val;
		{
			boost::mutex::scoped_lock lock2(outval_lock);
		if (!outval_valid)
			throw ValueNotSetException("Value not set");
		val=outval;
		}
		return val;
	}

	void WireConnectionBase::SetOutValueBase(RR_INTRUSIVE_PTR<RRValue> value)
	{
		if (direction == MemberDefinition_Direction_readonly)
			throw ReadOnlyMemberException("Read only member");

		{
			boost::mutex::scoped_lock lock (sendlock);
			
			TimeSpec time = TimeSpec::Now();
			if (time <= lasttime_send)
			{
				time=lasttime_send;
				time.nanoseconds += 1;
				time.cleanup_nanosecs();
			}
			
			
			GetParent()->SendWirePacket(value, time, endpoint, message3);
			
			boost::mutex::scoped_lock lock2(outval_lock);
			outval = value;
			lasttime_send = time;
			outval_valid = true;
			outval_wait.notify_all();
			
		}
	}

	bool WireConnectionBase::TryGetInValueBase(RR_INTRUSIVE_PTR<RRValue>& value, TimeSpec& time)
	{
		boost::mutex::scoped_lock lock2(inval_lock);
		if (!inval_valid) return false;
		value = inval;
		time = lasttime_recv;
		return true;
	}

	bool WireConnectionBase::TryGetOutValueBase(RR_INTRUSIVE_PTR<RRValue>& value, TimeSpec& time)
	{
		boost::mutex::scoped_lock lock2(outval_lock);
		if (!outval_valid) return false;				
		value = outval;
		time = lasttime_send;
		return true;
	}

	bool WireConnectionBase::GetInValueValid()
	{
		boost::mutex::scoped_lock lock2(inval_lock);
		return inval_valid;
	}

	bool WireConnectionBase::GetOutValueValid()
	{
		boost::mutex::scoped_lock lock2(outval_lock);
		return outval_valid;
	}

	bool WireConnectionBase::WaitInValueValid(int32_t timeout)
	{
		boost::mutex::scoped_lock lock2(inval_lock);
		if (inval_valid) return true;
		if (timeout == 0) return inval_valid;
		if (recv_closed) return false;
		if (timeout < 0)
		{
			inval_wait.wait(lock2);
		}
		else
		{
			inval_wait.wait_for(lock2, boost::chrono::milliseconds(timeout));
		}
		return inval_valid;
	}

	bool WireConnectionBase::WaitOutValueValid(int32_t timeout)
	{
		boost::mutex::scoped_lock lock2(outval_lock);
		if (outval_valid) return true;
		if (timeout == 0) return outval_valid;
		if (send_closed) return false;
		if (timeout < 0)
		{
			outval_wait.wait(lock2);
		}
		else
		{
			outval_wait.wait_for(lock2, boost::chrono::milliseconds(timeout));
		}
		return outval_valid;
	}

	bool WireConnectionBase::GetIgnoreInValue()
	{
		boost::mutex::scoped_lock lock2(inval_lock);
		return ignore_inval;
	}

	void WireConnectionBase::SetIgnoreInValue(bool ignore)
	{
		boost::mutex::scoped_lock lock2(inval_lock);
		ignore_inval = ignore;
	}

	void WireConnectionBase::AddListener(RR_SHARED_PTR<WireConnectionBaseListener> listener)
	{
		boost::mutex::scoped_lock lock(listeners_lock);
		listeners.push_back(listener);
	}

	void WireConnectionBase::Shutdown()
	{
		{
			boost::mutex::scoped_lock lock(outval_lock);
			send_closed = true;
			outval_wait.notify_all();
		}

		{
			boost::mutex::scoped_lock lock(inval_lock);
			recv_closed = true;
			inval_wait.notify_all();
		}

		RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&WireConnectionBase::fire_WireClosedCallback, shared_from_this()),true);

		std::list<RR_WEAK_PTR<WireConnectionBaseListener> > listeners1;
		{
			boost::mutex::scoped_lock lock(listeners_lock);
			listeners.swap(listeners1);
		}
		
		BOOST_FOREACH(RR_WEAK_PTR<WireConnectionBaseListener> l, listeners1)
		{
			RR_SHARED_PTR<WireConnectionBaseListener> l1 = l.lock();
			if (l1)
			{
				RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&WireConnectionBaseListener::WireConnectionClosed, l1, shared_from_this()),true);
			}
		}
	}

	MemberDefinition_Direction WireConnectionBase::Direction()
	{
		return direction;
	}

	RR_INTRUSIVE_PTR<RRValue> WireBase::UnpackPacket(RR_INTRUSIVE_PTR<MessageEntry> me, TimeSpec& ts)
	{
		if (me->EntryFlags & MessageEntryFlags_TIMESPEC)
		{
			ts = me->EntryTimeSpec;
		}
		else
		{
			RR_INTRUSIVE_PTR<MessageElementStructure> s = MessageElement::FindElement(me->elements, "packettime")->CastData<MessageElementStructure>();
			int64_t seconds = RRArrayToScalar(MessageElement::FindElement(s->Elements, "seconds")->CastData<RRArray<int64_t> >());
			int32_t nanoseconds = RRArrayToScalar(MessageElement::FindElement(s->Elements, "nanoseconds")->CastData<RRArray<int32_t> >());
			ts = TimeSpec(seconds, nanoseconds);
		}

		RR_INTRUSIVE_PTR<RRValue> data;
		if (!rawelements)
		{
			data = UnpackData(MessageElement::FindElement(me->elements, "packet"));
		}
		else
		{
			data = MessageElement::FindElement(me->elements, "packet");
		}

		return data;
	}

	void WireBase::DispatchPacket (RR_INTRUSIVE_PTR<MessageEntry> me, RR_SHARED_PTR<WireConnectionBase> e)
	{
		TimeSpec timespec;
		RR_INTRUSIVE_PTR<RRValue> data = UnpackPacket(me,timespec);
		e->WirePacketReceived(timespec,data);
	}

	RR_INTRUSIVE_PTR<MessageEntry> WireBase::PackPacket(RR_INTRUSIVE_PTR<RRValue> data, TimeSpec time, bool message3)
	{
		if (message3)
		{
			RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_WirePacket, GetMemberName());
			m->EntryTimeSpec = time;
			m->EntryFlags |= MessageEntryFlags_TIMESPEC;
			if (!rawelements)
			{
				RR_INTRUSIVE_PTR<MessageElementData> pdata = PackData(data);
				m->elements.push_back(CreateMessageElement("packet", pdata));
			}
			else
			{
				RR_INTRUSIVE_PTR<MessageElement> pme = RR_DYNAMIC_POINTER_CAST<MessageElement>(data);
				pme->ElementName = "packet";
				m->elements.push_back(pme);
			}

			return m;
		}
		else
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > timespec1;
			timespec1.push_back(CreateMessageElement("seconds", ScalarToRRArray(time.seconds)));
			timespec1.push_back(CreateMessageElement("nanoseconds", ScalarToRRArray(time.nanoseconds)));
			RR_INTRUSIVE_PTR<MessageElementStructure> s = CreateMessageElementStructure("RobotRaconteur.TimeSpec", timespec1);


			std::vector<RR_INTRUSIVE_PTR<MessageElement> > elems;
			elems.push_back(CreateMessageElement("packettime", s));
			if (!rawelements)
			{
				RR_INTRUSIVE_PTR<MessageElementData> pdata = PackData(data);
				elems.push_back(CreateMessageElement("packet", pdata));
			}
			else
			{
				RR_INTRUSIVE_PTR<MessageElement> pme = RR_DYNAMIC_POINTER_CAST<MessageElement>(data);
				pme->ElementName = "packet";
				elems.push_back(pme);
			}

			RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_WirePacket, GetMemberName());
			m->elements = elems;
			m->MetaData = "unreliable\n";
			return m;
		}
	}

	RR_SHARED_PTR<RobotRaconteurNode> WireBase::GetNode()
	{
		RR_SHARED_PTR<RobotRaconteurNode> n=node.lock();
		if (!n) throw InvalidOperationException("Node has been released");
		return n;
	}

	MemberDefinition_Direction WireBase::Direction()
	{
		return direction;
	}

	std::string WireClientBase::GetMemberName()
	{
		return m_MemberName;
	}

	void WireClientBase::WirePacketReceived(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
	{
		//boost::shared_lock<boost::shared_mutex> lock2(stub_lock);
		
		if (m->EntryType == MessageEntryType_WireClosed)
			{
				try
				{
					boost::mutex::scoped_lock lock (connection_lock);
					connection->Close();
					connection.reset();
				}
				catch (std::exception&)
				{
				}
			}
			else if (m->EntryType == MessageEntryType_WirePacket)
			{
				try
				{
					RR_SHARED_PTR<WireConnectionBase> c;
					{
					boost::mutex::scoped_lock lock (connection_lock);
					c=connection;
					if (!c)
						return;
					}
					DispatchPacket(m, c);
				}
				catch (std::exception&)
				{
					
				}
			}
	}

	void WireClientBase::Shutdown()
	{
		
		try
		{
			RR_SHARED_PTR<WireConnectionBase> c;
			{
				boost::mutex::scoped_lock lock (connection_lock);
				c=connection;
			}
			
			
			try
			{
				//RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_WireDisconnectReq, GetMemberName());
				//RR_INTRUSIVE_PTR<MessageEntry> ret = GetStub()->ProcessRequest(m);
				if (c)	c->Shutdown();
							
			}
			catch (std::exception& exp) 
			{
				RobotRaconteurNode::TryHandleException(node, &exp);
			}
			
		}
		catch (std::exception&)
		{
		}
		
	}

	RR_SHARED_PTR<ServiceStub> WireClientBase::GetStub()
	{
		RR_SHARED_PTR<ServiceStub> out=stub.lock();
		if (!out) throw InvalidOperationException("Wire connection has been closed");
		return out;
	}

	void WireClientBase::SendWirePacket(RR_INTRUSIVE_PTR<RRValue> packet, TimeSpec time, uint32_t endpoint, bool message3)
	{
		RR_INTRUSIVE_PTR<MessageEntry> m = PackPacket(packet, time, message3);
		
		GetStub()->SendWireMessage(m);
	}
	
	void WireClientBase::AsyncClose(RR_SHARED_PTR<WireConnectionBase> endpoint, bool remote, uint32_t ee, RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		
		
		{
			boost::mutex::scoped_lock lock (connection_lock);
			if (!remote)
			{
			RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_WireDisconnectReq, GetMemberName());
			GetStub()->AsyncProcessRequest(m,boost::bind(handler,_2),timeout);
			}
			connection.reset();
		}
	}

	void WireClientBase::AsyncConnect_internal(RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<WireConnectionBase>,RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		
		
		{
			boost::mutex::scoped_lock lock (connection_lock);
			try
			{
				if (connection != 0)
					throw InvalidOperationException("Already connected");
				
				RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_WireConnectReq, GetMemberName());
				GetStub()->AsyncProcessRequest(m,boost::bind(&WireClientBase::AsyncConnect_internal1, RR_DYNAMIC_POINTER_CAST<WireClientBase>(shared_from_this()),_1,_2,handler),timeout);


				
			}
			catch (std::exception &e)
			{
				connection.reset();
				throw e;
			}
		}
	}

	void WireClientBase::AsyncConnect_internal1(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void (RR_SHARED_PTR<WireConnectionBase>,RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		if (err)
		{
					
			detail::InvokeHandlerWithException(node, handler,err);
			return;			
		}

		try
		{		
			{
				boost::mutex::scoped_lock lock (connection_lock);
				if (connection)
				{
					
					detail::InvokeHandlerWithException(node, handler, RR_MAKE_SHARED<ServiceException>("Wire already connected"));
					
					return;
				}
				connection = CreateNewWireConnection(direction, GetStub()->GetContext()->UseMessage3());
			}
			

			detail::InvokeHandler(node, handler, connection);
			
			
		}
		catch (std::exception& err2)
		{
			detail::InvokeHandlerWithException(node, handler, err2);		
		}
	}

	std::string WireServerBase::GetMemberName()
	{
		return m_MemberName;
	}

	WireClientBase::WireClientBase(const std::string& name, RR_SHARED_PTR<ServiceStub> stub, MemberDefinition_Direction direction)
	{
		this->stub=stub;
		this->m_MemberName=name;
		this->node=stub->RRGetNode();
		this->direction = direction;
	}

	RR_INTRUSIVE_PTR<RRValue> WireClientBase::PeekInValueBase(TimeSpec& ts)
	{
		RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m = CreateMessageEntry(MessageEntryType_WirePeekInValueReq, GetMemberName());
		RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr = GetStub()->ProcessRequest(m);
		return UnpackPacket(mr, ts);		
	}

	RR_INTRUSIVE_PTR<RRValue> WireClientBase::PeekOutValueBase(TimeSpec& ts)
	{
		RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m = CreateMessageEntry(MessageEntryType_WirePeekOutValueReq, GetMemberName());
		RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr = GetStub()->ProcessRequest(m);
		return UnpackPacket(mr, ts);
	}

	void WireClientBase::PokeOutValueBase(RR_INTRUSIVE_PTR<RRValue> value)
	{
		RR_INTRUSIVE_PTR<MessageEntry> m = PackPacket(value, TimeSpec::Now(), GetStub()->GetContext()->UseMessage3());
		m->EntryType = MessageEntryType_WirePokeOutValueReq;
		m->MetaData = "";		
		RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr = GetStub()->ProcessRequest(m);
	}

	void WireClientBase::AsyncPeekValueBaseEnd1(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<RobotRaconteurException> err, 
		boost::function< void(const RR_INTRUSIVE_PTR<RRValue>&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>) >& handler)
	{
		TimeSpec ts;
		RR_INTRUSIVE_PTR<RRValue> value;
		if (err)
		{			
			handler(RR_INTRUSIVE_PTR<RRValue>(), ts, err);
			return;
		}
		if (m->Error != RobotRaconteur::MessageErrorType_None)
		{			
			handler(RR_INTRUSIVE_PTR<RRValue>(), ts, RobotRaconteurExceptionUtil::MessageEntryToException(m));
			return;
		}		
		try
		{			
			value = UnpackPacket(m, ts);
		}
		catch (RobotRaconteur::RobotRaconteurException& err)
		{			
			handler(RR_INTRUSIVE_PTR<RRValue>(), ts, RobotRaconteur::RobotRaconteurExceptionUtil::DownCastException(err));
			return;
		}
		catch (std::exception& err)
		{			
			handler(RR_INTRUSIVE_PTR<RRValue>(), ts, 
				RR_MAKE_SHARED<RobotRaconteurRemoteException>(std::string(typeid(err).name()), err.what()));
			return;
		}
		handler(value, ts, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());
	}

	void WireClientBase::AsyncPeekInValueBase(RR_MOVE_ARG(boost::function<void(const RR_INTRUSIVE_PTR<RRValue>&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m = CreateMessageEntry(MessageEntryType_WirePeekInValueReq, GetMemberName());
		GetStub()->AsyncProcessRequest(m, boost::bind(&WireClientBase::AsyncPeekValueBaseEnd1, RR_DYNAMIC_POINTER_CAST<WireClientBase>(shared_from_this()), _1, _2, handler),timeout);		
	}

	void WireClientBase::AsyncPeekOutValueBase(RR_MOVE_ARG(boost::function<void(const RR_INTRUSIVE_PTR<RRValue>&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m = CreateMessageEntry(MessageEntryType_WirePeekOutValueReq, GetMemberName());
		GetStub()->AsyncProcessRequest(m, boost::bind(&WireClientBase::AsyncPeekValueBaseEnd1, RR_DYNAMIC_POINTER_CAST<WireClientBase>(shared_from_this()), _1, _2, handler), timeout);
	}

	void WireClientBase_AsyncPokeValueBaseEnd(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, boost::function< void(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > handler)
	{
		if (err)
		{
			handler(err);
			return;
		}
		if (m->Error != RobotRaconteur::MessageErrorType_None)
		{
			handler(RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(m));
			return;
		}
		handler(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());
	}

	void WireClientBase::AsyncPokeOutValueBase(const RR_INTRUSIVE_PTR<RRValue>& value, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		RR_INTRUSIVE_PTR<MessageEntry> m = PackPacket(value, TimeSpec::Now(), GetStub()->GetContext()->UseMessage3());
		m->EntryType = MessageEntryType_WirePokeOutValueReq;
		m->MetaData = "";
		GetStub()->AsyncProcessRequest(m, boost::bind(&WireClientBase_AsyncPokeValueBaseEnd, _1, _2, handler), timeout);
	}

	void WireServerBase::WirePacketReceived(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
	{
		if (m->EntryType == MessageEntryType_WirePacket)
		{
			try
			{
				RR_SHARED_PTR<WireConnectionBase> c;
				{
					boost::mutex::scoped_lock lock(connections_lock);
					RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<WireConnectionBase> >::iterator e1 = connections.find(e);
					if (e1 == connections.end()) return;
					c = e1->second;
				}
				DispatchPacket(m, c);
			}
			catch (std::exception&)
			{				
			}

		}
	}

	void WireServerBase::ClientDisconnected(RR_SHARED_PTR<ServerContext> context, ServerServiceListenerEventType ev, RR_SHARED_PTR<void> param)
{
	
	if (ev==ServerServiceListenerEventType_ClientDisconnected)
	{
		uint32_t ep=*RR_STATIC_POINTER_CAST<uint32_t>(param);
		
		std::vector<RR_SHARED_PTR<WireConnectionBase> > c;
		{
			boost::mutex::scoped_lock lock(connections_lock);
			for (RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<WireConnectionBase> >::iterator ee= connections.begin(); ee!=connections.end(); )
			{
				if (ee->first==ep)
				{
					//ee->second->RemoteClose();
					c.push_back(ee->second);
					ee=connections.erase(ee);
				
				
				
				}
				else
				{
					++ee;
				}
			}
		}
		
		BOOST_FOREACH (RR_SHARED_PTR<WireConnectionBase> ee, c)
		{
			try
			{
				ee->fire_WireClosedCallback();
			}
			catch (std::exception& exp) 
			{
				RobotRaconteurNode::TryHandleException(node, &exp);
			}
			
		}
		
	}
}

	void WireServerBase::Shutdown()
	{
		

		std::vector<RR_SHARED_PTR<WireConnectionBase> > c;
		{
			boost::mutex::scoped_lock lock(connections_lock);
		
			boost::copy(connections | boost::adaptors::map_values, std::back_inserter(c));
			
			connections.clear();
		}
		BOOST_FOREACH (RR_SHARED_PTR<WireConnectionBase>& e, c)
		{
			try
			{
				RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_WireClosed, GetMemberName());

				GetSkel()->SendWireMessage(m, e->GetEndpoint());
			}
			catch (std::exception&)
			{
			}

			try
			{
				e->Shutdown();
			}
			catch (std::exception& exp) 
			{
				RobotRaconteurNode::TryHandleException(node, &exp);
			}
		}
		
		//skel.reset();;

		listener_connection.disconnect();		
	}

	RR_SHARED_PTR<ServiceSkel> WireServerBase::GetSkel()
	{
		RR_SHARED_PTR<ServiceSkel> out=skel.lock();
		if (!out) throw InvalidOperationException("Wire connection has been closed");
		return out;
	}

	void WireServerBase::SendWirePacket(RR_INTRUSIVE_PTR<RRValue> packet, TimeSpec time, uint32_t e, bool message3 )
	{
		{
		boost::mutex::scoped_lock lock(connections_lock);
		if (connections.find(e) == connections.end())
			throw InvalidOperationException("Wire has been disconnected");
		}
		RR_INTRUSIVE_PTR<MessageEntry> m = PackPacket(packet, time, message3);		
		
		GetSkel()->SendWireMessage(m, e);
	}

	void WireServerBase::AsyncClose(RR_SHARED_PTR<WireConnectionBase> c, bool remote, uint32_t ee, RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		
		if (!remote)
		{
		RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_WireClosed, GetMemberName());

		GetSkel()->SendWireMessage(m, ee);
		}

		try
		{

			
			{
				boost::mutex::scoped_lock lock (connections_lock);
				if (connections.find(c->GetEndpoint()) != connections.end())
					connections.erase(c->GetEndpoint());
			}
		}
		catch (std::exception&)
		{
		}

		
		detail::PostHandler(node, handler, true);		
	}

	void wire_server_client_disconnected(RR_SHARED_PTR<ServerContext> context, ServerServiceListenerEventType ev, RR_SHARED_PTR<void> param,RR_WEAK_PTR<WireServerBase> w)
	{
		RR_SHARED_PTR<WireServerBase> p=w.lock();
		if (!p) return;
		p->ClientDisconnected(context,ev,param);

	}

	RR_INTRUSIVE_PTR<MessageEntry> WireServerBase::WireCommand(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
	{

		
		{
			boost::mutex::scoped_lock lock(connections_lock);
			switch (m->EntryType)
			{
				case MessageEntryType_WireConnectReq:
				{
					if (!init)
					{
						RR_WEAK_PTR<WireServerBase> weak= RR_DYNAMIC_POINTER_CAST<WireServerBase>(shared_from_this());
						listener_connection=GetSkel()->GetContext()->ServerServiceListener.connect(boost::bind(&wire_server_client_disconnected,_1,_2,_3,weak));
						init=true;
					}


						if (connections.find(e) == connections.end())
							connections.insert(std::make_pair(e, CreateNewWireConnection( e, direction, GetSkel()->GetContext()->UseMessage3(e))));
						RR_SHARED_PTR<WireConnectionBase> con = connections.at(e);
						lock.unlock();
						fire_WireConnectCallback(con);

						RR_INTRUSIVE_PTR<MessageEntry> ret = CreateMessageEntry(MessageEntryType_WireConnectRet, GetMemberName());
						return ret;
				}
				case MessageEntryType_WireDisconnectReq:
				{
					RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<WireConnectionBase> >::iterator e1 = connections.find(e);
						if (e1 == connections.end())
							throw ServiceException("Invalid wire connection");
						RR_SHARED_PTR<WireConnectionBase> c = e1->second;
						lock.unlock();
						c->RemoteClose();
						//connections.erase(e);
						return CreateMessageEntry(MessageEntryType_WireDisconnectRet, GetMemberName());
				}
				case MessageEntryType_WirePeekInValueReq:
				{
					if (direction == MemberDefinition_Direction_writeonly)
						throw WriteOnlyMemberException("Write only member");
					RR_INTRUSIVE_PTR<RRValue> value = do_PeekInValue(e);					
					RR_INTRUSIVE_PTR<MessageEntry> mr = PackPacket(value, TimeSpec::Now(), GetSkel()->GetContext()->UseMessage3(e));
					mr->EntryType = MessageEntryType_WirePeekInValueRet;
					mr->MetaData = "";
					return mr;
				}
				case MessageEntryType_WirePeekOutValueReq:
				{
					if (direction == MemberDefinition_Direction_readonly)
						throw ReadOnlyMemberException("Read only member");
					RR_INTRUSIVE_PTR<RRValue> value = do_PeekOutValue(e);
					RR_INTRUSIVE_PTR<MessageEntry> mr = PackPacket(value, TimeSpec::Now(), GetSkel()->GetContext()->UseMessage3(e));
					mr->EntryType = MessageEntryType_WirePeekOutValueRet;
					mr->MetaData = "";
					return mr;
				}
				case MessageEntryType_WirePokeOutValueReq:
				{
					if (direction == MemberDefinition_Direction_readonly)
						throw ReadOnlyMemberException("Read only member");
					TimeSpec ts;
					RR_INTRUSIVE_PTR<RRValue> value1 = UnpackPacket(m, ts);
					do_PokeOutValue(value1, ts, e);
					return CreateMessageEntry(MessageEntryType_WirePokeOutValueRet, GetMemberName());
				}

				default:
					throw InvalidOperationException("Invalid Command");



			}
		}


	}

	WireServerBase::WireServerBase(const std::string& name, RR_SHARED_PTR<ServiceSkel> skel, MemberDefinition_Direction direction)
	{
		this->skel=skel;
		this->m_MemberName=name;
		this->init=false;
		this->node=skel->RRGetNode();
		this->direction = direction;

	}

	// WireBroadcasterBase
	namespace detail
	{
		class WireBroadcaster_connected_connection
		{
		public:

			WireBroadcaster_connected_connection(RR_SHARED_PTR<WireConnectionBase > connection)
			{
				this->connection = connection;
			}

			RR_WEAK_PTR<WireConnectionBase> connection;

		};
	}

	WireBroadcasterBase::~WireBroadcasterBase()
	{

	}

	WireBroadcasterBase::WireBroadcasterBase()
	{
		copy_element = false;
	}

	void WireBroadcasterBase::InitBase(RR_SHARED_PTR<WireBase> wire)
	{
		RR_SHARED_PTR<WireServerBase> wire1 = RR_DYNAMIC_POINTER_CAST<WireServerBase>(wire);
		if (!wire1) throw InvalidArgumentException("Wire must be a WireServer for WireBroadcaster");

		this->wire = wire1;
		this->node = wire->GetNode();

		AttachWireServerEvents(wire1);

		wire1->GetSkel()->GetContext()->ServerServiceListener.connect(
			boost::signals2::signal<void(RR_SHARED_PTR<ServerContext>, ServerServiceListenerEventType, RR_SHARED_PTR<void>)>::slot_type(
				boost::bind(&WireBroadcasterBase::ServiceEvent, this, _2)
			).track(shared_from_this())
		);
	}

	void WireBroadcasterBase::ServiceEvent(ServerServiceListenerEventType evt)
	{
		if (evt != ServerServiceListenerEventType_ServiceClosed) return;
		boost::mutex::scoped_lock lock(connected_wires_lock);
		predicate.clear();
	}

	void WireBroadcasterBase::ConnectionClosedBase(RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> ep)
	{
		boost::mutex::scoped_lock lock(connected_wires_lock);
		try
		{
			connected_wires.remove(ep);
		}
		catch (std::exception&) {}
	}

	void WireBroadcasterBase::ConnectionConnectedBase(RR_SHARED_PTR<WireConnectionBase > ep)
	{
		boost::mutex::scoped_lock lock(connected_wires_lock);
		ep->SetIgnoreInValue(true);

		RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> c = RR_MAKE_SHARED<detail::WireBroadcaster_connected_connection>(ep);

		AttachWireConnectionEvents(ep, c);

		connected_wires.push_back(c);
	}

	void WireBroadcasterBase::SetOutValueBase(RR_INTRUSIVE_PTR<RRValue> value)
	{
		boost::mutex::scoped_lock lock(connected_wires_lock);

		out_value = value;

		RR_SHARED_PTR<WireBroadcasterBase> this_ = shared_from_this();

		for (std::list<RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> >::iterator ee = connected_wires.begin(); ee != connected_wires.end(); )
		{
			try
			{
				RR_SHARED_PTR<WireConnectionBase > c = (*ee)->connection.lock();
				if (!c)
				{
					ee = connected_wires.erase(ee);
					continue;
				}
				else
				{
					if (predicate)
					{
						if (!predicate(this_, c->GetEndpoint()))
						{
							ee++;
							continue;
						}
					}
					
					if (!copy_element)
					{
						c->SetOutValueBase(value);
					}
					else
					{
						RR_INTRUSIVE_PTR<MessageElement> value2 = ShallowCopyMessageElement(rr_cast<MessageElement>(value));
						c->SetOutValueBase(value2);
					}
					ee++;
				}
			}
			catch (std::exception&)
			{
				ee = connected_wires.erase(ee);
			}
		}

	}

	size_t WireBroadcasterBase::GetActiveWireConnectionCount()
	{
		boost::mutex::scoped_lock lock(connected_wires_lock);
		return connected_wires.size();
	}

	void WireBroadcasterBase::AttachWireServerEvents(RR_SHARED_PTR<WireServerBase> p)
	{

	}

	void WireBroadcasterBase::AttachWireConnectionEvents(RR_SHARED_PTR<WireConnectionBase> p, RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> cep)
	{

	}

	boost::function<bool(RR_SHARED_PTR<WireBroadcasterBase>&, uint32_t)> WireBroadcasterBase::GetPredicate()
	{
		boost::mutex::scoped_lock lock(connected_wires_lock);
		return predicate;
	}
	void WireBroadcasterBase::SetPredicate(boost::function<bool(RR_SHARED_PTR<WireBroadcasterBase>&, uint32_t)> f)
	{
		boost::mutex::scoped_lock lock(connected_wires_lock);
		predicate = f;
	}

	RR_INTRUSIVE_PTR<RRValue> WireBroadcasterBase::ClientPeekInValueBase()
	{
		boost::mutex::scoped_lock lock(connected_wires_lock);
		return out_value;
	}

	RR_SHARED_PTR<WireBase> WireBroadcasterBase::GetWireBase()
	{
		RR_SHARED_PTR<WireBase> wire1 = wire.lock();
		if (!wire1) throw InvalidOperationException("Wire released");
		return wire1;
	}


}