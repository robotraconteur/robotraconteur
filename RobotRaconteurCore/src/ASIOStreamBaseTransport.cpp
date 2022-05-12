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

#include "RobotRaconteur/ASIOStreamBaseTransport.h"
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/IOUtils.h"
#include "RobotRaconteur/StringTable.h"

#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>

using namespace boost::asio;
using namespace RobotRaconteur;

using namespace std;
using namespace boost::asio::ip;
using namespace boost;

namespace RobotRaconteur
{
namespace detail
{
RR_SHARED_PTR<RobotRaconteurNode> ASIOStreamBaseTransport::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

ASIOStreamBaseTransport::ASIOStreamBaseTransport(const RR_SHARED_PTR<RobotRaconteurNode>& node)
    : _io_context(node->GetThreadPool()->get_io_context()), send_version4(false), use_string_table4(false),
      connected(true) RR_MEMBER_ARRAY_INIT(streammagic)
{

    send_message_size = 0;
    recv_message_size = 0;

    sendbuf_len = 4096;
    sendbuf = boost::shared_array<uint8_t>(new uint8_t[sendbuf_len]);
    sending = false;

    recvbuf_len = 4096;
    recvbuf_pos = 0;
    recvbuf_end = 0;
    recvbuf = boost::shared_array<uint8_t>(new uint8_t[recvbuf_len]);

    tlastrecv.store(node->NowNodeTime());
    tlastsend.store(node->NowNodeTime());

    this->ReceiveTimeout = 15000;
    this->SendHeartbeat = true;
    this->HeartbeatPeriod = 1000;

    streamop_waiting = false;
    streamop_closed = false;
    CheckStreamCapability_closed = false;
    CheckStreamCapability_waiting = false;
    this->node = node;
    recv_paused = false;
    send_paused = false;
    recv_pause_request = false;
    send_pause_request = false;

    receiving = false;
    max_message_size = 12 * 1024 * 1024;

    send_large_transfer_authorized = false;
    recv_large_transfer_authorized = false;

    // cout << "New stream" << endl;
    string_table_4_requestid = 0;

    server = false;
    string_table_4_pause_updates = false;
    string_table_4_closed = false;

    disable_message4 = false;
    disable_string_table = false;
    disable_async_io = false;

    async_reader = AsyncMessageReader::CreateInstance();

    async_recv_size = 0;
    async_recv_version = 0;
    async_recv_pos = 0;
    async_recv_continue_buf_count = 0;

    async_writer = AsyncMessageWriter::CreateInstance();
    async_send_version = 0;

    active_capabilities_message2_basic = 0;
    active_capabilities_message4_basic = 0;
    active_capabilities_message4_stringtable = 0;
}

void ASIOStreamBaseTransport::AsyncAttachStream(
    bool server, const NodeID& target_nodeid, boost::string_ref target_nodename,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Begin AsyncAttachStream");

    string_table4 = RR_MAKE_SHARED<detail::StringTable>(server);

    try
    {
        heartbeat_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(new boost::asio::deadline_timer(_io_context));
        {
            boost::mutex::scoped_lock lock(recv_lock);
            BeginReceiveMessage1();
        }

        // std::cout << "AsyncAttachStream" << std::endl;

        this->server = server;

        if (!server)
        {
            RR_SHARED_PTR<AsyncAttachStream_args> args =
                RR_MAKE_SHARED<AsyncAttachStream_args>(target_nodeid, target_nodename);

            boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)> cb =
                boost::bind(&ASIOStreamBaseTransport::AsyncAttachStream1,
                            RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), callback);
            AsyncStreamOp("CreateConnection", args, cb);
        }
        else
        {
            try
            {
                {
                    boost::mutex::scoped_lock lock(heartbeat_timer_lock);
                    if (heartbeat_timer)
                    {
                        heartbeat_timer->expires_from_now(boost::posix_time::milliseconds(400));
                        heartbeat_timer->async_wait(
                            boost::bind(&ASIOStreamBaseTransport::heartbeat_timer_func,
                                        RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
                                        boost::asio::placeholders::error));
                    }
                }
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "Completed AsyncAttachStream as server");
                detail::PostHandler(node, callback, true);
            }
            catch (std::exception& e)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "AsyncAttachStream failed: " << e.what());
                detail::PostHandlerWithException(node, callback, e, MessageErrorType_ConnectionError, true);
            }
        }
    }
    catch (std::exception& e)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "AsyncAttachStream failed: " << e.what());
        detail::InvokeHandlerWithException(node, callback, e, MessageErrorType_ConnectionError);
    }
}

void ASIOStreamBaseTransport::AsyncAttachStream1(
    const RR_SHARED_PTR<RRObject>& parameter, const RR_SHARED_PTR<RobotRaconteurException>& err,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    // std::cout << "AsyncAttachStream1" << std::endl;
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "AsyncAttachStream ConnectStream failed: " << err->what());
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&ASIOStreamBaseTransport::Close, shared_from_this()));

        detail::PostHandlerWithException(node, (callback), err, true);
        return;
    }
    try
    {
        if (!parameter)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "AsyncAttachStream ConnectStream did not return parameter");
            detail::PostHandlerWithException(node, (callback), RR_MAKE_SHARED<ConnectionException>("IO error"), true);
            return;
        }

        {
            boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
            NodeID RemoteNodeID1 = *rr_cast<NodeID>(parameter);
            if (RemoteNodeID.IsAnyNode())
            {
                this->RemoteNodeID = RemoteNodeID1;
            }
            else
            {
                if (RemoteNodeID != RemoteNodeID1)
                {
                    lock.unlock();
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                        node, Transport, this->GetLocalEndpoint(),
                        "AsyncAttachStream ConnectStream returned unexpected server NodeID");
                    detail::PostHandlerWithException(
                        node, (callback), RR_MAKE_SHARED<ConnectionException>("Invalid server NodeID"), true);
                    return;
                }
            }
        }

        {
            boost::mutex::scoped_lock lock(heartbeat_timer_lock);
            if (heartbeat_timer)
            {
                heartbeat_timer->expires_from_now(boost::posix_time::milliseconds(400));
                heartbeat_timer->async_wait(
                    boost::bind(&ASIOStreamBaseTransport::heartbeat_timer_func,
                                RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
                                boost::asio::placeholders::error));
            }
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Completed AsyncAttachStream as client");
        detail::PostHandler(node, callback, true);
    }
    catch (std::exception& e)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "AsyncAttachStream1 failed: " << e.what());
        detail::InvokeHandlerWithException(node, callback, e, MessageErrorType_ConnectionError);
    }
}

void ASIOStreamBaseTransport::AsyncSendMessage(
    const RR_INTRUSIVE_PTR<Message>& m,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{

    if (!connected.load())
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Attempt to send message to disconnected transport");
        throw ConnectionException("Transport not connected");
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Begin sending message");

    // Clear flags that should not be sent from the node layer
    // if (m->header->MessageFlags & MessageFlags_TRANSPORT_SPECIFIC) m->header->TransportSpecific.clear();
    // m->header->MessageFlags &= ~(MessageFlags_SUBSTREAM_SEQUENCE_NUMBER | MessageFlags_TRANSPORT_SPECIFIC);
    // End clear flags

    bool send_4 = send_version4.load();

    // TODO: find more elegant solution for this
    if (m->entries.size() == 1)
    {
        uint16_t t = m->entries[0]->EntryType;
        if (t == MessageEntryType_StreamOp || t == MessageEntryType_StreamOpRet)
        {
            if (m->entries[0]->MemberName == "CreateConnection")
            {
                send_4 = false;
            }
        }
    }

    boost::mutex::scoped_lock lock(send_lock);

    size_t message_size = 0;
    if (!send_4)
    {
        message_size = m->ComputeSize();
    }
    else
    {
        message_size = m->ComputeSize4();
    }

    if (boost::numeric_cast<int32_t>(message_size) > (max_message_size - 100))
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Attempt to send message size " << message_size << " when max is "
                                                                           << max_message_size - 100);
        throw ProtocolException("Message larger than maximum message size");
    }

    if (!send_large_transfer_authorized)
    {
        if (IsLargeTransferAuthorized())
        {
            send_large_transfer_authorized = true;
        }
        else
        {
            if (message_size > 524288)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "Attempt to send large message before authorized");
                throw ProtocolException("Attempt to send large message before authorized");
            }
        }
    }

    if (sending || send_paused)
    {

        message_queue_entry e;
        e.message = m;
        e.callback = callback;

        // Look for older wire and check connection packets and delete
        // if in queue to prevent packet flooding.

        bool replaced = false;

        for (std::list<message_queue_entry>::iterator ee = send_queue.begin(); ee != send_queue.end();)
        {
            bool remove = false;

            RR_INTRUSIVE_PTR<MessageHeader> h1 = m->header;
            RR_INTRUSIVE_PTR<MessageHeader> h2 = ee->message->header;

            if (h1->ReceiverNodeName == h2->ReceiverNodeName && h1->SenderNodeName == h2->SenderNodeName &&
                h1->ReceiverNodeID == h2->ReceiverNodeID && h1->SenderNodeID == h2->SenderNodeID &&
                h1->SenderEndpoint == h2->SenderEndpoint && h1->ReceiverEndpoint == h2->ReceiverEndpoint)
            {
                if (ee->message->entries.size() == m->entries.size() && ee->message->entries.size() == 1)
                {
                    if (ee->message->entries.at(0)->EntryType == MessageEntryType_ConnectionTest &&
                        m->entries.at(0)->EntryType == MessageEntryType_ConnectionTest)
                        return;

                    if (ee->message->entries.at(0)->EntryType == MessageEntryType_WirePacket &&
                        m->entries.at(0)->EntryType == MessageEntryType_WirePacket)
                    {
                        if (ee->message->entries.at(0)->ServicePath == m->entries.at(0)->ServicePath &&
                            ee->message->entries.at(0)->MemberName == m->entries.at(0)->MemberName)
                        {
                            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                               "Dropping wire message "
                                                                   << m->entries.at(0)->ServicePath.str());
                            remove = true;
                        }
                    }
                }
            }

            if (remove)
            {
                detail::PostHandler(node, ee->callback, true, false);

                if (replaced)
                {
                    ee = send_queue.erase(ee);
                }
                else
                {
                    *ee = e;
                    replaced = true;
                    ++ee;
                }
            }
            else
            {
                ++ee;
            }
        }

        if (!replaced)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Enqueuing message");
            send_queue.push_back(e);
        }
    }
    else
    {

        BeginSendMessage(m, callback);
    }
}

