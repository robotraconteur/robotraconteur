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

#define BOOST_ASIO_HAS_LOCAL_SOCKETS 1

#ifdef _WIN32
#include <WinSock2.h>
#include <versionhelpers.h>

#ifdef IO_REPARSE_TAG_AF_UNIX
#include <afunix.h>
#else
// Need to copy over sockaddr_un due to missing afunix.h
#define UNIX_PATH_MAX 108

typedef struct sockaddr_un
{
    ADDRESS_FAMILY sun_family;    /* AF_UNIX */
    char sun_path[UNIX_PATH_MAX]; /* pathname */
} SOCKADDR_UN, *PSOCKADDR_UN;
#endif

#include <boost/asio/version.hpp>

#if BOOST_ASIO_VERSION < 101801
namespace boost
{
namespace asio
{
namespace detail
{
typedef sockaddr_un sockaddr_un_type;
}
} // namespace asio
} // namespace boost

#include <boost/asio/local/stream_protocol.hpp>
#endif

#endif

#include <boost/bind/placeholders.hpp>
#include <boost/asio.hpp>

#include "LocalTransport_private.h"

#include "RobotRaconteur/Tap.h"

#include "RobotRaconteur/IOUtils.h"

namespace RobotRaconteur
{
namespace detail
{
class Tap_socket
{
  public:
    Tap_socket(RR_BOOST_ASIO_IO_CONTEXT& context)
    {
        socket.reset(new boost::asio::local::stream_protocol::socket(context));
    }
    RR_SHARED_PTR<boost::asio::local::stream_protocol::socket> socket;
};
class Tap_acceptor
{
  public:
    Tap_acceptor(RR_BOOST_ASIO_IO_CONTEXT& context, boost::asio::local::stream_protocol::endpoint ep)
        : acceptor(context, ep)
    {}
    Tap_acceptor(RR_BOOST_ASIO_IO_CONTEXT& context) : acceptor(context) {}
    boost::asio::local::stream_protocol::acceptor acceptor;
};
} // namespace detail

namespace detail
{

RR_INTRUSIVE_PTR<MessageEntry> RRLogRecordToMessageEntry(const RRLogRecord& record)
{
    RR_INTRUSIVE_PTR<MessageEntry> ret = CreateMessageEntry(MessageEntryType_StreamOp, "logrecord");
    RR_SHARED_PTR<RobotRaconteurNode> node = record.Node.lock();
    if (!node)
    {
        ret->AddElement("node", stringToRRArray("unknown"));
    }
    else
    {
        NodeID id;
        if (node->TryGetNodeID(id))
        {
            ret->AddElement("node", stringToRRArray(id.ToString()));
        }
        else
        {
            ret->AddElement("node", stringToRRArray("unknown"));
        }
    }
    ret->AddElement("level", ScalarToRRArray<int32_t>(record.Level));
    ret->AddElement("component", ScalarToRRArray<int32_t>(record.Component));
    ret->AddElement("componentname", stringToRRArray(record.ComponentName));
    ret->AddElement("componentobjectid", stringToRRArray(record.ComponentObjectID));
    ret->AddElement("endpoint", ScalarToRRArray<int64_t>(record.Endpoint));
    ret->AddElement("servicepath", stringToRRArray(record.ServicePath));
    ret->AddElement("member", stringToRRArray(record.Member));
    ret->AddElement("message", stringToRRArray(record.Message));
    ret->AddElement("time", stringToRRArray(boost::posix_time::to_iso_extended_string(record.Time)));
    ret->AddElement("sourcefile", stringToRRArray(record.SourceFile));
    ret->AddElement("sourceline", ScalarToRRArray<uint32_t>(record.SourceLine));
    ret->AddElement("threadid", stringToRRArray(record.ThreadID));
    ret->AddElement("fiberid", stringToRRArray(record.FiberID));
    return ret;
}

RR_INTRUSIVE_PTR<Message> RRLogRecordToMessage(const RRLogRecord& record)
{
    RR_INTRUSIVE_PTR<Message> ret = CreateMessage();
    ret->header = CreateMessageHeader();

    RR_SHARED_PTR<RobotRaconteurNode> node = record.Node.lock();
    if (node)
    {
        NodeID id;
        if (node->TryGetNodeID(id))
        {
            ret->header->SenderNodeID = id;
        }

        std::string name;
        if (node->TryGetNodeName(name))
        {
            ret->header->SenderNodeName = name;
        }
    }

    ret->entries.push_back(RRLogRecordToMessageEntry(record));
    return ret;
}

class LocalMessageTapConnectionImpl : public RR_ENABLE_SHARED_FROM_THIS<LocalMessageTapConnectionImpl>
{
  public:
    RR_SHARED_PTR<RR_BOOST_ASIO_IO_CONTEXT> io_context;
    RR_SHARED_PTR<detail::Tap_socket> socket;
    bool LogAll;
    bool sending;

