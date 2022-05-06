// Copyright 2011-2020 Wason Technology, LLC
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

#define RR_WIRE_CONNECTION_LISTENER_ITER(command)                                                                      \
    try                                                                                                                \
    {                                                                                                                  \
        boost::mutex::scoped_lock listen_lock(listeners_lock);                                                         \
        for (std::list<RR_WEAK_PTR<WireConnectionBaseListener> >::iterator e = listeners.begin();                      \
             e != listeners.end();)                                                                                    \
        {                                                                                                              \
            RR_SHARED_PTR<WireConnectionBaseListener> w1 = e->lock();                                                  \
            if (!w1)                                                                                                   \
            {                                                                                                          \
                e = listeners.erase(e);                                                                                \
                continue;                                                                                              \
            }                                                                                                          \
            command;                                                                                                   \
            e++;                                                                                                       \
        }                                                                                                              \
    }                                                                                                                  \
    catch (std::exception & exp)                                                                                       \
    {                                                                                                                  \
        RobotRaconteurNode::TryHandleException(node, &exp);                                                            \
    }

namespace RobotRaconteur
{
uint32_t WireConnectionBase::GetEndpoint() { return endpoint; }

TimeSpec WireConnectionBase::GetLastValueReceivedTime()
{
    boost::mutex::scoped_lock lock(inval_lock);
    if (!inval_valid)
        throw ValueNotSetException("No value received");
    return lasttime_recv;
}

TimeSpec WireConnectionBase::GetLastValueSentTime()
{
    boost::mutex::scoped_lock lock(outval_lock);
    if (!outval_valid)
        throw ValueNotSetException("No value sent");
    return lasttime_send;
}

RR_SHARED_PTR<WireBase> WireConnectionBase::GetParent()
{
    RR_SHARED_PTR<WireBase> out = parent.lock();
    if (!out)
        throw InvalidOperationException("Wire connection has been closed");
    return out;
}

void WireConnectionBase::Close()
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<void> > t = RR_MAKE_SHARED<detail::sync_async_handler<void> >();
    AsyncClose(boost::bind(&detail::sync_async_handler<void>::operator(), t, RR_BOOST_PLACEHOLDERS(_1)),
               GetNode()->GetRequestTimeout());
    t->end_void();
}

void WireConnectionBase::AsyncClose(RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler,
                                    int32_t timeout)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                            "Requesting close wire connection");

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

WireConnectionBase::WireConnectionBase(RR_SHARED_PTR<WireBase> parent, uint32_t endpoint,
                                       MemberDefinition_Direction direction)
{
    this->parent = parent;
    this->endpoint = endpoint;
    outval_valid = false;
    inval_valid = false;
    inval_lifespan = -1;
    outval_lifespan = -1;
    ignore_inval = false;
    send_closed = false;
    recv_closed = false;
    node = parent->GetNode();
    this->direction = direction;
    this->service_path = parent->GetServicePath();
    this->member_name = parent->GetMemberName();
}

RR_SHARED_PTR<RobotRaconteurNode> WireConnectionBase::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

void WireConnectionBase::WirePacketReceived(TimeSpec timespec, RR_INTRUSIVE_PTR<RRValue> packet)
{
    {
        boost::mutex::scoped_lock lock(recvlock);
        RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();

        if (ignore_inval)
        {
            return;
        }

        if (lasttime_recv == TimeSpec(0, 0) || timespec > lasttime_recv)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                                    "Wire packet in value received timespec " << timespec.seconds << ","
                                                                                              << timespec.nanoseconds);

            {
                boost::mutex::scoped_lock lock2(inval_lock);
                inval = packet;
                lasttime_recv = timespec;
                if (n)
                {
                    lasttime_recv_local = n->NowNodeTime();
                }
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
        else
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                                    "Old wire packet received timespec " << timespec.seconds << ","
                                                                                         << timespec.nanoseconds
                                                                                         << ", dropping");
        }
    }
}

void WireConnectionBase_RemoteClose_emptyhandler(RR_SHARED_PTR<RobotRaconteurException> err) {}

void WireConnectionBase::RemoteClose()
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                            "Received remote close wire connection");

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
        boost::mutex::scoped_lock lock(sendlock);
        // if (parent.expired()) return;
        // boost::mutex::scoped_lock lock2 (recvlock);
        GetParent()->AsyncClose(shared_from_this(), true, endpoint, &WireConnectionBase_RemoteClose_emptyhandler, 1000);
    }
    catch (std::exception&)
    {}
}