void ASIOStreamBaseTransport::SimpleAsyncSendMessage(
    const RR_INTRUSIVE_PTR<Message>& m,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    if (!connected.load())
        return;
    try
    {
        AsyncSendMessage(m, callback);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Failed sending internal message: " << exp.what());
        Close();
    }
}

void ASIOStreamBaseTransport::SimpleAsyncEndSendMessage(const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Failed sending internal message: " << err->what());
        Close();
    }
}

void ASIOStreamBaseTransport::BeginSendMessage(
    const RR_INTRUSIVE_PTR<Message>& m,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Begin sending message to stream");

    size_t message_size = 0;
    bool send_4 = send_version4.load();
    // bool string_table_4_closed = use_string_table4.load();

    // Don't use version 4 for special requests

    // TODO: find more elegant solution for this
    if (m->entries.size() == 1)
    {
        uint16_t t = m->entries[0]->EntryType;
        if (t == MessageEntryType_StreamOp || t == MessageEntryType_StreamOpRet)
        {
            if (m->entries[0]->MemberName == "CreateConnection")
            {
                send_4 = false;
            }
        }
    }

    if (!send_4)
    {
        message_size = m->ComputeSize();

        if (!disable_async_io)
        {
            async_send_version = 2;
            sending = true;
            send_message_size = message_size;
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Sending message size " << message_size
                                                                       << " using message version 2 with asyncio");
            BeginSendMessage1(m, callback);
            return;
        }

        if (message_size > sendbuf_len)
        {

            sendbuf = shared_array<uint8_t>(new uint8_t[message_size]);
            sendbuf_len = message_size;
        }

        ArrayBinaryWriter w(sendbuf.get(), 0, message_size);
        m->Write(w);
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Sending message size " << message_size
                                                                   << " using message version 2 buffer");
    }
    else
    {
        if (!GetRemoteNodeID().IsAnyNode() && GetRemoteEndpoint() != 0)
        {
            if (m->header->SenderNodeID == GetNode()->NodeID() && m->header->ReceiverNodeID == GetRemoteNodeID() &&
                m->header->SenderEndpoint == GetLocalEndpoint() && m->header->ReceiverEndpoint == GetRemoteEndpoint())
            {
                if (!(m->entries.size() == 1 && m->entries[0]->EntryType < 500))
                {
                    m->header->MessageFlags &= ~(MessageFlags_ROUTING_INFO | MessageFlags_ENDPOINT_INFO);
                }
            }
        }

        message_size = m->ComputeSize4();

        if (use_string_table4.load())
        {
            this->string_table4->MessageReplaceStringsWithCodes(m);
            message_size = m->ComputeSize4();
        }

        if (!disable_async_io)
        {
            sending = true;
            send_message_size = message_size;
            async_send_version = 4;
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Sending message size " << message_size
                                                                       << " using message version 4 with asyncio");
            BeginSendMessage1(m, callback);
            return;
        }

        if (message_size > sendbuf_len)
        {

            sendbuf = shared_array<uint8_t>(new uint8_t[message_size]);
            sendbuf_len = message_size;
        }

        ArrayBinaryWriter w(sendbuf.get(), 0, message_size);
        m->Write4(w);
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Sending message size " << message_size
                                                                   << " using message version 4 buffer");
    }

    boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> f =
        boost::bind(&ASIOStreamBaseTransport::EndSendMessage, shared_from_this(), 0, RR_BOOST_PLACEHOLDERS(_1),
                    RR_BOOST_PLACEHOLDERS(_2), m, message_size, callback, sendbuf);

    const_buffers buf;
    buf.push_back(const_buffer(sendbuf.get(), message_size));

    this->async_write_some(buf, f);

    sending = true;
    send_message_size = message_size;
}

void ASIOStreamBaseTransport::BeginSendMessage1(
    const RR_INTRUSIVE_PTR<Message>& m,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{

    async_writer->Reset();
    async_writer->BeginWrite(m, async_send_version);
    mutable_buffers work_bufs;

    work_bufs.push_back(boost::asio::buffer(sendbuf.get(), sendbuf_len));
    size_t work_bufs_used = 0;

    async_send_bufs.clear();
    switch (async_send_version)
    {
    case 2:
        async_writer->Write(send_message_size, work_bufs, work_bufs_used, async_send_bufs);
        break;
    case 4:
        async_writer->Write4(send_message_size, work_bufs, work_bufs_used, async_send_bufs);
        break;
    default:
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Attempt to send invalid message version " << async_send_version);
        throw InvalidOperationException("Invalid message version");
    }

    boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h =
        boost::bind(&ASIOStreamBaseTransport::EndSendMessage2, shared_from_this(), boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred, callback);
    async_write_some(async_send_bufs, h);
}

void ASIOStreamBaseTransport::EndSendMessage2(
    const boost::system::error_code& error, size_t bytes_transferred,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    try
    {
        if (error)
        {
            Close();
            sending = false;
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Error sending message: " << error.message());
            detail::InvokeHandlerWithException(node, callback, RR_MAKE_SHARED<ConnectionException>(error.message()));
            return;
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Sent " << bytes_transferred << " bytes using asyncio");

        buffers_consume(async_send_bufs, bytes_transferred);

        if (boost::asio::buffer_size(async_send_bufs) > 0)
        {
            boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h =
                boost::bind(&ASIOStreamBaseTransport::EndSendMessage2, shared_from_this(),
                            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, callback);

            async_write_some(async_send_bufs, h);
            return;
        }

        async_send_bufs.clear();

        size_t remaining = async_writer->WriteRemaining();
        if (remaining > 0)
        {
            mutable_buffers work_bufs;

            work_bufs.push_back(boost::asio::buffer(sendbuf.get(), sendbuf_len));
            size_t work_bufs_used = 0;

            async_send_bufs.clear();
            switch (async_send_version)
            {
            case 2:
                async_writer->Write(remaining, work_bufs, work_bufs_used, async_send_bufs);
                break;
            case 4:
                async_writer->Write4(remaining, work_bufs, work_bufs_used, async_send_bufs);
                break;
            default:
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "Attempt to send invalid message version " << async_send_version);
                throw InvalidOperationException("Invalid message version");
            }

            boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h =
                boost::bind(&ASIOStreamBaseTransport::EndSendMessage2, shared_from_this(),
                            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, callback);

            async_write_some(async_send_bufs, h);

            return;
        }
    }

    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Error sending message: " << exp.what());
        Close();
        detail::InvokeHandlerWithException(
            node, callback, RR_MAKE_SHARED<ConnectionException>("Send message error: " + std::string(exp.what())));
    }

    try
    {
        EndSendMessage1();
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Error sending next message: " << exp.what());
        Close();
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Message sent");
    detail::InvokeHandler(node, callback);
}

void ASIOStreamBaseTransport::EndSendMessage(
    size_t startpos, const boost::system::error_code& error, size_t bytes_transferred,
    const RR_INTRUSIVE_PTR<Message>& m, size_t m_len,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback,
    const boost::shared_array<uint8_t>& buf)
{
    RR_UNUSED(buf);
    try
    {
        if (error)
        {
            Close();
            sending = false;
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Error sending message: " << error.message());
            detail::InvokeHandlerWithException(node, callback, RR_MAKE_SHARED<ConnectionException>(error.message()));
            return;
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Sent " << bytes_transferred << " bytes");

        if (bytes_transferred < (m_len - startpos))
        {
            size_t new_startpos = startpos + bytes_transferred;

            boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> f =
                boost::bind(&ASIOStreamBaseTransport::EndSendMessage, shared_from_this(), new_startpos,
                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), m, m_len, callback, sendbuf);

            const_buffers buf1;
            buf1.push_back(const_buffer(sendbuf.get() + new_startpos, m_len - new_startpos));

            this->async_write_some(buf1, f);
            return;
        }

        EndSendMessage1();
    }

    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Error sending message or next message: " << exp.what());
        Close();
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Message sent");
    detail::InvokeHandler(node, callback);
}

void ASIOStreamBaseTransport::EndSendMessage1()
{

    boost::mutex::scoped_lock lock(send_lock);

    tlastsend.store(GetNode()->NowNodeTime());

    bool c = connected.load();

    if (!send_queue.empty() && c && !send_pause_request)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Dequeing next message");
        message_queue_entry m = send_queue.front();
        send_queue.pop_front();
        try
        {
            BeginSendMessage(m.message, m.callback);
        }
        catch (std::exception& e)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Error sending next message " << e.what());
            detail::InvokeHandlerWithException(node, m.callback, RR_MAKE_SHARED<ConnectionException>(e.what()));
        }
    }
    else
    {
        sending = false;
        send_event.notify_all();
        if (send_pause_request && !send_paused)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Pausing send");
            send_paused = true;
            boost::system::error_code ec;
            boost::function<void(const boost::system::error_code&)> f = send_pause_request_handler;
            send_pause_request_handler.clear();
            RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(f, ec), true);
        }
    }
}

// void ASIOStreamBaseTransport::
void ASIOStreamBaseTransport::AsyncPauseSend(const boost::function<void(const boost::system::error_code&)>& handler)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Requesting send pause");
    boost::mutex::scoped_lock lock(send_lock);

    if (send_pause_request || send_paused)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Attempt to pause send when already paused");
        throw InvalidOperationException("Already pausing");
    }

    if (!sending)
    {
        send_pause_request = true;
        send_paused = true;

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Pausing send");
        boost::system::error_code ec;
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, ec));
    }
    else
    {
        send_pause_request = true;
        send_pause_request_handler = handler;
    }
}

void ASIOStreamBaseTransport::AsyncResumeSend()
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Requesting send resume");
    boost::mutex::scoped_lock lock(send_lock);
    if (!send_pause_request)
        return;
    if (send_pause_request && !send_paused)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Attempt to resume send when not paused");
        throw InvalidOperationException("Invalid operation");
    }

    send_pause_request = false;
    send_paused = false;

    bool c = connected.load();

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Send resumed");
    if (!send_queue.empty() && c && !send_pause_request && !sending)
    {
        message_queue_entry m = send_queue.front();
        send_queue.pop_front();
        try
        {
            BeginSendMessage(m.message, m.callback);
        }
        catch (std::exception& e)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Error sending message after resuming");
            detail::InvokeHandlerWithException(node, m.callback, RR_MAKE_SHARED<ConnectionException>(e.what()));
        }
    }
}

