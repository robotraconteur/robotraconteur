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
        boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> callback;
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

    boost::array<uint8_t, 8> streammagic;
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
    boost::function<void(uint32_t, const RR_SHARED_PTR<RobotRaconteurException>&)> CheckStreamCapability_callback;
    std::queue<
        boost::tuple<std::string, boost::function<void(uint32_t, const RR_SHARED_PTR<RobotRaconteurException>&)> > >
        CheckStreamCapability_queue;

    RR_SHARED_PTR<boost::asio::deadline_timer> CheckStreamCapability_timer;

    bool streamop_closed;
    bool streamop_waiting;
    boost::mutex streamop_lock;
    boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
        streamop_callback;
    std::queue<boost::tuple<
        std::string, const RR_SHARED_PTR<RRObject>&,
        boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)> > >
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

    ASIOStreamBaseTransport(const RR_SHARED_PTR<RobotRaconteurNode>& node);

    RR_WEAK_PTR<RobotRaconteurNode> node;

  public:
    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteurNode> GetNode() RR_OVERRIDE;

  private:
    ASIOStreamBaseTransport(const ASIOStreamBaseTransport& that);

  protected:
    class AsyncAttachStream_args : public RRObject
    {
      public:
        RobotRaconteur::NodeID nodeid;
        std::string nodename;

        // NOLINTBEGIN(bugprone-throw-keyword-missing)
        AsyncAttachStream_args(const RobotRaconteur::NodeID& nodeid_, boost::string_ref nodename_)
            : nodeid(nodeid_), nodename(RR_MOVE(nodename_.to_string()))
        {}
        // NOLINTEND(bugprone-throw-keyword-missing)

        RR_OVIRTUAL std::string RRType() RR_OVERRIDE
        {
            return "RobotRaconteur::ASIOStreamBaseTransport::AsyncAttachStream_args";
        }
    };

    virtual void AsyncAttachStream(
        bool server, const NodeID& target_nodeid, boost::string_ref target_nodename,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

  public:
    RR_OVIRTUAL void AsyncSendMessage(
        const RR_INTRUSIVE_PTR<Message>& m,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback) RR_OVERRIDE;
    RR_OVIRTUAL void SendMessage(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;

  protected:
    void SimpleAsyncSendMessage(const RR_INTRUSIVE_PTR<Message>& m,
                                const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

    void SimpleAsyncEndSendMessage(const RR_SHARED_PTR<RobotRaconteurException>& err);

    virtual void AsyncAttachStream1(
        const RR_SHARED_PTR<RRObject>& parameter, const RR_SHARED_PTR<RobotRaconteurException>& err,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

    virtual void BeginSendMessage(const RR_INTRUSIVE_PTR<Message>& m,
                                  const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);
    virtual void BeginSendMessage1(
        const RR_INTRUSIVE_PTR<Message>& m,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

    virtual void EndSendMessage(size_t startpos, const boost::system::error_code& error, size_t bytes_transferred,
                                const RR_INTRUSIVE_PTR<Message>& m, size_t m_len,
                                const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback,
                                const boost::shared_array<uint8_t>& buf);
    virtual void EndSendMessage1();

    virtual void EndSendMessage2(const boost::system::error_code& error, size_t bytes_transferred,
                                 const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

    virtual void AsyncPauseSend(const boost::function<void(const boost::system::error_code&)>& handler);
    virtual void AsyncResumeSend();

    // virtual void EndReceiveMessage(const boost::system::error_code& error,
    // size_t bytes_transferred,  const boost::shared_array<uint8_t>& buf);

    // virtual void EndReceiveMessage2(const RR_SHARED_PTR<RobotRaconteur::Message>& message);

    virtual void BeginReceiveMessage1();

    virtual void EndReceiveMessage1(size_t startpos, const boost::system::error_code& error, size_t bytes_transferred);

    virtual void EndReceiveMessage2(size_t startpos, const boost::system::error_code& error, size_t bytes_transferred,
                                    size_t message_size, const boost::shared_array<uint8_t>& buf);

    virtual void EndReceiveMessage3(const RR_INTRUSIVE_PTR<Message>& message);
    virtual void EndReceiveMessage4();

    virtual void EndReceiveMessage5(const boost::system::error_code& error, size_t bytes_transferred);

    virtual void AsyncPauseReceive(const boost::function<void(const boost::system::error_code&)>& handler);
    virtual void AsyncResumeReceive();

    RR_OVIRTUAL void Close() RR_OVERRIDE;

    virtual void heartbeat_timer_func(const boost::system::error_code& e);

    boost::mutex heartbeat_timer_lock;

  public:
    virtual void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) = 0;

    virtual bool IsConnected();

    virtual uint32_t StreamCapabilities(boost::string_ref name);

    virtual void AsyncCheckStreamCapability(
        boost::string_ref name,
        const boost::function<void(uint32_t, const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

  protected:
    virtual void BeginCheckStreamCapability(
        boost::string_ref name,
        const boost::function<void(uint32_t, const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

    void CheckStreamCapability_EndSendMessage(const RR_SHARED_PTR<RobotRaconteurException>& err);

    static void CheckStreamCapability_timercallback(RR_WEAK_PTR<ASIOStreamBaseTransport> t,
                                                    const boost::system::error_code& e);

    void CheckStreamCapability_MessageReceived(const RR_INTRUSIVE_PTR<Message>& m);

  public:
    void AsyncStreamOp(
        boost::string_ref command, const RR_SHARED_PTR<RRObject>& args,
        boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>&);

    virtual void PeriodicCleanupTask();

    RR_OVIRTUAL NodeID GetRemoteNodeID() RR_OVERRIDE;

  protected:
    virtual void BeginStreamOp(
        boost::string_ref command, const RR_SHARED_PTR<RRObject>& args,
        boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>&);

    virtual RR_INTRUSIVE_PTR<MessageEntry> PackStreamOpRequest(boost::string_ref command,
                                                               const RR_SHARED_PTR<RRObject>& args);

    virtual void StreamOp_EndSendMessage(const RR_SHARED_PTR<RobotRaconteurException>& err);

    static void StreamOp_timercallback(RR_WEAK_PTR<ASIOStreamBaseTransport> t, const boost::system::error_code& e);

    virtual void StreamOpMessageReceived(const RR_INTRUSIVE_PTR<Message>& m);

    virtual RR_INTRUSIVE_PTR<MessageEntry> ProcessStreamOpRequest(const RR_INTRUSIVE_PTR<MessageEntry>& request,
                                                                  const RR_INTRUSIVE_PTR<MessageHeader>& header);

    virtual RR_SHARED_PTR<RRObject> UnpackStreamOpResponse(const RR_INTRUSIVE_PTR<MessageEntry>& response,
                                                           const RR_INTRUSIVE_PTR<MessageHeader>& header);

    virtual void async_write_some(
        const_buffers& b,
        const boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler) = 0;

    virtual void async_read_some(
        mutable_buffers& b,
        const boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler) = 0;

    virtual size_t available() = 0;

    virtual bool IsLargeTransferAuthorized();

  public:
    virtual bool GetDisableMessage4();
    virtual void SetDisableMessage4(bool d);

    virtual bool GetDisableStringTable();
    virtual void SetDisableStringTable(bool d);

    RR_OVIRTUAL bool CheckCapabilityActive(uint32_t cap) RR_OVERRIDE;
};

} // namespace detail
} // namespace RobotRaconteur