namespace detail
{
bool WireConnectionBase_IsValueExpired(RR_WEAK_PTR<RobotRaconteurNode> node, const boost::posix_time::ptime& recv_time,
                                       int32_t lifespan)
{
    if (lifespan < 0)
    {
        return false;
    }

    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
    {
        return true;
    }

    if (recv_time + boost::posix_time::milliseconds(lifespan) < n->NowNodeTime())
    {
        return true;
    }
    return false;
}
} // namespace detail

RR_INTRUSIVE_PTR<RRValue> WireConnectionBase::GetInValueBase()
{
    if (direction == MemberDefinition_Direction_writeonly)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                                "Attempt to get InValue of write only wire");
        throw WriteOnlyMemberException("Write only member");
    }
    RR_INTRUSIVE_PTR<RRValue> val;
    {
        boost::mutex::scoped_lock lock2(inval_lock);
        if (!inval_valid)
            throw ValueNotSetException("Value not set");
        if (detail::WireConnectionBase_IsValueExpired(node, lasttime_recv_local, inval_lifespan))
        {
            throw ValueNotSetException("Value expired");
        }
        val = inval;
    }
    return val;
}

RR_INTRUSIVE_PTR<RRValue> WireConnectionBase::GetOutValueBase()
{
    if (direction == MemberDefinition_Direction_readonly)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                                "Attempt to get OutValue of read only wire");
        throw ReadOnlyMemberException("Read only member");
    }
    RR_INTRUSIVE_PTR<RRValue> val;
    {
        boost::mutex::scoped_lock lock2(outval_lock);
        if (!outval_valid)
            throw ValueNotSetException("Value not set");
        if (detail::WireConnectionBase_IsValueExpired(node, lasttime_send_local, outval_lifespan))
        {
            throw ValueNotSetException("Value expired");
        }
        val = outval;
    }
    return val;
}

void WireConnectionBase::SetOutValueBase(RR_INTRUSIVE_PTR<RRValue> value)
{
    if (direction == MemberDefinition_Direction_readonly)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                                "Attempt to get OutValue of read only wire");
        throw ReadOnlyMemberException("Read only member");
    }

    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();

    try
    {
        boost::mutex::scoped_lock lock(sendlock);

        TimeSpec time = n->NowTimeSpec();
        if (time <= lasttime_send)
        {
            time = lasttime_send;
            time.nanoseconds += 1;
            time.cleanup_nanosecs();
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                                "Wire sending out value packet timespec " << time.seconds << ","
                                                                                          << time.nanoseconds);

        GetParent()->SendWirePacket(value, time, endpoint);

        boost::mutex::scoped_lock lock2(outval_lock);
        outval = value;
        lasttime_send = time;
        lasttime_send_local = n->NowNodeTime();
        outval_valid = true;
        outval_wait.notify_all();
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                                "Wire set OutValue failed: " << exp.what());
        throw;
    }
}

bool WireConnectionBase::TryGetInValueBase(RR_INTRUSIVE_PTR<RRValue>& value, TimeSpec& time)
{
    if (direction == MemberDefinition_Direction_writeonly)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                                "Attempt to get InValue of write only wire");
        throw WriteOnlyMemberException("Write only member");
    }
    boost::mutex::scoped_lock lock2(inval_lock);
    if (!inval_valid)
        return false;
    if (detail::WireConnectionBase_IsValueExpired(node, lasttime_recv_local, inval_lifespan))
    {
        return false;
    }
    value = inval;
    time = lasttime_recv;
    return true;
}

bool WireConnectionBase::TryGetOutValueBase(RR_INTRUSIVE_PTR<RRValue>& value, TimeSpec& time)
{
    if (direction == MemberDefinition_Direction_readonly)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                                "Attempt to get OutValue of read only wire");
        throw ReadOnlyMemberException("Read only member");
    }
    boost::mutex::scoped_lock lock2(outval_lock);
    if (!outval_valid)
        return false;
    if (detail::WireConnectionBase_IsValueExpired(node, lasttime_send_local, outval_lifespan))
    {
        return false;
    }
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
    if (inval_valid)
        return true;
    if (timeout == 0)
        return inval_valid;
    if (recv_closed)
        return false;
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
    if (outval_valid)
        return true;
    if (timeout == 0)
        return outval_valid;
    if (send_closed)
        return false;
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
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                            "IgnoreInValue set to " << ignore)
}

