// Copyright 2011-2023 Wason Technology, LLC
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

#include "RobotRaconteur/BrowserWebSocketTransport.h"
#include "RobotRaconteur/ASIOStreamBaseTransport.h"
#include <list>

#include <emscripten/websocket.h>

#pragma once

namespace RobotRaconteur
{
class BrowserWebSocketTransportConnection : public detail::ASIOStreamBaseTransport
{
    friend EM_BOOL websocket_open_callback_func(int eventType, const EmscriptenWebSocketOpenEvent* websocketEvent,
                                                void* userData);

    friend EM_BOOL websocket_message_callback_func(int eventType, const EmscriptenWebSocketMessageEvent* websocketEvent,
                                                   void* userData);

    friend EM_BOOL websocket_error_callback_func(int eventType, const EmscriptenWebSocketErrorEvent* websocketEvent,
                                                 void* userData);

    friend EM_BOOL websocket_close_callback_func(int eventType, const EmscriptenWebSocketCloseEvent* websocketEvent,
                                                 void* userData);

  public:
    friend class BrowserWebSocketTransport;

  protected:
    static std::map<void*, RR_SHARED_PTR<BrowserWebSocketTransportConnection> > active_transports;

    EMSCRIPTEN_WEBSOCKET_T socket;

    std::string url;

    RR_WEAK_PTR<BrowserWebSocketTransport> parent;

    uint32_t m_RemoteEndpoint;
    uint32_t m_LocalEndpoint;

    bool closing;

    struct recv_buf_entry
    {
        boost::asio::const_buffer buffer;
        boost::shared_array<uint8_t> data_storage;
    };

    std::list<recv_buf_entry> recv_buf;

    boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> active_read_handler;
    mutable_buffers active_read_buffers;

    boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> active_connect_handler;

    void websocket_message_received(const void* data, size_t len);

  public:
    BrowserWebSocketTransportConnection(RR_SHARED_PTR<BrowserWebSocketTransport> parent, const std::string& url,
                                        uint32_t local_endpoint);

    virtual void AsyncConnect(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> err);

    virtual void AsyncConnect1();

    RR_OVIRTUAL void Close() RR_OVERRIDE;

    RR_OVIRTUAL uint32_t GetLocalEndpoint() RR_OVERRIDE;

    RR_OVIRTUAL uint32_t GetRemoteEndpoint() RR_OVERRIDE;

    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE;

    RR_OVIRTUAL void async_write_some(
        const_buffers& b,
        const boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
        RR_OVERRIDE;

    RR_OVIRTUAL void async_read_some(
        mutable_buffers& b,
        const boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
        RR_OVERRIDE;

    RR_OVIRTUAL size_t available() RR_OVERRIDE;

    RR_OVIRTUAL bool IsLargeTransferAuthorized() RR_OVERRIDE;

    RR_OVIRTUAL void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;

    RR_OVIRTUAL RR_SHARED_PTR<Transport> GetTransport() RR_OVERRIDE;
};
} // namespace RobotRaconteur