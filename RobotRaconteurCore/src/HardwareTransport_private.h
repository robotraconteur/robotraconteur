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

#include "RobotRaconteur/HardwareTransport.h"
#include "LocalTransport_private.h"

#pragma once

namespace RobotRaconteur
{

class HardwareTransportConnection : public detail::ASIOStreamBaseTransport
{
  public:
    HardwareTransportConnection(RR_SHARED_PTR<HardwareTransport> parent, bool server, uint32_t local_endpoint);

    virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m);

  protected:
    void AsyncAttachSocket1(std::string noden, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    virtual void Close1() = 0;

    std::string scheme;

  public:
    virtual uint32_t GetLocalEndpoint();

    virtual uint32_t GetRemoteEndpoint();

    virtual void CheckConnection(uint32_t endpoint);

    virtual void Close();

    virtual RR_SHARED_PTR<Transport> GetTransport();

  protected:
    bool server;

    RR_WEAK_PTR<HardwareTransport> parent;

    uint32_t m_RemoteEndpoint;
    uint32_t m_LocalEndpoint;

    boost::recursive_mutex close_lock;
};

class HardwareTransportConnection_driver : public HardwareTransportConnection
{

  public:
#ifdef ROBOTRACONTEUR_WINDOWS
    typedef boost::asio::windows::stream_handle socket_type;
#else
    typedef boost::asio::posix::stream_descriptor socket_type;
#endif
    HardwareTransportConnection_driver(RR_SHARED_PTR<HardwareTransport> parent, bool server, uint32_t local_endpoint,
                                       boost::string_ref scheme);

    void AsyncAttachSocket(RR_SHARED_PTR<HardwareTransportConnection_driver::socket_type> socket, std::string noden,
                           boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

  protected:
    virtual void async_write_some(
        const_buffers& b,
        boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler);

    virtual void async_read_some(
        mutable_buffers& b,
        boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler);

    virtual size_t available();

    virtual void Close1();

  protected:
    RR_SHARED_PTR<HardwareTransportConnection_driver::socket_type> socket;
    boost::mutex socket_lock;
};

class HardwareTransportConnection_bluetooth : public HardwareTransportConnection
{

  public:
    HardwareTransportConnection_bluetooth(RR_SHARED_PTR<HardwareTransport> parent, bool server,
                                          uint32_t local_endpoint);

    void AsyncAttachSocket(RR_SHARED_PTR<boost::asio::generic::stream_protocol::socket> socket, std::string noden,
                           boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

  protected:
    virtual void async_write_some(
        const_buffers& b,
        boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler);

    virtual void async_read_some(
        mutable_buffers& b,
        boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler);

    virtual size_t available();

    virtual void Close1();

  protected:
    RR_SHARED_PTR<boost::asio::generic::stream_protocol::socket> socket;
    boost::mutex socket_lock;
};

void HardwareTransport_attach_transport(
    RR_SHARED_PTR<HardwareTransport> parent, RR_SHARED_PTR<HardwareTransportConnection_driver::socket_type> socket,
    bool server, uint32_t endpoint, std::string noden, boost::string_ref scheme,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& callback);

void HardwareTransport_attach_transport_bluetooth(
    RR_SHARED_PTR<HardwareTransport> parent, RR_SHARED_PTR<boost::asio::generic::stream_protocol::socket> socket,
    bool server, uint32_t endpoint, std::string noden,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& callback);
} // namespace RobotRaconteur