    boost::mutex this_lock;

    std::list<RR_INTRUSIVE_PTR<Message> > send_queue;
    boost::shared_array<uint8_t> send_buffer;
    size_t buffer_len;
    size_t message_len;
    size_t message_pos;

    uint8_t recv_buf[1024];

    LocalMessageTapConnectionImpl(RR_SHARED_PTR<RR_BOOST_ASIO_IO_CONTEXT> io_context, bool log_all)
    {
        this->LogAll = log_all;
        this->io_context = io_context;
        this->sending = false;

        buffer_len = 32 * 1024;
        send_buffer.reset(new uint8_t[buffer_len]);
    }

    void AttachSocket(RR_SHARED_PTR<detail::Tap_socket> socket)
    {
        this->socket = socket;

        start_recv();
    }

    void Close() { socket.reset(); }

    void RecordMessage(RR_INTRUSIVE_PTR<Message> message)
    {
        boost::mutex::scoped_lock lock(this_lock);

        if (sending)
        {
            send_queue.push_back(message);
        }
        else
        {
            start_send(message);
            sending = true;
        }
    }

    void start_send(RR_INTRUSIVE_PTR<Message> message)
    {
        message_len = message->header->MessageSize;
        message_pos = 0;
        if (message_len > buffer_len)
        {
            buffer_len = message_len + message_len / 10;
            send_buffer.reset(new uint8_t[buffer_len]);
        }

        ArrayBinaryWriter w(send_buffer.get(), 0, message_len);
        message->Write4(w);

        socket->socket->async_write_some(boost::asio::buffer(send_buffer.get(), message_len),
                                         boost::bind(&LocalMessageTapConnectionImpl::end_send, shared_from_this(),
                                                     boost::asio::placeholders::error,
                                                     boost::asio::placeholders::bytes_transferred));
    }