void ASIOStreamBaseTransport::BeginReceiveMessage1()
{

    receiving = true;
    if (disable_async_io)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Begin receive message");
        mutable_buffers buf;
        buf.push_back(boost::asio::buffer(streammagic, 8));
        boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h =
            boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage1,
                        RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), 0,
                        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
        async_read_some(buf, h);
    }
    else
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Begin receive message with asyncio");
        active_recv_bufs.clear();
        active_recv_bufs.push_back(boost::asio::buffer(recvbuf.get(), 16));

        recvbuf_pos = 0;
        recvbuf_end = 0;
        async_recv_size = 0;
        async_recv_pos = 0;
        async_recv_version = 2;
        async_recv_continue_buf_count = 0;

        boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h =
            boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage5,
                        RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
                        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);

        async_read_some(active_recv_bufs, h);
    }
}

void ASIOStreamBaseTransport::EndReceiveMessage1(size_t startpos, const boost::system::error_code& error,
                                                 size_t bytes_transferred)
{
    try
    {
        if (!error)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Received " << bytes_transferred << " bytes");
            if (bytes_transferred < (8 - startpos))
            {
                size_t new_startpos = startpos + bytes_transferred;

                mutable_buffers buf1;
                buf1.push_back(boost::asio::buffer(streammagic.data() + new_startpos, 8 - new_startpos));
                boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h =
                    boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage1,
                                RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), new_startpos,
                                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
                async_read_some(buf1, h);
                return;
            }

            boost::array<char, 4> magic = {};
            uint32_t size = 0;

            memcpy(magic.data(), streammagic.data(), 4);
            memcpy(&size, streammagic.data() + 4, 4);

#ifdef BOOST_BIG_ENDIAN
            std::reverse((uint8_t*)&size, ((uint8_t*)&size) + 4);
#endif

            if (std::string(magic.data(), 4) != "RRAC")
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "Received invalid magic expected {0x52,0x52,0x41,0x43} received "
                                                       << std::hex << "{0x" << (int)magic[0] << ",0x" << (int)magic[1]
                                                       << ",0x" << (int)magic[2] << ",0x" << (int)magic[3] << "}");
                throw ProtocolException("Received invalid magic");
            }

            if (size < 8)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(), "Received too small a message");
                throw ProtocolException("Received too small a message");
            }

            if (boost::numeric_cast<int32_t>(size) > (max_message_size))
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "Received too large a message " << size << " but max allowed "
                                                                                   << max_message_size);
                throw ProtocolException("Received too large a message");
            }

            if (!recv_large_transfer_authorized)
            {
                if (IsLargeTransferAuthorized())
                {
                    recv_large_transfer_authorized = true;
                }
                else
                {
                    if (size > 512 * 1024)
                    {
                        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                           "Received too large a message " << size
                                                                                           << " before authorized");
                        throw ProtocolException("Received too large a message before authorized");
                    }
                }
            }

            /*
            if (size > recvbuf_len)
            {
                delete recvbuf;
                recvbuf=new uint8_t[size];
                recvbuf_len=size;
            }

            memcpy(recvbuf,streammagic,8);

            recv_message_size=size;

            async_read(*stream,buffer(recvbuf+8, recv_message_size-8),
                boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage3,
                RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));*/

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Receiving message size " << size << " bytes");
            if (recvbuf_len < size)
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "Receive buffer too small, resizing to " << size << " bytes");
                recvbuf_len = size * 12 / 10;
                recvbuf = boost::shared_array<uint8_t>(new uint8_t[recvbuf_len]);
            }

            memcpy(recvbuf.get(), streammagic.data(), 8);

            recv_message_size = size;

            // std::cout << this->available() << std::endl;

            mutable_buffers buf2;
            buf2.push_back(buffer(recvbuf.get() + 8, size - 8));

            boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h = boost::bind(
                &ASIOStreamBaseTransport::EndReceiveMessage2,
                RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), 8,
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, size, recvbuf);

            async_read_some(buf2, h);
        }
        else
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Closing due to stream error: " << error);
            Close();
        }
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Error receiving message: " << exp.what());
        Close();
    }
}

void ASIOStreamBaseTransport::EndReceiveMessage2(size_t startpos, const boost::system::error_code& error,
                                                 size_t bytes_transferred, size_t message_size,
                                                 const boost::shared_array<uint8_t>& buf)
{
    RR_UNUSED(buf);
    try
    {
        if (!error)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Received " << bytes_transferred << " bytes");
            if (bytes_transferred < (message_size - startpos))
            {
                size_t new_startpos = startpos + bytes_transferred;

                mutable_buffers buf2;
                buf2.push_back(buffer(recvbuf.get() + new_startpos, message_size - new_startpos));

                boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h =
                    boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage2,
                                RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), new_startpos,
                                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                                message_size, recvbuf);

                async_read_some(buf2, h);
                return;
            }

            ArrayBinaryReader r(recvbuf.get(), 0, message_size);
            r.ReadString8(4);
            r.ReadNumber<uint32_t>();
            uint16_t message_version = r.ReadNumber<uint16_t>();
            r.Seek(0);

            RR_INTRUSIVE_PTR<Message> message = CreateMessage();

            if (message_version == 4)
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "Received message deserialize using message 4 buffer");
                message->Read4(r);

                bool string_table4 = use_string_table4.load();

                if (string_table4)
                {
                    this->string_table4->MessageReplaceCodesWithStrings(message);
                }

                uint16_t flags = message->header->MessageFlags;

                if (!(flags & MessageFlags_ROUTING_INFO))
                {
                    message->header->SenderNodeID = GetRemoteNodeID();
                    message->header->ReceiverNodeID = GetNode()->NodeID();
                }

                if (!(flags & MessageFlags_ENDPOINT_INFO))
                {
                    message->header->SenderEndpoint = GetRemoteEndpoint();
                    message->header->ReceiverEndpoint = GetLocalEndpoint();
                }
            }
            else
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "Received message deserialize using message 2 buffer");
                message->Read(r);
            }

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Successfully deserialized message");
            EndReceiveMessage3(message);
        }
        else
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Closing due to stream error: " << error.message());
            Close();
        }
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Error receiving message: " << exp.what());
        Close();
    }
}

void ASIOStreamBaseTransport::EndReceiveMessage3(const RR_INTRUSIVE_PTR<Message>& message)
{

    try
    {

        if (!message->entries.empty())
        {
            if (message->entries.at(0)->EntryType == MessageEntryType_ConnectionTest)
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "Received heartbeat request, sending response");
                EndReceiveMessage4();
                const RR_INTRUSIVE_PTR<Message>& m = message;

                RR_INTRUSIVE_PTR<Message> ret = CreateMessage();
                ret->header = CreateMessageHeader();
                ret->header->ReceiverNodeName = m->header->SenderNodeName;
                ret->header->SenderNodeName = GetNode()->NodeName();
                ret->header->ReceiverNodeID = m->header->SenderNodeID;
                ret->header->ReceiverEndpoint = m->header->SenderEndpoint;
                ret->header->SenderEndpoint = m->header->ReceiverEndpoint;
                ret->header->SenderNodeID = GetNode()->NodeID();

                RR_INTRUSIVE_PTR<MessageEntry> eret =
                    ret->AddEntry(MessageEntryType_ConnectionTestRet, m->entries.at(0)->MemberName);
                eret->RequestID = m->entries.at(0)->RequestID;
                eret->ServicePath = m->entries.at(0)->ServicePath;

                bool send_4 = send_version4.load();

                if (send_4)
                {
                    ret->header->MessageFlags &= ~(MessageFlags_ROUTING_INFO | MessageFlags_ENDPOINT_INFO);
                }

                RR_SHARED_PTR<ASIOStreamBaseTransport> p =
                    RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this());

                boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h = boost::bind(
                    (&ASIOStreamBaseTransport::SimpleAsyncEndSendMessage),
                    RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1));
                AsyncSendMessage(ret, h);
            }
            else if (message->entries.at(0)->EntryType == MessageEntryType_ConnectionTestRet)
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Received heartbeat response");
                EndReceiveMessage4();
            }
            else if (message->entries.at(0)->EntryType == MessageEntryType_StreamOp ||
                     message->entries.at(0)->EntryType == MessageEntryType_StreamOpRet)
            {
                StreamOpMessageReceived(message);
                EndReceiveMessage4();
            }
            else if (message->entries.at(0)->EntryType == MessageEntryType_StreamCheckCapability ||
                     message->entries.at(0)->EntryType == MessageEntryType_StreamCheckCapabilityRet)
            {
                CheckStreamCapability_MessageReceived(message);
                EndReceiveMessage4();
            }
            else
            {
                EndReceiveMessage4();
                try
                {
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                       "Dispatching received message to node");
                    MessageReceived(message);
                }
                catch (std::exception& exp)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                       "Error dispatching message: " << exp.what()
                                                                                     << ", closing transport");
                    Close();
                }
            }

            tlastrecv.store(GetNode()->NowNodeTime());
        }
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Error processing received message: " << exp.what());
    }
}

void ASIOStreamBaseTransport::EndReceiveMessage4()
{
    {
        boost::mutex::scoped_lock recv_lock;

        if (recv_pause_request)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Pausing receive");
            receiving = false;
            recv_paused = true;
            boost::system::error_code ec;

            boost::function<void(const boost::system::error_code&)> f = recv_pause_request_handler;
            recv_pause_request_handler.clear();
            RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(f, ec), true);
            return;
        }

        if (!recv_pause_request && recv_paused)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Resuming receive");
            recv_paused = false;
        }

        BeginReceiveMessage1();
    }
}

void ASIOStreamBaseTransport::EndReceiveMessage5(const boost::system::error_code& error, size_t bytes_transferred)
{
    try
    {
        if (error)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Closing due to stream error with asyncio: " << error.message());
            Close();
            return;
        }

        if (bytes_transferred == 0)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                node, Transport, GetLocalEndpoint(),
                "Closing due to zero byte receive with asyncio (connection cleanly closed) with asyncio");
            Close();
            return;
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Received " << bytes_transferred << " bytes with asyncio");

        {
            // boost::mutex::scoped_lock lock(recv_lock);

            if (async_recv_size == 0)
            {
                if (active_recv_bufs.size() != 1)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(), "Asyncio internal error");
                    throw ProtocolException("");
                }

                recvbuf_end += bytes_transferred;

                if ((recvbuf_end - recvbuf_pos) < 16)
                {

                    buffers_consume(active_recv_bufs, bytes_transferred);
                    boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h =
                        boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage5,
                                    RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
                                    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
                    async_read_some(active_recv_bufs, h);
                    return;
                }

                boost::array<char, 4> magic = {};
                uint32_t size = 0;
                uint16_t message_version = 0;

                uint8_t* recvbuf1 = recvbuf.get() + recvbuf_pos;

                memcpy(magic.data(), recvbuf1, 4);
                memcpy(&size, recvbuf1 + 4, 4);
                memcpy(&message_version, recvbuf1 + 8, 2);