void WireConnectionBase::AddListener(RR_SHARED_PTR<WireConnectionBaseListener> listener)
{
    boost::mutex::scoped_lock lock(listeners_lock);
    listeners.push_back(listener);
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                            "WireConnectionBaseListener added to wire connection");
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

    RobotRaconteurNode::TryPostToThreadPool(
        node, boost::bind(&WireConnectionBase::fire_WireClosedCallback, shared_from_this()), true);

    std::list<RR_WEAK_PTR<WireConnectionBaseListener> > listeners1;
    {
        boost::mutex::scoped_lock lock(listeners_lock);
        listeners.swap(listeners1);
    }

    BOOST_FOREACH (RR_WEAK_PTR<WireConnectionBaseListener> l, listeners1)
    {
        RR_SHARED_PTR<WireConnectionBaseListener> l1 = l.lock();
        if (l1)
        {
            RobotRaconteurNode::TryPostToThreadPool(
                node, boost::bind(&WireConnectionBaseListener::WireConnectionClosed, l1, shared_from_this()), true);
        }
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                            "WireConnectionBase shut down");
}

MemberDefinition_Direction WireConnectionBase::Direction() { return direction; }

int32_t WireConnectionBase::GetInValueLifespan()
{
    boost::mutex::scoped_lock lock(inval_lock);
    return inval_lifespan;
}
void WireConnectionBase::SetInValueLifespan(int32_t millis)
{
    boost::mutex::scoped_lock lock(inval_lock);
    inval_lifespan = millis;
}

int32_t WireConnectionBase::GetOutValueLifespan()
{
    boost::mutex::scoped_lock lock(outval_lock);
    return outval_lifespan;
}
void WireConnectionBase::SetOutValueLifespan(int32_t millis)
{
    boost::mutex::scoped_lock lock(outval_lock);
    outval_lifespan = millis;
}

RR_INTRUSIVE_PTR<RRValue> WireBase::UnpackPacket(RR_INTRUSIVE_PTR<MessageEntry> me, TimeSpec& ts)
{

    RR_INTRUSIVE_PTR<MessageElementNestedElementList> s =
        MessageElement::FindElement(me->elements, "packettime")->CastDataToNestedList(DataTypes_structure_t);
    int64_t seconds =
        RRArrayToScalar(MessageElement::FindElement(s->Elements, "seconds")->CastData<RRArray<int64_t> >());
    int32_t nanoseconds =
        RRArrayToScalar(MessageElement::FindElement(s->Elements, "nanoseconds")->CastData<RRArray<int32_t> >());
    ts = TimeSpec(seconds, nanoseconds);

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

void WireBase::DispatchPacket(RR_INTRUSIVE_PTR<MessageEntry> me, RR_SHARED_PTR<WireConnectionBase> e)
{
    TimeSpec timespec;
    RR_INTRUSIVE_PTR<RRValue> data = UnpackPacket(me, timespec);
    e->WirePacketReceived(timespec, data);
}

RR_INTRUSIVE_PTR<MessageEntry> WireBase::PackPacket(RR_INTRUSIVE_PTR<RRValue> data, TimeSpec time)
{
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > timespec1;
    timespec1.push_back(CreateMessageElement("seconds", ScalarToRRArray(time.seconds)));
    timespec1.push_back(CreateMessageElement("nanoseconds", ScalarToRRArray(time.nanoseconds)));
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> s =
        CreateMessageElementNestedElementList(DataTypes_structure_t, "RobotRaconteur.TimeSpec", RR_MOVE(timespec1));

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

RR_SHARED_PTR<RobotRaconteurNode> WireBase::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

MemberDefinition_Direction WireBase::Direction() { return direction; }

std::string WireClientBase::GetMemberName() { return m_MemberName; }

std::string WireClientBase::GetServicePath() { return service_path; }

void WireClientBase::WirePacketReceived(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
{
    // boost::shared_lock<boost::shared_mutex> lock2(stub_lock);

    if (m->EntryType == MessageEntryType_WireClosed)
    {
        try
        {
            RR_SHARED_PTR<WireConnectionBase> c;
            {
                boost::mutex::scoped_lock lock(connection_lock);
                c = connection;
                connection.reset();
            }
            c->RemoteClose();
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                                    "Error closing wire connection: " << exp.what());
        }
    }
    else if (m->EntryType == MessageEntryType_WirePacket)
    {
        try
        {
            RR_SHARED_PTR<WireConnectionBase> c;
            {
                boost::mutex::scoped_lock lock(connection_lock);
                c = connection;
                if (!c)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                                            "Received packet for unconnected wire");
                    return;
                }
            }
            DispatchPacket(m, c);
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                                    "Error receiving and dispatching wire packet: " << exp.what());
        }
    }
}

