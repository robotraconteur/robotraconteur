/**
 * @file BrowserWebSocketTransport.h
 *
 * @author John Wason, PhD
 *
 * @copyright Copyright 2011-2024 Wason Technology, LLC
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
#include <boost/shared_array.hpp>

#pragma once

namespace RobotRaconteur
{

class BrowserWebSocketTransportConnection;
/**
 * @brief Transport for creating client connections inside a web browser using WebSockets
 * 
 * Robot Raconteur can be compiled to run inside a web browser using Emscripten and WebAssembly (WASM).
 * While inside a web browser, the only connection method currently available to connection to
 * a Robot Raconteur service is using WebSockets. The BrowserWebSocketTransport class implements
 * the WebSocket transport for the web browser. Currently only the client side is implemented.
 * 
 * See \ref robotraconteur_url for more information on URLs.
 * 
 * Currently the url connections schemes `rr+ws`, `rr+wss` and `rr+tcp` are supported.
 * `rr+tcp` are treated as `rr+ws` connections.
 * 
 * BrowserWebSocketTransport must be manually registered with the node using
 * `RobotRaconteurNode::RegisterTransport()`. NodeSetup is not currently
 * available in the web browser.
 * 
 * Note that for services to accept a WebSocket connection, the service must
 * have the WebSocket "origin" configured correctly. The origin is the base
 * URL of the web page that is hosting the web page that is connecting to the
 * service. For example, if the web page is hosted at `https://example.com/application/index.html`,
 * the origin would be `https://example.com`. For localhost, the origin is `http://localhost:8080`,
 * where 8080 is the port the web page is hosted on. The origin can be configured
 * using the function `TcpTransport::AddWebSocketAllowedOrigin()`, or using
 * the `--robotraconteur-tcp-ws-add-origin` command line option if a node setup class is used.
 * If a local file is used to host the web page, the origin is `null` and no origin
 * checking is performed.
 * 
 * See `TcpTransport::AddWebSocketAllowedOrigin()` for more information on configuring
 * the WebSocket origin and the default origins that are automatically configured.
 */
class BrowserWebSocketTransport : public Transport, public RR_ENABLE_SHARED_FROM_THIS<BrowserWebSocketTransport>
{
  public:
    friend class BrowserWebSocketTransportConnection;

    RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> > TransportConnections;

    /**
     * @brief Construct a new BrowserWebSocketTransport
     *
     * Must use boost::make_shared<BrowserWebSocketTransport>()
     *
     *
     * The transport must be registered with the node using
     * RobotRaconteurNode::RegisterTransport() after construction.
     *
     * @param node The node that will use the transport. Default is the singleton node
     */
    BrowserWebSocketTransport(RR_SHARED_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp());

    RR_OVIRTUAL ~BrowserWebSocketTransport() RR_OVERRIDE;

    RR_OVIRTUAL bool IsServer() const RR_OVERRIDE;
    RR_OVIRTUAL bool IsClient() const RR_OVERRIDE;

    virtual int32_t GetDefaultReceiveTimeout();
    virtual void SetDefaultReceiveTimeout(int32_t milliseconds);
    virtual int32_t GetDefaultConnectTimeout();
    virtual void SetDefaultConnectTimeout(int32_t milliseconds);

    RR_OVIRTUAL std::string GetUrlSchemeString() const RR_OVERRIDE;

    RR_OVIRTUAL void SendMessage(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;

    RR_OVIRTUAL void AsyncSendMessage(
        const RR_INTRUSIVE_PTR<Message>& m,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback) RR_OVERRIDE;

    RR_OVIRTUAL void AsyncCreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>& callback) RR_OVERRIDE;

    RR_OVIRTUAL RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE;

    RR_OVIRTUAL void CloseTransportConnection(const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE;

    RR_OVIRTUAL bool CanConnectService(boost::string_ref url) RR_OVERRIDE;

    RR_OVIRTUAL void Close() RR_OVERRIDE;

    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE;

    RR_OVIRTUAL void PeriodicCleanupTask() RR_OVERRIDE;

    RR_OVIRTUAL uint32_t TransportCapability(boost::string_ref name) RR_OVERRIDE;

    RR_OVIRTUAL void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;

    /**
     * @brief Get the default heartbeat period
     *
     * The heartbeat is used to keep the connection alive
     * if no communication is occuring between nodes.
     *
     * Default: 5 seconds
     *
     * @return int32_t The period in milliseconds
     */
    virtual int32_t GetDefaultHeartbeatPeriod();
    /**
     * @brief Set the default heartbeat period
     *
     * The heartbeat is used to keep the connection alive
     * if no communication is occuring between nodes.
     *
     * Default: 5 seconds
     *
     * @param milliseconds The heartbeat in milliseconds
     */
    virtual void SetDefaultHeartbeatPeriod(int32_t milliseconds);

    /**
     * @brief Get disable Message Format Version 4
     *
     * Message Format Version 2 will be used
     *
     * Default: Message V4 is enabled
     *
     * @return true Disable Message V4
     * @return false Enable Message V4
     */
    virtual bool GetDisableMessage4();
    /**
     * @brief Set disable Message Format Version 4
     *
     * Message Format Version 2 will be used
     *
     * Default: Message V4 is enabled
     *
     * @param d If true, Message V4 is disabled
     */
    virtual void SetDisableMessage4(bool d);

    /**
     * @brief Get disable string table
     *
     * Default: false
     *
     * RobotRaconteurNodeSetup and its subclasses
     * will disable the string table by default
     *
     * @return true Disable the string table
     * @return false String table is not disabled
     */
    virtual bool GetDisableStringTable();
    /**
     * @brief Set disable string table
     *
     * Default: false
     *
     * RobotRaconteurNodeSetup and its subclasses
     * will disable the string table by default
     *
     * @param d If true, string table is disabled
     */
    virtual void SetDisableStringTable(bool d);

    /**
     * @brief Get if async message io is disabled
     *
     * Async message io has better memory handling, at the
     * expense of slightly higher latency.
     *
     * Default: Async io disabled
     *
     * @return true Async message io is disabled
     * @return false Async message io is not disabled
     */
    virtual bool GetDisableAsyncMessageIO();
    /**
     * @brief Set if async message io is disabled
     *
     * Async message io is not well supported 
     * by this transport. Enabling async message io
     * is not recommended.
     * 
     * Async message io has better memory handling, at the
     * expense of slightly higher latency.
     *
     * Default: Async io disabled
     *
     * @param d If true, async io is disabled
     */
    virtual void SetDisableAsyncMessageIO(bool d);

    /**
     * @brief Get the maximum serialized message size
     *
     * Default: 10 MB
     *
     * @return int32_t The size in bytes
     */
    virtual int32_t GetMaxMessageSize();
    /**
     * @brief Set the maximum serialized message size
     *
     * Default: 10 MB
     *
     * @param size The size in bytes
     */
    virtual void SetMaxMessageSize(int32_t size);

    RR_OVIRTUAL std::vector<std::string> GetServerListenUrls() RR_OVERRIDE;

  protected:
    virtual void register_transport(RR_SHARED_PTR<ITransportConnection> connection);
    virtual void erase_transport(RR_SHARED_PTR<ITransportConnection> connection);

    bool closed;
    int32_t heartbeat_period;
    int32_t default_connect_timeout;
    int32_t default_receive_timeout;
    bool disable_message4;
    bool disable_string_table;
    bool disable_async_message_io;
    int32_t max_message_size;
};
} // namespace RobotRaconteur