#ifdef BOOST_BIG_ENDIAN
                std::reverse((uint8_t*)&size, ((uint8_t*)&size) + 4);
                std::reverse((uint8_t*)&message_version, ((uint8_t*)&message_version) + 2);
#endif

                if (std::string(magic.data(), 4) != "RRAC")
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                       "Received invalid magic expected {0x52,0x52,0x41,0x43} received "
                                                           << std::hex << "{0x" << (int)magic[0] << ",0x"
                                                           << (int)magic[1] << ",0x" << (int)magic[2] << ",0x"
                                                           << (int)magic[3] << "} with asyncio");
                    throw ProtocolException("Received invalid magic");
                }
                if (size < 16)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                       "Received too small a message with asyncio");
                    throw ProtocolException("Received too small a message");
                }

                if (boost::numeric_cast<int32_t>(size) > (max_message_size))
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                       "Received too large a message " << size << " but max allowed "
                                                                                       << max_message_size
                                                                                       << ", with asyncio");
                    throw ProtocolException("Received too large a message");
                }

                if (!recv_large_transfer_authorized)
                {
                    if (IsLargeTransferAuthorized())
                    {
                        recv_large_transfer_authorized = true;
                    }
                    else
                    {
                        if (size > 512 * 1024)
                        {
                            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                               "Received too large a message "
                                                                   << size << " before authorized, with asyncio");
                            throw ProtocolException("Received too large a message before authorization");
                        }
                    }
                }

                async_recv_size = size;
                async_recv_version = message_version;

                async_reader->Reset();
            }
            else if (async_recv_continue_buf_count == 0)
            {
                recvbuf_end += bytes_transferred;
            }

            size_t bufs_read = 0;
            mutable_buffers continue_bufs;

            AsyncMessageReader::return_type ret = AsyncMessageReader::ReadReturn_continue_buffers;

            if (async_recv_continue_buf_count == 0)
            {
                const_buffers bufs;
                bufs.push_back(boost::asio::buffer(recvbuf.get(), recvbuf_end) + recvbuf_pos);

                switch (async_recv_version)
                {
                case 2:
                    ret = async_reader->Read(bufs, bufs_read, 0, continue_bufs);
                    break;
                case 4:
                    ret = async_reader->Read4(bufs, bufs_read, 0, continue_bufs);
                    break;
                default:
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                       "Received invalid message version: " << async_recv_version);
                    throw InvalidOperationException("Received invalid message version");
                }

                if (bufs_read > recvbuf_end - recvbuf_pos)
                    throw ProtocolException("");
                recvbuf_pos += bufs_read;
                async_recv_pos += bufs_read;
                if (recvbuf_pos == recvbuf_end)
                {
                    recvbuf_pos = 0;
                    recvbuf_end = 0;
                }

                if (ret == AsyncMessageReader::ReadReturn_continue_buffers && recvbuf_end != 0)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(), "Asyncio internal error");
                    throw ProtocolException("Asyncio internal error");
                }
            }
            else
            {
                size_t continue_buffer_bytes = 0;
                mutable_buffers::iterator e = active_recv_bufs.begin();
                {
                    for (size_t i = 0; i < async_recv_continue_buf_count; i++)
                    {
                        if (e == active_recv_bufs.end())
                        {
                            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                               "Asyncio internal error");
                            throw ProtocolException("Asyncio internal error");
                        }
                        continue_buffer_bytes += boost::asio::buffer_size(*e);
                        e++;
                    }
                }

                size_t b1 = bytes_transferred;

                if (bytes_transferred > continue_buffer_bytes)
                {
                    b1 = continue_buffer_bytes;
                    recvbuf_end += bytes_transferred - continue_buffer_bytes;
                    if (recvbuf_end > recvbuf_len)
                    {
                        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                           "Asyncio internal error");
                        throw ProtocolException("Asyncio internal error");
                    }
                }

                const_buffers bufs;

                switch (async_recv_version)
                {
                case 2:
                    ret = async_reader->Read(bufs, bufs_read, b1, continue_bufs);
                    break;
                case 4:
                    ret = async_reader->Read4(bufs, bufs_read, b1, continue_bufs);
                    break;
                default:
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                       "Received invalid message version: " << async_recv_version);
                    throw ProtocolException("Received invalid message version");
                }

                if (bufs_read != 0)
                    throw ProtocolException("Asyncio internal error");

                async_recv_pos += b1;

                if (ret != AsyncMessageReader::ReadReturn_done && recvbuf_pos != recvbuf_end)
                {
                    // Read data in extra buffer if not finished
                    bufs.push_back(boost::asio::buffer(recvbuf.get(), recvbuf_end) + recvbuf_pos);

                    switch (async_recv_version)
                    {
                    case 2:
                        ret = async_reader->Read(bufs, bufs_read, 0, continue_bufs);
                        break;
                    case 4:
                        ret = async_reader->Read4(bufs, bufs_read, 0, continue_bufs);
                        break;
                    default:
                        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                           "Received invalid message version: " << async_recv_version);
                        throw InvalidOperationException("Received invalid message version");
                    }

                    if (bufs_read > recvbuf_end - recvbuf_pos)
                        throw ProtocolException("Asyncio internal error");
                    recvbuf_pos += bufs_read;
                    async_recv_pos += bufs_read;
                    if (recvbuf_pos == recvbuf_end)
                    {
                        recvbuf_pos = 0;
                        recvbuf_end = 0;
                    }
                }
            }

            // Sanity checks
            if (recvbuf_pos > recvbuf_len)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "Received message protocol error");
                throw ProtocolException("Received message protocol error");
            }
            if (async_recv_pos > async_recv_size)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "Received message protocol error");
                throw ProtocolException("Received message protocol error");
            }

            if (ret == AsyncMessageReader::ReadReturn_done)
            {
                if (async_recv_pos != async_recv_size)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                        node, Transport, GetLocalEndpoint(),
                        "Received message protocol error: message did not read all data");
                    throw ProtocolException("Message did not read all data");
                }

                async_recv_size = 0;
                async_recv_pos = 0;
                async_recv_continue_buf_count = 0;
                active_recv_bufs.clear();

                if (async_reader->MessageReady())
                {
                    RR_INTRUSIVE_PTR<Message> m = async_reader->GetNextMessage();

                    if (string_table4)
                    {
                        this->string_table4->MessageReplaceCodesWithStrings(m);
                    }

                    uint16_t flags = m->header->MessageFlags;

                    if (!(flags & MessageFlags_ROUTING_INFO))
                    {
                        m->header->SenderNodeID = GetRemoteNodeID();
                        m->header->ReceiverNodeID = GetNode()->NodeID();
                    }

                    if (!(flags & MessageFlags_ENDPOINT_INFO))
                    {
                        m->header->SenderEndpoint = GetRemoteEndpoint();
                        m->header->ReceiverEndpoint = GetLocalEndpoint();
                    }

                    if (recvbuf_pos != recvbuf_end)
                    {
                        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                           "Received message protocol error: data still in buffer");
                        throw ProtocolException("Still data in buffer");
                    }

                    // lock.unlock();
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                       "Successfully deserialized message with asyncio");
                    EndReceiveMessage3(m);

                    return;
                }
            }

            if (recvbuf_pos != recvbuf_end)
            {
                // Still some data in the buffer, process it
                size_t l1 = recvbuf_end - recvbuf_pos;
                size_t l2 = recvbuf_len - recvbuf_pos;
                if (l2 < 128)
                {
                    std::memmove(recvbuf.get(), recvbuf.get() + recvbuf_pos, l1);
                    recvbuf_pos = 0;
                }

                recvbuf_end = recvbuf_pos;

                size_t l3 = std::min(l2, async_recv_size - async_recv_pos);
                l3 = std::min(l3, static_cast<size_t>(1024));

                active_recv_bufs.clear();
                active_recv_bufs.push_back(boost::asio::buffer(recvbuf.get() + async_recv_pos, l3));

                // TODO: This post may not always be necessary
                boost::system::error_code ec1;
                RobotRaconteurNode::TryPostToThreadPool(
                    node, boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage5,
                                      RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), ec1, l1));
            }
            else
            {
                if (ret != AsyncMessageReader::ReadReturn_continue_buffers)
                {
                    recvbuf_pos = 0;
                    recvbuf_end = 0;
                    async_recv_continue_buf_count = 0;

                    size_t l1 = std::min(recvbuf_len, async_recv_size - async_recv_pos);
                    l1 = std::min(l1, static_cast<size_t>(1024));

                    active_recv_bufs.clear();
                    active_recv_bufs.push_back(boost::asio::buffer(recvbuf.get(), l1));
                    boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h =
                        boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage5,
                                    RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
                                    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
                    async_read_some(active_recv_bufs, h);
                }
                else
                {
                    recvbuf_pos = 0;
                    recvbuf_end = 0;
                    async_recv_continue_buf_count = continue_bufs.size();
                    active_recv_bufs.clear();
                    size_t l3 = async_recv_size - async_recv_pos;
                    size_t l2 = boost::asio::buffer_size(continue_bufs);
                    if (l2 > l3)
                        throw ProtocolException("");
                    size_t l1 = std::min(recvbuf_len, l3 - l2);
                    l1 = std::min(l1, static_cast<size_t>(1024));

                    boost::range::copy(continue_bufs, std::back_inserter(active_recv_bufs));
                    if (l1 > 0)
                    {
                        active_recv_bufs.push_back(boost::asio::buffer(recvbuf.get(), l1));
                    }
                    boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h =
                        boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage5,
                                    RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
                                    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
                    async_read_some(active_recv_bufs, h);
                }
            }
        }
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Error receiving message with asyncio: " << exp.what());
        Close();
    }
}

void ASIOStreamBaseTransport::AsyncPauseReceive(const boost::function<void(const boost::system::error_code&)>& handler)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Requesting receive pause");
    boost::mutex::scoped_lock lock(recv_lock);
    if (recv_pause_request || recv_paused)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Attempt to pause receive when already paused");
        throw InvalidOperationException("Already pausing");
    }

    if (!receiving)
    {
        recv_pause_request = true;
        recv_paused = true;

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Pausing receive");

        boost::system::error_code ec;
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, ec));
    }
    else
    {
        recv_pause_request = true;
        recv_pause_request_handler = handler;
    }
}