void WireClientBase::Shutdown()
{

    try
    {
        RR_SHARED_PTR<WireConnectionBase> c;
        {
            boost::mutex::scoped_lock lock(connection_lock);
            c = connection;
        }

        try
        {
            // RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_WireDisconnectReq,
            // GetMemberName()); RR_INTRUSIVE_PTR<MessageEntry> ret = GetStub()->ProcessRequest(m);
            if (c)
                c->Shutdown();
        }
        catch (std::exception& exp)
        {
            RobotRaconteurNode::TryHandleException(node, &exp);
        }
    }
    catch (std::exception&)
    {}

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName, "WireClient shut down");
}

RR_SHARED_PTR<ServiceStub> WireClientBase::GetStub()
{
    RR_SHARED_PTR<ServiceStub> out = stub.lock();
    if (!out)
        throw InvalidOperationException("Wire connection has been closed");
    return out;
}

void WireClientBase::SendWirePacket(RR_INTRUSIVE_PTR<RRValue> packet, TimeSpec time, uint32_t endpoint)
{
    RR_INTRUSIVE_PTR<MessageEntry> m = PackPacket(packet, time);

    GetStub()->SendWireMessage(m);
}

void WireClientBase::AsyncClose(RR_SHARED_PTR<WireConnectionBase> endpoint, bool remote, uint32_t ee,
                                RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler,
                                int32_t timeout)
{

    {
        boost::mutex::scoped_lock lock(connection_lock);
        if (!remote)
        {
            RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_WireDisconnectReq, GetMemberName());
            GetStub()->AsyncProcessRequest(m, boost::bind(handler, RR_BOOST_PLACEHOLDERS(_2)), timeout);
        }
        connection.reset();
    }
}

void WireClientBase::AsyncConnect_internal(
    RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<WireConnectionBase>, RR_SHARED_PTR<RobotRaconteurException>)>)
        handler,
    int32_t timeout)
{

    {
        boost::mutex::scoped_lock lock(connection_lock);
        try
        {
            if (connection != 0)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                                        "Wire already connected");
                throw InvalidOperationException("Already connected");
            }

            RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_WireConnectReq, GetMemberName());
            GetStub()->AsyncProcessRequest(m,
                                           boost::bind(&WireClientBase::AsyncConnect_internal1,
                                                       RR_DYNAMIC_POINTER_CAST<WireClientBase>(shared_from_this()),
                                                       RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), handler),
                                           timeout);

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                                    "Begin connect wire connection");
        }
        catch (std::exception& e)
        {
            connection.reset();
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                                    "Wire connect failed: " << e.what());
            throw e;
        }
    }
}

void WireClientBase::AsyncConnect_internal1(
    RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err,
    boost::function<void(RR_SHARED_PTR<WireConnectionBase>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
{
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                                "Connecting wire failed: " << err->what());
        detail::InvokeHandlerWithException(node, handler, err);
        return;
    }

    try
    {
        {
            boost::mutex::scoped_lock lock(connection_lock);
            if (connection)
            {

                detail::InvokeHandlerWithException(node, handler,
                                                   RR_MAKE_SHARED<ServiceException>("Wire already connected"));

                return;
            }
            connection = CreateNewWireConnection(direction);
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName, "Wire connected");

        detail::InvokeHandler(node, handler, connection);
    }
    catch (std::exception& err2)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                                "Connecting wire failed: " << err2.what());
        detail::InvokeHandlerWithException(node, handler, err2);
    }
}

std::string WireServerBase::GetMemberName() { return m_MemberName; }

std::string WireServerBase::GetServicePath() { return service_path; }

WireClientBase::WireClientBase(boost::string_ref name, RR_SHARED_PTR<ServiceStub> stub,
                               MemberDefinition_Direction direction)
{
    this->stub = stub;
    this->m_MemberName = RR_MOVE(name.to_string());
    this->node = stub->RRGetNode();
    this->direction = direction;
    this->service_path = stub->ServicePath;
    this->endpoint = stub->GetContext()->GetLocalEndpoint();

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName, "WireClient created");
}

RR_INTRUSIVE_PTR<RRValue> WireClientBase::PeekInValueBase(TimeSpec& ts)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                            "Requesting PeekInValue");
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m =
        CreateMessageEntry(MessageEntryType_WirePeekInValueReq, GetMemberName());
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr = GetStub()->ProcessRequest(m);
    return UnpackPacket(mr, ts);
}

RR_INTRUSIVE_PTR<RRValue> WireClientBase::PeekOutValueBase(TimeSpec& ts)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                            "Requesting PeekOutValue");
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m =
        CreateMessageEntry(MessageEntryType_WirePeekOutValueReq, GetMemberName());
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr = GetStub()->ProcessRequest(m);
    return UnpackPacket(mr, ts);
}

