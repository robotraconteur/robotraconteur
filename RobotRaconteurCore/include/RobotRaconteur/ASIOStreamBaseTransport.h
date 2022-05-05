/**
 * @file ASIOStreamBaseTransport.h
 *
 * @author John Wason, PhD
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

#include "RobotRaconteur/RobotRaconteurNode.h"
#include <boost/bind/placeholders.hpp>
#include <boost/asio.hpp>
#include <list>
#include <boost/bind/protect.hpp>
#include <boost/shared_array.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/atomic.hpp>
#include "RobotRaconteur/AsyncMessageIO.h"

#pragma once

namespace RobotRaconteur
{

namespace detail
{
class StringTable;

class ASIOStreamBaseTransport : public ITransportConnection, public RR_ENABLE_SHARED_FROM_THIS<ASIOStreamBaseTransport>
{

  protected:
    struct message_queue_entry
    {
        RR_INTRUSIVE_PTR<Message> message;
        boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> callback;
    };

    RR_BOOST_ASIO_IO_CONTEXT& _io_context;

    boost::atomic<bool> connected;

    boost::shared_array<uint8_t> sendbuf;
    size_t sendbuf_len;

    boost::mutex send_lock;
    boost::mutex sync_send_lock;
    bool sending;
    bool send_pause_request;
    bool send_paused;
    boost::function<void(const boost::system::error_code&)> send_pause_request_handler;

    std::list<message_queue_entry> send_queue;
    size_t send_message_size;
    boost::condition_variable send_event;

    boost::atomic<boost::posix_time::ptime> tlastsend;
    boost::atomic<boost::posix_time::ptime> tlastrecv;

    uint8_t streamseed[8];
    uint32_t recv_message_size;
    boost::shared_array<uint8_t> recvbuf;
    size_t recvbuf_len;
    size_t recvbuf_pos;
    size_t recvbuf_end;
    bool recv_pause_request;
    bool recv_paused;
    bool receiving;
    boost::function<void(const boost::system::error_code&)> recv_pause_request_handler;

    boost::mutex recv_lock;

    RR_SHARED_PTR<boost::asio::deadline_timer> heartbeat_timer;

    uint32_t ReceiveTimeout;
    uint32_t HeartbeatPeriod;

    bool SendHeartbeat;

    bool CheckStreamCapability_closed;
    bool CheckStreamCapability_waiting;
    boost::mutex CheckStreamCapability_lock;
    boost::function<void(uint32_t, RR_SHARED_PTR<RobotRaconteurException>)> CheckStreamCapability_callback;
    std::queue<boost::tuple<std::string, boost::function<void(uint32_t, RR_SHARED_PTR<RobotRaconteurException>)> > >
        CheckStreamCapability_queue;

    RR_SHARED_PTR<boost::asio::deadline_timer> CheckStreamCapability_timer;

    bool streamop_closed;
    bool streamop_waiting;
    boost::mutex streamop_lock;
    boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> streamop_callback;
    std::queue<boost::tuple<std::string, RR_SHARED_PTR<RRObject>,
                            boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> > >
        streamop_queue;

    RR_SHARED_PTR<boost::asio::deadline_timer> streamop_timer;
    NodeID RemoteNodeID;
    boost::shared_mutex RemoteNodeID_lock;

    NodeID target_nodeid;
    std::string target_nodename;

    int32_t max_message_size;

    bool send_large_transfer_authorized;
    bool recv_large_transfer_authorized;

    boost::atomic<bool> send_version4;
    boost::atomic<bool> use_string_table4;

    RR_SHARED_PTR<detail::StringTable> string_table4;
    boost::mutex string_table4_lock;
    std::list<uint32_t> string_table_4_confirming;

    RR_UNORDERED_MAP<uint32_t, boost::tuple<std::vector<uint32_t>, boost::posix_time::ptime> > string_table_4_requests;
    uint32_t string_table_4_requestid;

    std::list<RR_SHARED_PTR<boost::asio::deadline_timer> > string_table_4_timers;
    bool string_table_4_pause_updates;

    bool string_table_4_closed;

    bool server;

    bool disable_message4;
    bool disable_string_table;
    bool disable_async_io;

    mutable_buffers active_recv_bufs;

    RR_SHARED_PTR<AsyncMessageReader> async_reader;

    size_t async_recv_size;
    size_t async_recv_pos;
    uint16_t async_recv_version;
    size_t async_recv_continue_buf_count;

    RR_SHARED_PTR<AsyncMessageWriter> async_writer;

    uint16_t async_send_version;
    const_buffers async_send_bufs;

    uint32_t active_capabilities_message2_basic;
    uint32_t active_capabilities_message4_basic;
    uint32_t active_capabilities_message4_stringtable;

  protected:
    ASIOStreamBaseTransport(RR_SHARED_PTR<RobotRaconteurNode> node);

    RR_WEAK_PTR<RobotRaconteurNode> node;

  public:
    virtual RR_SHARED_PTR<RobotRaconteurNode> GetNode();

  private:
    ASIOStreamBaseTransport(const ASIOStreamBaseTransport& that);

  protected:
    class AsyncAttachStream_args : public RRObject
    {
      public:
        NodeID nodeid;
        std::string nodename;

        AsyncAttachStream_args(const NodeID& nodeid_, boost::string_ref nodename_)
        {
            nodeid = nodeid_;
            nodename = RR_MOVE(nodename_.to_string());
        }

        virtual std::string RRType() { return "RobotRaconteur::ASIOStreamBaseTransport::AsyncAttachStream_args"; }
    };

    virtual void AsyncAttachStream(bool server, const NodeID& target_nodeid, boost::string_ref target_nodename,
                                   boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

  public:
    virtual void AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m,
                                  boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);
    virtual void SendMessage(RR_INTRUSIVE_PTR<Message> m);

  protected:
    void SimpleAsyncSendMessage(RR_INTRUSIVE_PTR<Message> m,
                                boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    void SimpleAsyncEndSendMessage(RR_SHARED_PTR<RobotRaconteurException> err);

    virtual void AsyncAttachStream1(RR_SHARED_PTR<RRObject> parameter, RR_SHARED_PTR<RobotRaconteurException> err,
                                    boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    virtual void BeginSendMessage(RR_INTRUSIVE_PTR<Message> m,
                                  boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);
    virtual void BeginSendMessage1(RR_INTRUSIVE_PTR<Message> m,
                                   boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    virtual void EndSendMessage(size_t startpos, const boost::system::error_code& error, size_t bytes_transferred,
                                RR_INTRUSIVE_PTR<Message> m, size_t m_len,
                                boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback,
                                boost::shared_array<uint8_t> buf);
    virtual void EndSendMessage1();

    virtual void EndSendMessage2(const boost::system::error_code& error, size_t bytes_transferred,
                                 boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    virtual void AsyncPauseSend(boost::function<void(const boost::system::error_code&)>& handler);
    virtual void AsyncResumeSend();

    // virtual void EndReceiveMessage(const boost::system::error_code& error,
    // size_t bytes_transferred,  boost::shared_array<uint8_t> buf);

    // virtual void EndReceiveMessage2(RR_SHARED_PTR<RobotRaconteur::Message> message);

    virtual void BeginReceiveMessage1();

    virtual void EndReceiveMessage1(size_t startpos, const boost::system::error_code& error, size_t bytes_transferred);

    virtual void EndReceiveMessage2(size_t startpos, const boost::system::error_code& error, size_t bytes_transferred,
                                    size_t message_len, boost::shared_array<uint8_t> buf);

    virtual void EndReceiveMessage3(RR_INTRUSIVE_PTR<Message> message);
    virtual void EndReceiveMessage4();

    virtual void EndReceiveMessage5(const boost::system::error_code& error, size_t bytes_transferred);

    virtual void AsyncPauseReceive(boost::function<void(const boost::system::error_code&)>& handler);
    virtual void AsyncResumeReceive();

    virtual void Close();

    virtual void heartbeat_timer_func(const boost::system::error_code& e);

    boost::mutex heartbeat_timer_lock;

  public:
    virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m) = 0;

    virtual bool IsConnected();

    virtual uint32_t StreamCapabilities(boost::string_ref name);

    virtual void AsyncCheckStreamCapability(
        boost::string_ref name, boost::function<void(uint32_t, RR_SHARED_PTR<RobotRaconteurException>)>& callback);

  protected:
    virtual void BeginCheckStreamCapability(
        boost::string_ref name, boost::function<void(uint32_t, RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    void CheckStreamCapability_EndSendMessage(RR_SHARED_PTR<RobotRaconteurException> err);

    static void CheckStreamCapability_timercallback(RR_WEAK_PTR<ASIOStreamBaseTransport> t,
                                                    const boost::system::error_code& e);

    void CheckStreamCapability_MessageReceived(RR_INTRUSIVE_PTR<Message> m);

  public:
    void AsyncStreamOp(boost::string_ref command, RR_SHARED_PTR<RRObject> args,
                       boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>&);

    virtual void PeriodicCleanupTask();

    virtual NodeID GetRemoteNodeID();

  protected:
    virtual void BeginStreamOp(boost::string_ref command, RR_SHARED_PTR<RRObject> args,
                               boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>&);

    virtual RR_INTRUSIVE_PTR<MessageEntry> PackStreamOpRequest(boost::string_ref command, RR_SHARED_PTR<RRObject> args);

    virtual void StreamOp_EndSendMessage(RR_SHARED_PTR<RobotRaconteurException> err);

    static void StreamOp_timercallback(RR_WEAK_PTR<ASIOStreamBaseTransport> t, const boost::system::error_code& e);

    virtual void StreamOpMessageReceived(RR_INTRUSIVE_PTR<Message> m);

    virtual RR_INTRUSIVE_PTR<MessageEntry> ProcessStreamOpRequest(RR_INTRUSIVE_PTR<MessageEntry> request,
                                                                  RR_INTRUSIVE_PTR<MessageHeader> header);

    virtual RR_SHARED_PTR<RRObject> UnpackStreamOpResponse(RR_INTRUSIVE_PTR<MessageEntry> response,
                                                           RR_INTRUSIVE_PTR<MessageHeader> header);

    virtual void async_write_some(
        const_buffers& b,
        boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler) = 0;

    virtual void async_read_some(
        mutable_buffers& b,
        boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler) = 0;

    virtual size_t available() = 0;

    virtual bool IsLargeTransferAuthorized();

  public:
    virtual bool GetDisableMessage4();
    virtual void SetDisableMessage4(bool d);

    virtual bool GetDisableStringTable();
    virtual void SetDisableStringTable(bool d);

    virtual bool CheckCapabilityActive(uint32_t flag);
};

} // namespace detail
} // namespace RobotRaconteur