void ASIOStreamBaseTransport::AsyncResumeReceive()
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Requesting receive resume");
    boost::mutex::scoped_lock lock(recv_lock);
    if (!recv_pause_request)
        return;
    if (recv_pause_request && !recv_paused)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Attempt to resume receive when not paused");
        throw InvalidOperationException("Invalid operation");
    }

    recv_pause_request = false;
    recv_paused = false;

    bool c = connected.load();

    if (!c)
        return;

    if (receiving)
        return;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Receive resumed");
    BeginReceiveMessage1();
}

void ASIOStreamBaseTransport::Close()
{

    {
        bool c = connected.exchange(false);
        if (!c)
            return;
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Close requested");

    // std::cout << "ASIO Close" << std::endl;

    if (heartbeat_timer)
    {
        boost::mutex::scoped_lock lock(heartbeat_timer_lock);
        heartbeat_timer->cancel();
        heartbeat_timer.reset();
    }

    try
    {
        // stream->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        // stream->close();

        boost::mutex::scoped_lock lock2(send_lock);

        if (send_pause_request_handler)
        {
            boost::system::error_code ec(boost::system::errc::broken_pipe, boost::system::generic_category());
            boost::function<void(const boost::system::error_code&)> f = send_pause_request_handler;
            send_pause_request_handler.clear();
            RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(f, ec));
        }

        for (std::list<message_queue_entry>::iterator e = send_queue.begin(); e != send_queue.end();)
        {
            boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> f = e->callback;
            e = send_queue.erase(e);
            detail::PostHandlerWithException(node, f, RR_MAKE_SHARED<ConnectionException>("Transport Closed"), true,
                                             false);
        }

        send_queue.clear();
    }
    catch (std::exception&)
    {}

    try
    {
        boost::mutex::scoped_lock lock(recv_lock);
        if (recv_pause_request_handler)
        {
            boost::system::error_code ec(boost::system::errc::broken_pipe, boost::system::generic_category());
            boost::function<void(const boost::system::error_code&)> f = recv_pause_request_handler;
            recv_pause_request_handler.clear();
            RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(f, ec));
        }
    }
    catch (std::exception&)
    {}

    boost::mutex::scoped_lock lock(streamop_lock);
    boost::mutex::scoped_lock lock2(CheckStreamCapability_lock);

    CheckStreamCapability_closed = true;
    streamop_closed = true;

    if (CheckStreamCapability_timer)
        CheckStreamCapability_timer->cancel();
    if (streamop_timer)
        streamop_timer->cancel();

    streamop_timer.reset();
    CheckStreamCapability_timer.reset();

    if (streamop_waiting && streamop_callback)
        detail::PostHandlerWithException(node, streamop_callback,
                                         RR_MAKE_SHARED<ConnectionException>("Connection closed"), false, false);
    if (CheckStreamCapability_waiting && CheckStreamCapability_callback)
        RobotRaconteurNode::TryPostToThreadPool(
            node,
            boost::bind(CheckStreamCapability_callback, 0, RR_MAKE_SHARED<ConnectionException>("Connection closed")));

    streamop_waiting = false;
    CheckStreamCapability_waiting = false;

    while (!CheckStreamCapability_queue.empty())
    {
        boost::tuple<std::string,
                     boost::function<void(const uint32_t&, const RR_SHARED_PTR<RobotRaconteurException>&)> >
            d = CheckStreamCapability_queue.front();
        CheckStreamCapability_queue.pop();
        detail::PostHandlerWithException(node, d.get<1>(), RR_MAKE_SHARED<ConnectionException>("Connection closed"),
                                         false, false);
    }

    while (!streamop_queue.empty())
    {
        boost::tuple<
            std::string, const RR_SHARED_PTR<RRObject>&,
            boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)> >
            d = streamop_queue.front();
        streamop_queue.pop();
        detail::PostHandlerWithException(node, d.get<2>(), RR_MAKE_SHARED<ConnectionException>("Connection closed"),
                                         false, false);
    }
    // boost::this_thread::sleep(boost::posix_time::milliseconds(100));

    streamop_callback.clear();
    CheckStreamCapability_callback.clear();

    {
        boost::mutex::scoped_lock lock(string_table4_lock);
        string_table_4_closed = true;
        BOOST_FOREACH (RR_SHARED_PTR<boost::asio::deadline_timer>& e, string_table_4_timers)
        {
            boost::system::error_code ec;
            e->cancel(ec);
        }
        string_table_4_timers.clear();
    }

    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(), "Transport connection closed");
}

void ASIOStreamBaseTransport::heartbeat_timer_func(const boost::system::error_code& e)
{

    if (!connected.load())
        return;

    if (e != boost::asio::error::operation_aborted)
    {
        uint32_t heartbeat_period2 = HeartbeatPeriod;

        if (server)
        {
            heartbeat_period2 += (HeartbeatPeriod * 2) / 10;
        }

        try
        {
            boost::posix_time::ptime t = tlastsend.load();

            if ((GetNode()->NowNodeTime() - t).total_milliseconds() > heartbeat_period2 && SendHeartbeat)
            {
                RR_INTRUSIVE_PTR<Message> m = CreateMessage();
                m->header = CreateMessageHeader();
                m->header->SenderNodeID = GetNode()->NodeID();
                RR_INTRUSIVE_PTR<MessageEntry> mm = CreateMessageEntry(MessageEntryType_ConnectionTest, "");
                m->entries.push_back(mm);

                boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h = boost::bind(
                    &ASIOStreamBaseTransport::SimpleAsyncEndSendMessage,
                    RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1));
                AsyncSendMessage(m, h);
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "Heartbeat request sent");
            }

            boost::posix_time::ptime tr = tlastrecv.load();

            if ((t - tr).total_milliseconds() > ReceiveTimeout)
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "Transport close requested due to heartbeat timeout");
                Close();
            }
            else
            {
                boost::mutex::scoped_lock lock(heartbeat_timer_lock);
                if (!heartbeat_timer)
                    return;

                boost::posix_time::time_duration t =
                    boost::posix_time::milliseconds(heartbeat_period2 + 10) - (GetNode()->NowNodeTime() - tlastsend);

                heartbeat_timer->expires_from_now(t);
                heartbeat_timer->async_wait(
                    boost::bind(&ASIOStreamBaseTransport::heartbeat_timer_func,
                                RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
                                boost::asio::placeholders::error));
            }
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                node, Transport, GetLocalEndpoint(),
                "Transport close requested due exception in heartbeat timer: " << exp.what());
            Close();
        }
    }
}

bool ASIOStreamBaseTransport::IsConnected() { return connected.load(); }

uint32_t ASIOStreamBaseTransport::StreamCapabilities(boost::string_ref name)
{
    if (name == "com.robotraconteur.message.v_max")
    {
        return 4;
    }

    if (name == "com.robotraconteur.v2")
    {
        return 1;
    }

    if (name == "com.robotraconteur.v2.minor")
    {
        return 0;
    }

    if (name == "com.robotraconteur.v2.0")
    {
        return 1;
    }

    if (name == "com.robotraconteur.message.v4")
    {
        return 1;
    }

    if (name == "com.robotraconteur.message.v4.minor")
    {
        return 0;
    }

    if (name == "com.robotraconteur.message.v4.0")
    {
        return 1;
    }

    if (name == "com.robotraconteur.stringtable")
    {
        return 4;
    }

    if (name == "com.robotraconteur.stringtable.v4")
    {
        return 1;
    }

    if (name == "com.robotraconteur.stringtable.v4.minor")
    {
        return 0;
    }

    if (name == "com.robotraconteur.stringtable.v4.0")
    {
        return 1;
    }

    return 0;
}

void ASIOStreamBaseTransport::AsyncCheckStreamCapability(
    boost::string_ref name,
    const boost::function<void(uint32_t, const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{

    boost::mutex::scoped_lock lock(CheckStreamCapability_lock);
    if (CheckStreamCapability_closed)
    {
        RobotRaconteurNode::TryPostToThreadPool(
            node, boost::bind(callback, 0, RR_MAKE_SHARED<ConnectionException>("Connection closed")), true);
        return;
    }
    if (!CheckStreamCapability_waiting)
    {
        BeginCheckStreamCapability(name, callback);
    }
    else
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Queuing AsyncCheckStreamCapability \"" << name << "\"");
        CheckStreamCapability_queue.push(boost::make_tuple(name.to_string(), callback));
    }
}

void ASIOStreamBaseTransport::BeginCheckStreamCapability(
    boost::string_ref name,
    const boost::function<void(uint32_t, const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{

    {

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "BeginCheckStreamCapability \"" << name << "\"");
        if (CheckStreamCapability_waiting)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Attempt to call AsyncCheckStreamCapability \""
                                                   << name << "\" when already in progress");
            throw InvalidOperationException("Already checking capability");
        }
        CheckStreamCapability_waiting = true;
        RR_INTRUSIVE_PTR<Message> m = CreateMessage();
        m->header->SenderNodeID = GetNode()->NodeID();
        {
            boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
            m->header->ReceiverNodeID = RemoteNodeID;
        }
        RR_INTRUSIVE_PTR<MessageEntry> mm = CreateMessageEntry(MessageEntryType_StreamCheckCapability, name);
        m->entries.push_back(mm);

        if (CheckStreamCapability_timer)
        {
            try
            {
                CheckStreamCapability_timer->cancel();
            }
            catch (std::exception&)
            {}
        }

        CheckStreamCapability_callback = callback;

        CheckStreamCapability_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(
            new boost::asio::deadline_timer(_io_context, boost::posix_time::milliseconds(10000)));
        RR_WEAK_PTR<ASIOStreamBaseTransport> t = RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this());
        RobotRaconteurNode::asio_async_wait(node, CheckStreamCapability_timer,
                                            boost::bind(&ASIOStreamBaseTransport::CheckStreamCapability_timercallback,
                                                        t, boost::asio::placeholders::error));

        CheckStreamCapability_waiting = true;

        boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
            boost::bind(&ASIOStreamBaseTransport::SimpleAsyncEndSendMessage,
                        RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1));
        AsyncSendMessage(m, h);
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Sent CheckStreamCapability \"" << name << "\" request");

        return;
    }
}