void WireClientBase::PokeOutValueBase(RR_INTRUSIVE_PTR<RRValue> value)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                            "Requesting PokeOutValue");
    RR_INTRUSIVE_PTR<MessageEntry> m = PackPacket(value, GetNode()->NowTimeSpec());
    m->EntryType = MessageEntryType_WirePokeOutValueReq;
    m->MetaData.reset();
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr = GetStub()->ProcessRequest(m);
}

void WireClientBase::AsyncPeekValueBaseEnd1(RR_INTRUSIVE_PTR<MessageEntry> m,
                                            RR_SHARED_PTR<RobotRaconteurException> err,
                                            boost::function<void(const RR_INTRUSIVE_PTR<RRValue>&, const TimeSpec&,
                                                                 RR_SHARED_PTR<RobotRaconteurException>)>& handler)
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

void WireClientBase::AsyncPeekInValueBase(
    RR_MOVE_ARG(boost::function<void(const RR_INTRUSIVE_PTR<RRValue>&, const TimeSpec&,
                                     RR_SHARED_PTR<RobotRaconteurException>)>) handler,
    int32_t timeout)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                            "Requesting PeekInValue");
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m =
        CreateMessageEntry(MessageEntryType_WirePeekInValueReq, GetMemberName());
    GetStub()->AsyncProcessRequest(m,
                                   boost::bind(&WireClientBase::AsyncPeekValueBaseEnd1,
                                               RR_DYNAMIC_POINTER_CAST<WireClientBase>(shared_from_this()),
                                               RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), handler),
                                   timeout);
}

void WireClientBase::AsyncPeekOutValueBase(
    RR_MOVE_ARG(boost::function<void(const RR_INTRUSIVE_PTR<RRValue>&, const TimeSpec&,
                                     RR_SHARED_PTR<RobotRaconteurException>)>) handler,
    int32_t timeout)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                            "Requesting PeekOutValue");
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m =
        CreateMessageEntry(MessageEntryType_WirePeekOutValueReq, GetMemberName());
    GetStub()->AsyncProcessRequest(m,
                                   boost::bind(&WireClientBase::AsyncPeekValueBaseEnd1,
                                               RR_DYNAMIC_POINTER_CAST<WireClientBase>(shared_from_this()),
                                               RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), handler),
                                   timeout);
}

void WireClientBase_AsyncPokeValueBaseEnd(
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err,
    boost::function<void(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> handler)
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

void WireClientBase::AsyncPokeOutValueBase(const RR_INTRUSIVE_PTR<RRValue>& value,
                                           RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>)
                                               handler,
                                           int32_t timeout)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                            "Requesting PokeOutValue");
    RR_INTRUSIVE_PTR<MessageEntry> m = PackPacket(value, GetNode()->NowTimeSpec());
    m->EntryType = MessageEntryType_WirePokeOutValueReq;
    m->MetaData.reset();
    GetStub()->AsyncProcessRequest(m,
                                   boost::bind(&WireClientBase_AsyncPokeValueBaseEnd, RR_BOOST_PLACEHOLDERS(_1),
                                               RR_BOOST_PLACEHOLDERS(_2), handler),
                                   timeout);
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
                if (e1 == connections.end())
                    return;
                c = e1->second;
            }
            DispatchPacket(m, c);
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, e, service_path, m_MemberName,
                                                    "Error receiving and dispatching wire packet: " << exp.what());
        }
    }
}

void WireServerBase::ClientDisconnected(RR_SHARED_PTR<ServerContext> context, ServerServiceListenerEventType ev,
                                        RR_SHARED_PTR<void> param)
{

    if (ev == ServerServiceListenerEventType_ClientDisconnected)
    {
        uint32_t ep = *RR_STATIC_POINTER_CAST<uint32_t>(param);

        std::vector<RR_SHARED_PTR<WireConnectionBase> > c;
        {
            boost::mutex::scoped_lock lock(connections_lock);
            for (RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<WireConnectionBase> >::iterator ee = connections.begin();
                 ee != connections.end();)
            {
                if (ee->first == ep)
                {
                    // ee->second->RemoteClose();
                    c.push_back(ee->second);
                    ee = connections.erase(ee);
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, ep, service_path, m_MemberName,
                                                            "Client disconected, closing connection");
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
        {}

        try
        {
            e->Shutdown();
        }
        catch (std::exception& exp)
        {
            RobotRaconteurNode::TryHandleException(node, &exp);
        }
    }

    // skel.reset();;

    listener_connection.disconnect();

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, -1, service_path, m_MemberName, "WireServer shut down");
}

RR_SHARED_PTR<ServiceSkel> WireServerBase::GetSkel()
{
    RR_SHARED_PTR<ServiceSkel> out = skel.lock();
    if (!out)
        throw InvalidOperationException("Wire connection has been closed");
    return out;
}

void WireServerBase::SendWirePacket(RR_INTRUSIVE_PTR<RRValue> packet, TimeSpec time, uint32_t e)
{
    {
        boost::mutex::scoped_lock lock(connections_lock);
        if (connections.find(e) == connections.end())
            throw InvalidOperationException("Wire has been disconnected");
    }
    RR_INTRUSIVE_PTR<MessageEntry> m = PackPacket(packet, time);

    GetSkel()->SendWireMessage(m, e);
}

void WireServerBase::AsyncClose(RR_SHARED_PTR<WireConnectionBase> c, bool remote, uint32_t ee,
                                RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler,
                                int32_t timeout)
{

    if (!remote)
    {
        RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_WireClosed, GetMemberName());

        GetSkel()->SendWireMessage(m, ee);
    }

    try
    {

        {
            boost::mutex::scoped_lock lock(connections_lock);
            if (connections.find(c->GetEndpoint()) != connections.end())
                connections.erase(c->GetEndpoint());
        }
    }
    catch (std::exception&)
    {}

    detail::PostHandler(node, handler, true);
}