    void end_send(const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        if (ec)
        {
            return;
        }

        boost::mutex::scoped_lock lock(this_lock);
        message_pos += bytes_transferred;
        if (message_pos < message_len)
        {
            socket->socket->async_write_some(
                boost::asio::buffer(send_buffer.get() + message_pos, message_len - message_pos),
                boost::bind(&LocalMessageTapConnectionImpl::end_send, shared_from_this(),
                            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
            return;
        }

        if (send_queue.empty())
        {
            sending = false;
            return;
        }
        else
        {
            RR_INTRUSIVE_PTR<Message> m = send_queue.front();
            send_queue.pop_front();
            start_send(m);
            return;
        }
    }

    void start_recv()
    {
        socket->socket->async_read_some(boost::asio::buffer(recv_buf, sizeof(recv_buf)),
                                        boost::bind(&LocalMessageTapConnectionImpl::end_recv, shared_from_this(),
                                                    boost::asio::placeholders::error,
                                                    boost::asio::placeholders::bytes_transferred));
    }

    void end_recv(const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        if (ec)
        {
            return;
        }

        boost::mutex::scoped_lock lock(this_lock);
        socket->socket->async_read_some(boost::asio::buffer(recv_buf, sizeof(recv_buf)),
                                        boost::bind(&LocalMessageTapConnectionImpl::end_recv, shared_from_this(),
                                                    boost::asio::placeholders::error,
                                                    boost::asio::placeholders::bytes_transferred));
    }
};

class LocalMessageTapImpl : public RR_ENABLE_SHARED_FROM_THIS<LocalMessageTapImpl>
{
  public:
    RR_SHARED_PTR<LocalTransportFD> all_accept;
    RR_SHARED_PTR<LocalTransportFD> log_accept;

    RR_SHARED_PTR<RR_BOOST_ASIO_IO_CONTEXT> io_context;

    boost::filesystem::path all_tap_fname;
    boost::filesystem::path log_tap_fname;

    boost::atomic<bool> is_open;

    boost::mutex connections_lock;
    std::list<RR_WEAK_PTR<LocalMessageTapConnectionImpl> > connections;

    void Open(const std::string& tap_name)
    {
        is_open.store(true);
        io_context = RR_MAKE_SHARED<RR_BOOST_ASIO_IO_CONTEXT>();

        try
        {
            boost::filesystem::path run_path = LocalTransportUtil::GetUserRunPath();
            boost::filesystem::path tap_path = run_path / "tap";
            boost::filesystem::path all_tap_path = tap_path / "all";
            boost::filesystem::path log_tap_path = tap_path / "log";
            boost::filesystem::create_directories(all_tap_path);
            boost::filesystem::create_directories(log_tap_path);
            all_tap_fname = all_tap_path / (tap_name + ".sock");
            log_tap_fname = log_tap_path / (tap_name + ".sock");

            boost::filesystem::remove(all_tap_fname);
            boost::filesystem::remove(log_tap_fname);

            std::string all_tap_fname_str = all_tap_fname.string();
            std::string log_tap_fname_str = log_tap_fname.string();
            boost::asio::local::stream_protocol::endpoint all_ep(all_tap_fname_str);
            boost::asio::local::stream_protocol::endpoint log_ep(log_tap_fname_str);

            RR_SHARED_PTR<detail::Tap_acceptor> all_acceptor1(new detail::Tap_acceptor(*io_context));
            RR_SHARED_PTR<detail::Tap_acceptor> log_acceptor1(new detail::Tap_acceptor(*io_context));

            all_acceptor1->acceptor.open();
            all_acceptor1->acceptor.bind(all_ep);
            all_acceptor1->acceptor.listen(4);

            log_acceptor1->acceptor.open();
            log_acceptor1->acceptor.bind(log_ep);
            log_acceptor1->acceptor.listen(4);

            RR_SHARED_PTR<detail::Tap_socket> all_socket1(new detail::Tap_socket(*io_context));
            all_acceptor1->acceptor.async_accept(
                *all_socket1->socket, boost::bind(&LocalMessageTapImpl::handle_accept_all, shared_from_this(),
                                                  all_acceptor1, all_socket1, boost::asio::placeholders::error));

            RR_SHARED_PTR<detail::Tap_socket> log_socket1(new detail::Tap_socket(*io_context));
            log_acceptor1->acceptor.async_accept(
                *log_socket1->socket, boost::bind(&LocalMessageTapImpl::handle_accept_log, shared_from_this(),
                                                  log_acceptor1, log_socket1, boost::asio::placeholders::error));

            boost::thread thread(boost::bind(&LocalMessageTapImpl::thread_func, shared_from_this(), io_context,
                                             all_tap_fname, log_tap_fname));
        }
        catch (std::exception& e)
        {
            std::string e_what(e.what());
            throw SystemResourceException("Could not create local sockets for tap: " + e_what);
        }
    }

    void handle_accept_all(RR_SHARED_PTR<detail::Tap_acceptor> acceptor, RR_SHARED_PTR<detail::Tap_socket> socket,
                           const boost::system::error_code& error)
    {
        if (error)
            return;
        if (!is_open.load())
            return;

        try
        {
            RR_SHARED_PTR<LocalMessageTapConnectionImpl> c =
                RR_MAKE_SHARED<LocalMessageTapConnectionImpl>(io_context, true);
            c->AttachSocket(socket);
            {
                boost::mutex::scoped_lock lock(connections_lock);
                connections.push_back(c);
            }
        }
        catch (std::exception&)
        {}

        RR_SHARED_PTR<detail::Tap_socket> all_socket1(new detail::Tap_socket(*io_context));
        acceptor->acceptor.async_accept(*all_socket1->socket,
                                        boost::bind(&LocalMessageTapImpl::handle_accept_all, shared_from_this(),
                                                    acceptor, all_socket1, boost::asio::placeholders::error));
    }

    void handle_accept_log(RR_SHARED_PTR<detail::Tap_acceptor> acceptor, RR_SHARED_PTR<detail::Tap_socket> socket,
                           const boost::system::error_code& error)
    {
        if (error)
            return;
        if (!is_open.load())
            return;

        try
        {
            RR_SHARED_PTR<LocalMessageTapConnectionImpl> c =
                RR_MAKE_SHARED<LocalMessageTapConnectionImpl>(io_context, false);
            c->AttachSocket(socket);
            {
                boost::mutex::scoped_lock lock(connections_lock);
                connections.push_back(c);
            }
        }
        catch (std::exception&)
        {}

        RR_SHARED_PTR<detail::Tap_socket> log_socket1(new detail::Tap_socket(*io_context));
        acceptor->acceptor.async_accept(*log_socket1->socket,
                                        boost::bind(&LocalMessageTapImpl::handle_accept_log, shared_from_this(),
                                                    acceptor, log_socket1, boost::asio::placeholders::error));
    }

    static void thread_func(RR_SHARED_PTR<LocalMessageTapImpl> this1,
                            RR_SHARED_PTR<RR_BOOST_ASIO_IO_CONTEXT> io_context1, boost::filesystem::path all_tap_fname1,
                            boost::filesystem::path log_tap_fname1)
    {
        RR_WEAK_PTR<LocalMessageTapImpl> this2 = this1;
        this1.reset();
        while (true)
        {
            {
                RR_SHARED_PTR<LocalMessageTapImpl> this3 = this2.lock();
                if (!this3)
                {
                    break;
                }

                if (!this3->is_open.load())
                {
                    break;
                }
            }

            try
            {
                if (io_context1->run_one() == 0)
                {
                    break;
                }
            }
            catch (std::exception&)
            {}
        }

        boost::filesystem::remove(all_tap_fname1);
        boost::filesystem::remove(log_tap_fname1);
    }

    void Close()
    {
        is_open.store(false);
        io_context->stop();
        {
            boost::mutex::scoped_lock lock(connections_lock);
            for (std::list<RR_WEAK_PTR<LocalMessageTapConnectionImpl> >::iterator c = connections.begin();
                 c != connections.end();)
            {
                RR_SHARED_PTR<LocalMessageTapConnectionImpl> c1 = c->lock();
                if (c1)
                {
                    c1->Close();
                    ++c;
                }
                else
                {
                    c = connections.erase(c);
                }
            }
        }
    }

    void RecordLogRecord(const RRLogRecord& log_record)
    {
        RR_INTRUSIVE_PTR<Message> message2 = RRLogRecordToMessage(log_record);
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
        message2->header->MetaData =
            message2->header->MetaData.str() + "timestamp: " + boost::posix_time::to_iso_extended_string(now) + "\n";
        message2->ComputeSize4();
        {
            boost::mutex::scoped_lock lock(connections_lock);
            for (std::list<RR_WEAK_PTR<LocalMessageTapConnectionImpl> >::iterator c = connections.begin();
                 c != connections.end();)
            {
                RR_SHARED_PTR<LocalMessageTapConnectionImpl> c1 = c->lock();
                if (c1)
                {
                    c1->RecordMessage(message2);
                    ++c;
                }
                else
                {
                    c = connections.erase(c);
                }
            }
        }
    }

    void RecordMessage(RR_INTRUSIVE_PTR<Message> message)
    {
        RR_INTRUSIVE_PTR<Message> message2 = ShallowCopyMessage(message);
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
        message2->header->MetaData =
            message2->header->MetaData.str() + "timestamp: " + boost::posix_time::to_iso_extended_string(now) + "\n";
        message2->ComputeSize4();
        {
            boost::mutex::scoped_lock lock(connections_lock);
            for (std::list<RR_WEAK_PTR<LocalMessageTapConnectionImpl> >::iterator c = connections.begin();
                 c != connections.end();)
            {
                RR_SHARED_PTR<LocalMessageTapConnectionImpl> c1 = c->lock();
                if (c1)
                {
                    if (c1->LogAll)
                    {
                        c1->RecordMessage(message2);
                    }
                    ++c;
                }
                else
                {
                    c = connections.erase(c);
                }
            }
        }
    }
};
} // namespace detail

LocalMessageTap::LocalMessageTap(const std::string& tap_name) { this->tap_name = tap_name; }
LocalMessageTap::~LocalMessageTap() {}

void LocalMessageTap::Open()
{
    RR_SHARED_PTR<detail::LocalMessageTapImpl> tap = RR_MAKE_SHARED<detail::LocalMessageTapImpl>();
    tap->Open(tap_name);
    this->tap_impl = tap;
}

void LocalMessageTap::Close()
{
    RR_SHARED_PTR<detail::LocalMessageTapImpl> tap = tap_impl.lock();
    if (tap)
    {
        tap->Close();
    }
}

void LocalMessageTap::RecordLogRecord(const RRLogRecord& log_record)
{
    RR_SHARED_PTR<detail::LocalMessageTapImpl> tap = tap_impl.lock();
    if (tap)
    {
        tap->RecordLogRecord(log_record);
    }
}

void LocalMessageTap::RecordMessage(RR_INTRUSIVE_PTR<Message> message)
{
    RR_SHARED_PTR<detail::LocalMessageTapImpl> tap = tap_impl.lock();
    if (tap)
    {
        tap->RecordMessage(message);
    }
}
} // namespace RobotRaconteur