void ASIOStreamBaseTransport::CheckStreamCapability_EndSendMessage(const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "CheckStreamCapability send message failed " << err->what());
        boost::mutex::scoped_lock lock(CheckStreamCapability_lock);
        if (CheckStreamCapability_waiting)
        {
            if (CheckStreamCapability_callback)
            {
                RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(CheckStreamCapability_callback, 0, err));
            }
            CheckStreamCapability_waiting = false;
            CheckStreamCapability_callback.clear();
            if (CheckStreamCapability_timer)
            {
                CheckStreamCapability_timer->cancel();
                CheckStreamCapability_timer.reset();
            }

            if (!CheckStreamCapability_queue.empty())
            {
                boost::tuple<std::string,
                             boost::function<void(uint32_t, const RR_SHARED_PTR<RobotRaconteurException>&)> >
                    d = CheckStreamCapability_queue.front();
                CheckStreamCapability_queue.pop();
                try
                {
                    BeginCheckStreamCapability(d.get<0>(), d.get<1>());
                }
                catch (std::exception& exp)
                {
                    detail::InvokeHandlerWithException(node, d.get<1>(), exp);
                }
            }
        }
    }
}

void ASIOStreamBaseTransport::CheckStreamCapability_timercallback(RR_WEAK_PTR<ASIOStreamBaseTransport> t,
                                                                  const boost::system::error_code& e)
{
    if (e != boost::asio::error::operation_aborted)
    {
        RR_SHARED_PTR<ASIOStreamBaseTransport> t2 = t.lock();
        if (!t2)
            return;

        boost::mutex::scoped_lock lock(t2->CheckStreamCapability_lock);
        if (t2->CheckStreamCapability_waiting)
        {

            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(t2->node, Transport, t2->GetLocalEndpoint(),
                                               "CheckStreamCapability timed out");
            if (t2->CheckStreamCapability_callback)
            {
                RobotRaconteurNode::TryPostToThreadPool(
                    t2->node, boost::bind(t2->CheckStreamCapability_callback, 0,
                                          RR_MAKE_SHARED<RequestTimeoutException>("Timed out")));
            }
            t2->CheckStreamCapability_waiting = false;
            t2->CheckStreamCapability_callback.clear();
            t2->CheckStreamCapability_timer.reset();

            while (!t2->CheckStreamCapability_queue.empty())
            {
                boost::tuple<std::string,
                             boost::function<void(const uint32_t&, const RR_SHARED_PTR<RobotRaconteurException>&)> >
                    d = t2->CheckStreamCapability_queue.front();
                t2->CheckStreamCapability_queue.pop();
                // t2->BeginCheckStreamCapability(d.get<0>(),d.get<1>());
                detail::PostHandlerWithException(t2->node, d.get<1>(),
                                                 RR_MAKE_SHARED<RequestTimeoutException>("Timed out"));
            }
        }
    }
}

void ASIOStreamBaseTransport::CheckStreamCapability_MessageReceived(const RR_INTRUSIVE_PTR<Message>& m)
{
    try
    {
        if (m->entries.at(0)->EntryType == MessageEntryType_StreamCheckCapability)
        {
            RR_INTRUSIVE_PTR<Message> ret = CreateMessage();
            ret->header = CreateMessageHeader();
            ret->header->SenderNodeID = GetNode()->NodeID();
            ret->header->ReceiverNodeID = m->header->SenderNodeID;
            RR_INTRUSIVE_PTR<MessageEntry> mret =
                CreateMessageEntry(MessageEntryType_StreamCheckCapabilityRet, m->entries.at(0)->MemberName);
            mret->ServicePath = m->entries.at(0)->ServicePath;
            mret->AddElement("return", ScalarToRRArray(StreamCapabilities(m->entries.at(0)->MemberName.str())));
            ret->entries.push_back(mret);

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Received CheckStreamCapability \"" << m->entries.at(0)->MemberName
                                                                                   << "\" request, sending response");

            boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h = boost::bind(
                &ASIOStreamBaseTransport::SimpleAsyncEndSendMessage,
                RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1));
            AsyncSendMessage(ret, h);
        }
        else if (m->entries.at(0)->EntryType == MessageEntryType_StreamCheckCapabilityRet)
        {
            boost::mutex::scoped_lock lock(CheckStreamCapability_lock);
            if (CheckStreamCapability_waiting)
            {
                if (CheckStreamCapability_callback)
                {
                    uint32_t cap_level =
                        RRArrayToScalar(m->entries.at(0)->FindElement("return")->CastData<RRArray<uint32_t> >());
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                       "Received CheckStreamCapability \""
                                                           << m->entries.at(0)->MemberName
                                                           << "\" response: " << cap_level);
                    RobotRaconteurNode::TryPostToThreadPool(node,
                                                            boost::bind(CheckStreamCapability_callback, cap_level,
                                                                        RR_SHARED_PTR<RobotRaconteurException>()));
                }
                CheckStreamCapability_waiting = false;
                CheckStreamCapability_callback.clear();
                if (CheckStreamCapability_timer)
                {
                    CheckStreamCapability_timer->cancel();
                    CheckStreamCapability_timer.reset();
                }

                if (!CheckStreamCapability_queue.empty())
                {
                    boost::tuple<std::string,
                                 boost::function<void(uint32_t, const RR_SHARED_PTR<RobotRaconteurException>&)> >
                        d = CheckStreamCapability_queue.front();
                    CheckStreamCapability_queue.pop();
                    try
                    {
                        BeginCheckStreamCapability(d.get<0>(), d.get<1>());
                    }
                    catch (std::exception& exp)
                    {
                        detail::PostHandlerWithException(node, d.get<1>(), exp);
                    }
                }
            }
        }
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Receiving CheckStreamCapability message failed: " << exp.what());
    };
}

void ASIOStreamBaseTransport::AsyncStreamOp(
    boost::string_ref command, const RR_SHARED_PTR<RRObject>& args,
    boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    boost::mutex::scoped_lock lock(streamop_lock);

    if (streamop_closed)
    {
        detail::PostHandlerWithException(node, callback, RR_MAKE_SHARED<ConnectionException>("Connection closed"), true,
                                         false);
        return;
    }

    if (!streamop_waiting)
    {
        BeginStreamOp(command, args, callback);
    }
    else
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Queuing AsyncStreamOp \"" << command << "\"");
        streamop_queue.push(boost::make_tuple(command.to_string(), args, callback));
    }
}

static std::string ASIOStreamBaseTransport_log_caps(const std::vector<uint32_t>& caps)
{
    std::stringstream ss;
    for (size_t i = 0;;)
    {
        ss << "0x" << std::hex << caps[i];
        ++i;
        if (i < caps.size())
        {
            ss << ", ";
        }
        else
        {
            break;
        }
    }
    return ss.str();
}

void ASIOStreamBaseTransport::BeginStreamOp(
    boost::string_ref command, const RR_SHARED_PTR<RRObject>& args,
    boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    {

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(), "BeginStreamOp \"" << command << "\"");
        RR_INTRUSIVE_PTR<Message> m = CreateMessage();
        m->header = CreateMessageHeader();
        m->header->ReceiverNodeName.reset();
        m->header->SenderNodeName = GetNode()->NodeName();
        m->header->SenderNodeID = GetNode()->NodeID();
        {
            boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
            m->header->ReceiverNodeID = RemoteNodeID;
        }

        if (command == "CreateConnection")
        {
            RR_SHARED_PTR<AsyncAttachStream_args> a = RR_STATIC_POINTER_CAST<AsyncAttachStream_args>(args);
            m->header->ReceiverNodeID = a->nodeid;
            m->header->ReceiverNodeName = a->nodename;
            RR_INTRUSIVE_PTR<MessageEntry> mm = CreateMessageEntry(MessageEntryType_StreamOp, command);

            std::vector<uint32_t> caps;
            caps.push_back(TransportCapabilityCode_MESSAGE2_BASIC_PAGE | TransportCapabilityCode_MESSAGE2_BASIC_ENABLE |
                           TransportCapabilityCode_MESSAGE2_BASIC_CONNECTCOMBINED);

            if (!disable_message4)
            {
                caps.push_back(TransportCapabilityCode_MESSAGE4_BASIC_PAGE |
                               TransportCapabilityCode_MESSAGE4_BASIC_ENABLE |
                               TransportCapabilityCode_MESSAGE4_BASIC_CONNECTCOMBINED);
                if (!disable_string_table)
                {
                    caps.push_back(TransportCapabilityCode_MESSAGE4_STRINGTABLE_PAGE |
                                   TransportCapabilityCode_MESSAGE4_STRINGTABLE_ENABLE |
                                   TransportCapabilityCode_MESSAGE4_STRINGTABLE_MESSAGE_LOCAL |
                                   TransportCapabilityCode_MESSAGE4_STRINGTABLE_STANDARD_TABLE);
                }
            }
            mm->AddElement("capabilities", VectorToRRArray<uint32_t>(caps));
            m->entries.push_back(mm);
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                node, Transport, GetLocalEndpoint(),
                "Client transport requesting capabilities: " << ASIOStreamBaseTransport_log_caps(caps));
        }
        else
        {
            RR_INTRUSIVE_PTR<MessageEntry> mm = PackStreamOpRequest(command, args);
            m->entries.push_back(mm);
        }
        if (streamop_timer)
        {
            try
            {
                streamop_timer->cancel();
            }
            catch (std::exception&)
            {}
        }

        streamop_callback = callback;

        streamop_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(
            new boost::asio::deadline_timer(_io_context, boost::posix_time::milliseconds(10000)));
        RR_WEAK_PTR<ASIOStreamBaseTransport> t = RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this());
        RobotRaconteurNode::asio_async_wait(
            node, streamop_timer,
            boost::bind(&ASIOStreamBaseTransport::StreamOp_timercallback, t, boost::asio::placeholders::error));

        streamop_waiting = true;

        boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
            boost::bind(&ASIOStreamBaseTransport::SimpleAsyncEndSendMessage,
                        RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1));

        AsyncSendMessage(m, h);

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Sent StreamOp \"" << command << "\" request");
    }
}

RR_INTRUSIVE_PTR<MessageEntry> ASIOStreamBaseTransport::PackStreamOpRequest(boost::string_ref command,
                                                                            const RR_SHARED_PTR<RRObject>& args)
{
    RR_UNUSED(args);
    RR_INTRUSIVE_PTR<MessageEntry> mm = CreateMessageEntry(MessageEntryType_StreamOp, command);

    if (command == "GetRemoteNodeID")
    {}
    else
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Unknown StreamOp command: \"" << command << "\"");
        throw InvalidOperationException("Unknown StreamOp command");
    }

    return mm;
}