void wire_server_client_disconnected(RR_SHARED_PTR<ServerContext> context, ServerServiceListenerEventType ev,
                                     RR_SHARED_PTR<void> param, RR_WEAK_PTR<WireServerBase> w)
{
    RR_SHARED_PTR<WireServerBase> p = w.lock();
    if (!p)
        return;
    p->ClientDisconnected(context, ev, param);
}

RR_INTRUSIVE_PTR<MessageEntry> WireServerBase::WireCommand(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
{

    {
        boost::mutex::scoped_lock lock(connections_lock);
        switch (m->EntryType)
        {
        case MessageEntryType_WireConnectReq: {
            if (!init)
            {
                RR_WEAK_PTR<WireServerBase> weak = RR_DYNAMIC_POINTER_CAST<WireServerBase>(shared_from_this());
                listener_connection = GetSkel()->GetContext()->ServerServiceListener.connect(
                    boost::bind(&wire_server_client_disconnected, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2),
                                RR_BOOST_PLACEHOLDERS(_3), weak));
                init = true;
            }

            try
            {

                if (connections.find(e) == connections.end())
                {
                    // Switch connection direction since this is the server
                    MemberDefinition_Direction ep_direction = direction;
                    if (direction == MemberDefinition_Direction_readonly)
                        ep_direction = MemberDefinition_Direction_writeonly;
                    if (direction == MemberDefinition_Direction_writeonly)
                        ep_direction = MemberDefinition_Direction_readonly;

                    connections.insert(std::make_pair(e, CreateNewWireConnection(e, ep_direction)));
                }
                RR_SHARED_PTR<WireConnectionBase> con = connections.at(e);
                lock.unlock();
                fire_WireConnectCallback(con);

                RR_INTRUSIVE_PTR<MessageEntry> ret =
                    CreateMessageEntry(MessageEntryType_WireConnectRet, GetMemberName());

                ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, e, service_path, m_MemberName, "Wire connected");
                return ret;
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, e, service_path, m_MemberName,
                                                        "Connecting wire connection failed: " << exp.what());
                throw;
            }
        }
        case MessageEntryType_WireDisconnectReq: {
            try
            {
                RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<WireConnectionBase> >::iterator e1 = connections.find(e);
                if (e1 == connections.end())
                    throw ServiceException("Invalid wire connection");
                RR_SHARED_PTR<WireConnectionBase> c = e1->second;
                lock.unlock();
                c->RemoteClose();
                // connections.erase(e);
                return CreateMessageEntry(MessageEntryType_WireDisconnectRet, GetMemberName());
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, e, service_path, m_MemberName,
                                                        "Disconnecting wire failed: " << exp.what());
                throw;
            }
        }
        case MessageEntryType_WirePeekInValueReq: {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, e, service_path, m_MemberName,
                                                    "Wire PeekInValue requested");
            try
            {
                if (direction == MemberDefinition_Direction_writeonly)
                    throw WriteOnlyMemberException("Write only member");
                lock.unlock();
                RR_INTRUSIVE_PTR<RRValue> value = do_PeekInValue(e);
                RR_INTRUSIVE_PTR<MessageEntry> mr = PackPacket(value, GetNode()->NowTimeSpec());
                mr->EntryType = MessageEntryType_WirePeekInValueRet;
                mr->MetaData.reset();
                return mr;
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, e, service_path, m_MemberName,
                                                        "Wire PeekInValue failed: " << exp.what());
                throw;
            }
        }
        case MessageEntryType_WirePeekOutValueReq: {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, e, service_path, m_MemberName,
                                                    "Wire PeekOutValue requested");
            try
            {
                if (direction == MemberDefinition_Direction_readonly)
                    throw ReadOnlyMemberException("Read only member");
                lock.unlock();
                RR_INTRUSIVE_PTR<RRValue> value = do_PeekOutValue(e);
                RR_INTRUSIVE_PTR<MessageEntry> mr = PackPacket(value, GetNode()->NowTimeSpec());
                mr->EntryType = MessageEntryType_WirePeekOutValueRet;
                mr->MetaData.reset();
                return mr;
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, e, service_path, m_MemberName,
                                                        "Wire PeekOutValue failed: " << exp.what());
                throw;
            }
        }
        case MessageEntryType_WirePokeOutValueReq: {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, e, service_path, m_MemberName,
                                                    "Wire PokeOutValue requested");
            try
            {
                if (direction == MemberDefinition_Direction_readonly)
                    throw ReadOnlyMemberException("Read only member");
                TimeSpec ts;
                RR_INTRUSIVE_PTR<RRValue> value1 = UnpackPacket(m, ts);
                lock.unlock();
                do_PokeOutValue(value1, ts, e);
                return CreateMessageEntry(MessageEntryType_WirePokeOutValueRet, GetMemberName());
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, e, service_path, m_MemberName,
                                                        "Wire PokeOutValue failed: " << exp.what());
                throw;
            }
        }

        default: {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, e, service_path, m_MemberName,
                                                    "Received invalid wire command");
            throw InvalidOperationException("Invalid Command");
        }
        }
    }
}