void ASIOStreamBaseTransport::StreamOp_EndSendMessage(const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "StreamOp send message failed " << err->Message);
        // std::cout << "Err sending streamop message" << endl;
        boost::mutex::scoped_lock lock(streamop_lock);
        if (streamop_waiting)
        {
            if (streamop_callback)
            {
                detail::PostHandlerWithException(node, streamop_callback, err);
            }
            streamop_waiting = false;

            streamop_callback.clear();
            if (streamop_timer)
            {
                streamop_timer->cancel();
                streamop_timer.reset();
            }
            if (!streamop_queue.empty())
            {
                boost::tuple<std::string, const RR_SHARED_PTR<RRObject>&,
                             boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                  const RR_SHARED_PTR<RobotRaconteurException>&)> >
                    d = streamop_queue.front();
                streamop_queue.pop();
                try
                {
                    BeginStreamOp(d.get<0>(), d.get<1>(), d.get<2>());
                }
                catch (std::exception& exp)
                {
                    detail::InvokeHandlerWithException(node, d.get<2>(), exp, MessageErrorType_ConnectionError);
                }
            }
        }
    }
}

void ASIOStreamBaseTransport::StreamOp_timercallback(RR_WEAK_PTR<ASIOStreamBaseTransport> t,
                                                     const boost::system::error_code& e)
{
    if (e != boost::asio::error::operation_aborted)
    {
        RR_SHARED_PTR<ASIOStreamBaseTransport> t2 = t.lock();
        if (!t2)
            return;
        boost::mutex::scoped_lock lock(t2->streamop_lock);
        if (t2->streamop_waiting)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(t2->node, Transport, t2->GetLocalEndpoint(), "StreamOp timed out");
            if (t2->streamop_callback)
            {
                detail::PostHandlerWithException(t2->node, t2->streamop_callback,
                                                 RR_MAKE_SHARED<RequestTimeoutException>("Timed out"), true, false);
            }
            t2->streamop_waiting = false;

            t2->streamop_callback.clear();
            t2->streamop_timer.reset();

            while (!t2->streamop_queue.empty())
            {
                boost::tuple<std::string, const RR_SHARED_PTR<RRObject>&,
                             boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                  const RR_SHARED_PTR<RobotRaconteurException>&)> >
                    d = t2->streamop_queue.front();
                t2->streamop_queue.pop();
                // t2->BeginStreamOp(d.get<0>(),d.get<1>(),d.get<2>());
                detail::PostHandlerWithException(t2->node, d.get<2>(),
                                                 RR_MAKE_SHARED<RequestTimeoutException>("Timed out"), true, false);
            }
        }
    }
}

RR_INTRUSIVE_PTR<MessageEntry> ASIOStreamBaseTransport::ProcessStreamOpRequest(
    const RR_INTRUSIVE_PTR<MessageEntry>& request, const RR_INTRUSIVE_PTR<MessageHeader>& header)
{
    const MessageStringPtr& command = request->MemberName;
    RR_INTRUSIVE_PTR<MessageEntry> mmret = CreateMessageEntry(MessageEntryType_StreamOpRet, command);

    try
    {
        if (command == "GetRemoteNodeID")
        {}
        else if (command == "CreateConnection")
        {
            RR_INTRUSIVE_PTR<MessageElement> elem_caps;
            if (request->TryFindElement("capabilities", elem_caps))
            {
                uint32_t message2_basic_caps = TransportCapabilityCode_MESSAGE2_BASIC_ENABLE;
                uint32_t message4_basic_caps = 0;
                uint32_t message4_string_caps = 0;

                std::vector<uint32_t> ret_caps;

                RR_INTRUSIVE_PTR<RRArray<uint32_t> > caps_array =
                    rr_null_check(elem_caps->CastData<RRArray<uint32_t> >());
                std::vector<uint32_t> caps_array1 = RRArrayToVector<uint32_t>(caps_array);
                for (size_t i = 0; i < caps_array->size(); i++)
                {
                    uint32_t cap = (*caps_array)[i];
                    uint32_t cap_page = cap & TranspartCapabilityCode_PAGE_MASK;
                    uint32_t cap_value = cap & ~TranspartCapabilityCode_PAGE_MASK;
                    if (cap_page == TransportCapabilityCode_MESSAGE2_BASIC_PAGE)
                    {
                        message2_basic_caps = (cap_value & (TransportCapabilityCode_MESSAGE2_BASIC_ENABLE |
                                                            TransportCapabilityCode_MESSAGE2_BASIC_CONNECTCOMBINED));
                    }

                    if (cap_page == TransportCapabilityCode_MESSAGE4_BASIC_PAGE)
                    {
                        message4_basic_caps = (cap_value & (TransportCapabilityCode_MESSAGE4_BASIC_ENABLE |
                                                            TransportCapabilityCode_MESSAGE4_BASIC_CONNECTCOMBINED));
                    }

                    if (cap_page == TransportCapabilityCode_MESSAGE4_STRINGTABLE_PAGE)
                    {
                        message4_string_caps =
                            (cap_value & (TransportCapabilityCode_MESSAGE4_STRINGTABLE_ENABLE |
                                          TransportCapabilityCode_MESSAGE4_STRINGTABLE_MESSAGE_LOCAL |
                                          TransportCapabilityCode_MESSAGE4_STRINGTABLE_STANDARD_TABLE));
                    }
                }

                if (!(message2_basic_caps & TransportCapabilityCode_MESSAGE2_BASIC_ENABLE))
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                        node, Transport, GetLocalEndpoint(),
                        "CreateConnection client transport must support message version 2");
                    throw ProtocolException("Transport must support Message Version 2");
                }
                else
                {
                    message2_basic_caps |= TransportCapabilityCode_MESSAGE2_BASIC_PAGE;
                    ret_caps.push_back(message2_basic_caps);
                    active_capabilities_message2_basic = message2_basic_caps;
                }

                if ((message4_basic_caps & TransportCapabilityCode_MESSAGE4_BASIC_ENABLE) && !disable_message4)
                {
                    send_version4.store(true);
                    message4_basic_caps |= TransportCapabilityCode_MESSAGE4_BASIC_PAGE;
                    ret_caps.push_back(message4_basic_caps);
                    active_capabilities_message4_basic = message4_basic_caps;
                    if ((message4_string_caps & TransportCapabilityCode_MESSAGE4_STRINGTABLE_ENABLE) &&
                        !disable_string_table)
                    {
                        message4_string_caps |= TransportCapabilityCode_MESSAGE4_STRINGTABLE_PAGE;
                        std::vector<uint32_t> string_table_flags;
                        string_table_flags.push_back(message4_string_caps);
                        string_table4->SetTableFlags(string_table_flags);
                        ret_caps.push_back(message4_string_caps);
                        active_capabilities_message4_stringtable = message4_string_caps;
                        use_string_table4.store(true);
                    }
                }

                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "Server transport negotiated capabilities: "
                                                       << ASIOStreamBaseTransport_log_caps(ret_caps)
                                                       << " client requested capabilities: "
                                                       << ASIOStreamBaseTransport_log_caps(ret_caps));
                mmret->AddElement("capabilities", VectorToRRArray<uint32_t>(ret_caps));
            }

            boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
            if (RemoteNodeID.IsAnyNode() || RemoteNodeID == header->SenderNodeID)
            {
                if (header->ReceiverNodeID.IsAnyNode() &&
                    (header->ReceiverNodeName == "" || header->ReceiverNodeName == GetNode()->NodeName()))
                {
                    RemoteNodeID = header->SenderNodeID;
                    return mmret;
                }

                if (header->ReceiverNodeID == GetNode()->NodeID())
                {
                    if (header->ReceiverNodeName == "" || header->ReceiverNodeName == GetNode()->NodeName())
                    {
                        RemoteNodeID = header->SenderNodeID;
                        return mmret;
                    }
                }
            }

            lock.unlock();
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(), "Client requested unknown node");

            mmret->Error = MessageErrorType_NodeNotFound;
            mmret->AddElement("errorname", stringToRRArray("RobotRaconteur.NodeNotFound"));
            mmret->AddElement("errorstring", stringToRRArray("Node not found"));
        }
        else
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Client requested unknown StreamOp command: \"" << command << "\"");
            throw ProtocolException("Unknown StreamOp command");
        }
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Processing StreamOp request failed: \"" << exp.what() << "\"");
        mmret->Error = MessageErrorType_ProtocolError;
        mmret->AddElement("errorname", stringToRRArray("RobotRaconteur.ProtocolError"));
        mmret->AddElement("errorstring", stringToRRArray("Invalid Stream Operation"));
    }

    return mmret;
}

void ASIOStreamBaseTransport::StreamOpMessageReceived(const RR_INTRUSIVE_PTR<Message>& m)
{

    // std::cout << "streamop got message" << endl;

    RR_INTRUSIVE_PTR<MessageEntry> mm;

    try
    {
        mm = m->entries.at(0);
    }
    catch (std::exception&)
    {
        return;
    }
    if (mm->EntryType == MessageEntryType_StreamOp)
    {
        const MessageStringPtr& command = mm->MemberName;
        RR_INTRUSIVE_PTR<Message> mret = CreateMessage();
        mret->header = CreateMessageHeader();
        mret->header->SenderNodeName = GetNode()->NodeName();
        mret->header->ReceiverNodeName = m->header->SenderNodeName;
        mret->header->SenderNodeID = GetNode()->NodeID();
        mret->header->ReceiverNodeID = m->header->SenderNodeID;
        RR_INTRUSIVE_PTR<MessageEntry> mmret = ProcessStreamOpRequest(mm, m->header);

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Received StreamOp \"" << command << "\" request");

        if (mmret)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Sending StreamOp \"" << command << "\" response");
            mret->entries.push_back(mmret);
            boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h = boost::bind(
                &ASIOStreamBaseTransport::StreamOp_EndSendMessage,
                RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1));
            AsyncSendMessage(mret, h);
        }
    }
    else
    {
        boost::mutex::scoped_lock lock(streamop_lock);
        if (streamop_waiting)
        {
            streamop_waiting = false;

            const MessageStringPtr& command = mm->MemberName;

            RR_SHARED_PTR<RobotRaconteurException> rrexp;
            RR_SHARED_PTR<RRObject> ret;

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, GetLocalEndpoint(),
                                               "Received StreamOp \"" << command << "\" response");

            try
            {

                ret = UnpackStreamOpResponse(mm, m->header);
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "Unpacking StreamOp \"" << m->entries.at(0)->MemberName
                                                                           << "\" response failed: " << exp.what());
                rrexp = RobotRaconteurExceptionUtil::ExceptionToSharedPtr(exp);
            }

            if (streamop_callback)
            {
                try
                {
                    streamop_callback(rr_cast<RRObject>(ret), rrexp);
                }
                catch (std::exception& exp)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                       "Error in StreamOp \"" << m->entries.at(0)->MemberName
                                                                              << "\" handler: " << exp.what()
                                                                              << ", closing transport connection");
                    Close();
                }
            }
            streamop_callback.clear();
            if (streamop_timer)
            {
                streamop_timer->cancel();
                streamop_timer.reset();
            }
            while (!streamop_queue.empty())
            {
                boost::tuple<std::string, const RR_SHARED_PTR<RRObject>&,
                             boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                  const RR_SHARED_PTR<RobotRaconteurException>&)> >
                    d = streamop_queue.front();
                streamop_queue.pop();
                try
                {
                    BeginStreamOp(d.get<0>(), d.get<1>(), d.get<2>());
                    break;
                }
                catch (std::exception& exp)
                {
                    detail::InvokeHandlerWithException(node, d.get<2>(), exp);
                }
            }
        }
    }
}