WireServerBase::WireServerBase(boost::string_ref name, RR_SHARED_PTR<ServiceSkel> skel,
                               MemberDefinition_Direction direction)
{
    this->skel = skel;
    this->m_MemberName = RR_MOVE(name.to_string());
    this->init = false;
    this->node = skel->RRGetNode();
    this->direction = direction;
    this->service_path = skel->GetServicePath();

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, -1, service_path, m_MemberName, "WireServer created");
}

// WireBroadcasterBase
namespace detail
{
class WireBroadcaster_connected_connection
{
  public:
    WireBroadcaster_connected_connection(RR_SHARED_PTR<WireConnectionBase> connection)
    {
        this->connection = connection;
    }

    RR_WEAK_PTR<WireConnectionBase> connection;
};
} // namespace detail

WireBroadcasterBase::~WireBroadcasterBase() {}

WireBroadcasterBase::WireBroadcasterBase()
{
    copy_element = false;
    out_value_lifespan = -1;
}

void WireBroadcasterBase::InitBase(RR_SHARED_PTR<WireBase> wire)
{
    RR_SHARED_PTR<WireServerBase> wire1 = RR_DYNAMIC_POINTER_CAST<WireServerBase>(wire);
    if (!wire1)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, -1, service_path, member_name,
                                                "WireBroadcaster init must be passed a WireServer");
        throw InvalidArgumentException("Wire must be a WireServer for WireBroadcaster");
    }

    this->wire = wire1;
    this->node = wire->GetNode();
    this->service_path = wire1->GetServicePath();
    this->member_name = wire1->GetMemberName();

    AttachWireServerEvents(wire1);

    wire1->GetSkel()->GetContext()->ServerServiceListener.connect(
        boost::signals2::signal<void(RR_SHARED_PTR<ServerContext>, ServerServiceListenerEventType,
                                     RR_SHARED_PTR<void>)>::slot_type(boost::bind(&WireBroadcasterBase::ServiceEvent,
                                                                                  this, RR_BOOST_PLACEHOLDERS(_2)))
            .track(shared_from_this()));

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, -1, service_path, member_name, "WireBroadcaster initialized");
}

void WireBroadcasterBase::ServiceEvent(ServerServiceListenerEventType evt)
{
    if (evt != ServerServiceListenerEventType_ServiceClosed)
        return;
    boost::mutex::scoped_lock lock(connected_wires_lock);
    predicate.clear();
}

void WireBroadcasterBase::ConnectionClosedBase(RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> ep)
{
    boost::mutex::scoped_lock lock(connected_wires_lock);
    uint32_t endpoint = 0;
    try
    {
        RR_SHARED_PTR<WireConnectionBase> ep1 = ep->connection.lock();
        if (ep1)
        {
            endpoint = ep1->GetEndpoint();
        }
    }
    catch (std::exception&)
    {}
    try
    {
        connected_wires.remove(ep);
    }
    catch (std::exception&)
    {}

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, member_name,
                                            "WireBroadcaster wire connection closed");
}