RR_SHARED_PTR<RRObject> ASIOStreamBaseTransport::UnpackStreamOpResponse(const RR_INTRUSIVE_PTR<MessageEntry>& response,
                                                                        const RR_INTRUSIVE_PTR<MessageHeader>& header)
{
    const MessageStringPtr& command = response->MemberName;
    if (command == "GetRemoteNodeID")
    {
        RR_SHARED_PTR<NodeID> ret = RR_MAKE_SHARED<NodeID>(header->SenderNodeID);
        return ret;
    }
    else if (command == "CreateConnection")
    {
        boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
        if (response->Error != 0 && response->Error != MessageErrorType_ProtocolError)
        {
            RobotRaconteurExceptionUtil::ThrowMessageEntryException(response);
        }

        if (!RemoteNodeID.IsAnyNode())
        {
            if (header->SenderNodeID != RemoteNodeID)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                   "CreateConnection server returned unexpected NodeID");
                throw ConnectionException("Invalid node connection");
            }
        }
        else
        {
            if (!target_nodename.empty())
            {
                if (header->SenderNodeName != target_nodename)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                       "CreateConnection server returned unexpected NodeName");
                    throw ConnectionException("Invalid node connection");
                }
            }

            if (!target_nodeid.IsAnyNode())
            {
                if (target_nodeid != header->SenderNodeID)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                                       "CreateConnection server returned unexpected NodeID");
                    throw ConnectionException("Invalid node connection");
                }
            }
        }

        RR_INTRUSIVE_PTR<MessageElement> elem_caps;
        if (response->TryFindElement("capabilities", elem_caps))
        {
            uint32_t message2_basic_caps = TransportCapabilityCode_MESSAGE2_BASIC_ENABLE;
            uint32_t message4_basic_caps = 0;
            uint32_t message4_string_caps = 0;

            RR_INTRUSIVE_PTR<RRArray<uint32_t> > caps_array = rr_null_check(elem_caps->CastData<RRArray<uint32_t> >());
            std::vector<uint32_t> caps_array1 = RRArrayToVector<uint32_t>(caps_array);
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                node, Transport, GetLocalEndpoint(),
                "Client transport negotiated capabilities: " << ASIOStreamBaseTransport_log_caps(caps_array1));
            for (size_t i = 0; i < caps_array->size(); i++)
            {
                uint32_t cap = (*caps_array)[i];
                uint32_t cap_page = cap & TranspartCapabilityCode_PAGE_MASK;
                uint32_t cap_value = cap & ~TranspartCapabilityCode_PAGE_MASK;
                if (cap_page == TransportCapabilityCode_MESSAGE2_BASIC_PAGE)
                {
                    if (!(cap_value & TransportCapabilityCode_MESSAGE2_BASIC_ENABLE))
                    {
                        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                            node, Transport, GetLocalEndpoint(),
                            "CreateConnection server transport must support message version 2");
                        throw ProtocolException("Transport must support Message Version 2");
                    }

                    if ((cap_value & ~(TransportCapabilityCode_MESSAGE2_BASIC_ENABLE |
                                       TransportCapabilityCode_MESSAGE2_BASIC_CONNECTCOMBINED)) != 0)
                    {
                        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                            node, Transport, GetLocalEndpoint(),
                            "CreateConnection invalid version 2 message caps returned by server");
                        throw ProtocolException("Invalid Message Version 2 capabilities");
                    }

                    message2_basic_caps = cap_value;
                }

                if (cap_page == TransportCapabilityCode_MESSAGE4_BASIC_PAGE)
                {
                    if (disable_message4)
                    {
                        if (cap_value != 0)
                        {
                            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                                node, Transport, GetLocalEndpoint(),
                                "CreateConnection invalid version 4 message caps returned by server");
                            throw ProtocolException("Invalid Message Version 4 capabilities");
                        }
                    }
                    else
                    {
                        if (!(cap_value & TransportCapabilityCode_MESSAGE4_BASIC_ENABLE))
                        {
                            if (cap_value != 0)
                            {
                                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                                    node, Transport, GetLocalEndpoint(),
                                    "CreateConnection invalid version 4 message caps returned by server");
                                throw ProtocolException("Invalid Message Version 4 capabilities");
                            }
                        }
                        else
                        {
                            if ((cap_value & ~(TransportCapabilityCode_MESSAGE4_BASIC_ENABLE |
                                               TransportCapabilityCode_MESSAGE4_BASIC_CONNECTCOMBINED)) != 0)
                            {
                                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                                    node, Transport, GetLocalEndpoint(),
                                    "CreateConnection invalid version 4 message caps returned by server");
                                throw ProtocolException("Invalid Message Version 4 capabilities");
                            }

                            message4_basic_caps = cap_value;
                        }
                    }
                }

                if (cap_page == TransportCapabilityCode_MESSAGE4_STRINGTABLE_PAGE)
                {
                    if (disable_message4 || disable_string_table)
                    {
                        if (cap_value != 0)
                        {
                            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                                node, Transport, GetLocalEndpoint(),
                                "CreateConnection invalid version 4 string table caps returned by server");
                            throw ProtocolException("Invalid Message Version 4 String Table capabilities");
                        }
                    }
                    else
                    {
                        if (!(cap_value & TransportCapabilityCode_MESSAGE4_STRINGTABLE_ENABLE))
                        {
                            if (cap_value != 0)
                            {
                                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                                    node, Transport, GetLocalEndpoint(),
                                    "CreateConnection invalid version 4 string table caps returned by server");
                                throw ProtocolException("Invalid Message Version 4 String Table capabilities");
                            }
                        }
                        else
                        {
                            if ((cap_value & ~(TransportCapabilityCode_MESSAGE4_STRINGTABLE_ENABLE |
                                               TransportCapabilityCode_MESSAGE4_STRINGTABLE_MESSAGE_LOCAL |
                                               TransportCapabilityCode_MESSAGE4_STRINGTABLE_STANDARD_TABLE)) != 0)
                            {
                                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                                    node, Transport, GetLocalEndpoint(),
                                    "CreateConnection invalid version 4 string table caps returned by server");
                                throw ProtocolException("Invalid Message Version 4 String Table capabilities");
                            }

                            message4_string_caps = cap_value;
                        }
                    }
                }
            }

            active_capabilities_message2_basic = message2_basic_caps | TransportCapabilityCode_MESSAGE2_BASIC_PAGE;

            if (message4_basic_caps)
            {
                active_capabilities_message4_basic = message4_basic_caps | TransportCapabilityCode_MESSAGE2_BASIC_PAGE;
                send_version4.store(true);
                if (message4_string_caps)
                {
                    active_capabilities_message4_stringtable =
                        message4_string_caps | TransportCapabilityCode_MESSAGE4_STRINGTABLE_PAGE;
                    std::vector<uint32_t> string_table_flags;
                    string_table_flags.push_back(active_capabilities_message4_stringtable);
                    string_table4->SetTableFlags(string_table_flags);
                    use_string_table4.store(true);
                }
            }
            else
            {
                if (message4_string_caps != 0)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                        node, Transport, GetLocalEndpoint(),
                        "CreateConnection invalid version 4 string table settings returned by server");
                    throw ProtocolException("Message 4 must be enabled for String Table 4");
                }
            }
        }

        RR_SHARED_PTR<NodeID> ret = RR_MAKE_SHARED<NodeID>(header->SenderNodeID);
        return ret;
    }
    else
    {

        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Unknown StreamOp command \"" << command << "\"");
        throw MemberNotFoundException("Unknown command");
    }
}

void ASIOStreamBaseTransport::PeriodicCleanupTask() {}

NodeID ASIOStreamBaseTransport::GetRemoteNodeID()
{
    boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
    return RemoteNodeID;
}

void ASIOStreamBaseTransport::SendMessage(const RR_INTRUSIVE_PTR<Message>& m)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<void> > s =
        RR_MAKE_SHARED<detail::sync_async_handler<void> >(RR_MAKE_SHARED<ConnectionException>("Send timeout"));

    boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
        boost::bind(&detail::sync_async_handler<void>::operator(), s, RR_BOOST_PLACEHOLDERS(_1));
    AsyncSendMessage(m, h);

    s->end_void();
}

bool ASIOStreamBaseTransport::IsLargeTransferAuthorized() { return true; }

bool ASIOStreamBaseTransport::GetDisableMessage4() { return disable_message4; }
void ASIOStreamBaseTransport::SetDisableMessage4(bool d) { disable_message4 = d; }

bool ASIOStreamBaseTransport::GetDisableStringTable() { return disable_string_table; }
void ASIOStreamBaseTransport::SetDisableStringTable(bool d) { disable_string_table = d; }

bool ASIOStreamBaseTransport::CheckCapabilityActive(uint32_t cap)
{
    uint32_t cap_page = cap & TranspartCapabilityCode_PAGE_MASK;
    uint32_t cap_value = cap & (~TranspartCapabilityCode_PAGE_MASK);

    if (cap_page == TransportCapabilityCode_MESSAGE2_BASIC_PAGE)
    {
        return (cap_value & (active_capabilities_message2_basic & (~TranspartCapabilityCode_PAGE_MASK))) != 0;
    }

    if (cap_page == TransportCapabilityCode_MESSAGE4_BASIC_PAGE)
    {
        return (cap_value & (active_capabilities_message4_basic & (~TranspartCapabilityCode_PAGE_MASK))) != 0;
    }

    if (cap_page == TransportCapabilityCode_MESSAGE4_STRINGTABLE_PAGE)
    {
        return (cap_value & (active_capabilities_message4_stringtable & (~TranspartCapabilityCode_PAGE_MASK))) != 0;
    }

    return false;
}

} // namespace detail
} // namespace RobotRaconteur