void WireBroadcasterBase::ConnectionConnectedBase(RR_SHARED_PTR<WireConnectionBase> ep)
{
    boost::mutex::scoped_lock lock(connected_wires_lock);
    ep->SetIgnoreInValue(true);

    RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> c =
        RR_MAKE_SHARED<detail::WireBroadcaster_connected_connection>(ep);

    AttachWireConnectionEvents(ep, c);

    if (out_value_valid &&
        !detail::WireConnectionBase_IsValueExpired(node, out_value_lasttime_local, out_value_lifespan))
    {
        ep->SetOutValueBase(out_value);
    }

    connected_wires.push_back(c);

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, ep->GetEndpoint(), service_path, member_name,
                                            "WireBroadcaster wire connected");
}

void WireBroadcasterBase::SetOutValueBase(RR_INTRUSIVE_PTR<RRValue> value)
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    boost::mutex::scoped_lock lock(connected_wires_lock);

    out_value = value;
    out_value_valid.data() = true;
    if (n)
    {
        out_value_lasttime_local = n->NowNodeTime();
    }

    RR_SHARED_PTR<WireBroadcasterBase> this_ = shared_from_this();

    for (std::list<RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> >::iterator ee = connected_wires.begin();
         ee != connected_wires.end();)
    {
        uint32_t ep_endpoint = 0;
        try
        {
            RR_SHARED_PTR<WireConnectionBase> c = (*ee)->connection.lock();
            if (!c)
            {
                ee = connected_wires.erase(ee);
                continue;
            }
            else
            {
                ep_endpoint = c->GetEndpoint();
                if (predicate)
                {
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(
                        node, Member, ep_endpoint, service_path, member_name,
                        "WireBroadcaster skipping send out value: predicate is false");
                    if (!predicate(this_, c->GetEndpoint()))
                    {
                        ee++;
                        continue;
                    }
                }

                ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, ep_endpoint, service_path, member_name,
                                                        "WireBroadcaster sending out value");
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
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, ep_endpoint, service_path, member_name,
                                                    "WireBroadcaster failed sending out value: " << exp.what());
            ee = connected_wires.erase(ee);
        }
    }
}

size_t WireBroadcasterBase::GetActiveWireConnectionCount()
{
    boost::mutex::scoped_lock lock(connected_wires_lock);
    return connected_wires.size();
}

void WireBroadcasterBase::AttachWireServerEvents(RR_SHARED_PTR<WireServerBase> p) {}

void WireBroadcasterBase::AttachWireConnectionEvents(RR_SHARED_PTR<WireConnectionBase> p,
                                                     RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> cep)
{}

boost::function<bool(RR_SHARED_PTR<WireBroadcasterBase>&, uint32_t)> WireBroadcasterBase::GetPredicate()
{
    boost::mutex::scoped_lock lock(connected_wires_lock);
    return predicate;
}
void WireBroadcasterBase::SetPredicate(boost::function<bool(RR_SHARED_PTR<WireBroadcasterBase>&, uint32_t)> f)
{
    boost::mutex::scoped_lock lock(connected_wires_lock);
    predicate = f;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, -1, service_path, member_name,
                                            "WireBroadcaster predicate set");
}

RR_INTRUSIVE_PTR<RRValue> WireBroadcasterBase::ClientPeekInValueBase()
{
    boost::mutex::scoped_lock lock(connected_wires_lock);
    if (!out_value_valid)
        throw ValueNotSetException("Value not set");
    if (detail::WireConnectionBase_IsValueExpired(node, out_value_lasttime_local, out_value_lifespan))
    {
        throw ValueNotSetException("Value expired");
    }
    if (!copy_element)
    {
        RR_INTRUSIVE_PTR<RRValue> out_value1 = out_value;
        return out_value1;
    }
    else
    {
        return ShallowCopyMessageElement(rr_cast<MessageElement>(out_value));
    }
}

RR_SHARED_PTR<WireBase> WireBroadcasterBase::GetWireBase()
{
    RR_SHARED_PTR<WireBase> wire1 = wire.lock();
    if (!wire1)
        throw InvalidOperationException("Wire released");
    return wire1;
}

int32_t WireBroadcasterBase::GetOutValueLifespan()
{
    boost::mutex::scoped_lock lock(connected_wires_lock);
    return out_value_lifespan;
}

void WireBroadcasterBase::SetOutValueLifespan(int32_t millis)
{
    boost::mutex::scoped_lock lock(connected_wires_lock);
    out_value_lifespan = millis;
}

void SetOutValueLifespan(int32_t millis);

} // namespace RobotRaconteur