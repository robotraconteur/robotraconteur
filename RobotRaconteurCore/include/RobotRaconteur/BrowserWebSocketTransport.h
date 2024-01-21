/**
 * @file BrowserWebSocketTransport.h
 *
 * @author John Wason, PhD
 *
 * @copyright Copyright 2011-2023 Wason Technology, LLC
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

    class BrowserWebSocketTransport : public Transport, public RR_ENABLE_SHARED_FROM_THIS<BrowserWebSocketTransport>
    {
        public:

            friend class BrowserWebSocketTransportConnection;

		    RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> > TransportConnections;
		
            BrowserWebSocketTransport(RR_SHARED_PTR<RobotRaconteurNode> node=RobotRaconteurNode::sp());

            RR_OVIRTUAL ~BrowserWebSocketTransport() RR_OVERRIDE;

            RR_OVIRTUAL bool IsServer() const RR_OVERRIDE;
            RR_OVIRTUAL bool IsClient() const RR_OVERRIDE;
            
            virtual int32_t GetDefaultReceiveTimeout();
            virtual void SetDefaultReceiveTimeout(int32_t milliseconds);
            virtual int32_t GetDefaultConnectTimeout();
            virtual void SetDefaultConnectTimeout(int32_t milliseconds);

            RR_OVIRTUAL std::string GetUrlSchemeString() const RR_OVERRIDE;

            RR_OVIRTUAL void SendMessage(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;
            
            RR_OVIRTUAL void AsyncSendMessage(const RR_INTRUSIVE_PTR<Message>& m, const boost::function<void (const RR_SHARED_PTR<RobotRaconteurException>& )>& callback) RR_OVERRIDE;

            RR_OVIRTUAL void AsyncCreateTransportConnection(boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e, boost::function<void (const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>& ) >& callback) RR_OVERRIDE;

            RR_OVIRTUAL RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(
                boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE;

            RR_OVIRTUAL void CloseTransportConnection(const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE;

            RR_OVIRTUAL bool CanConnectService(boost::string_ref url) RR_OVERRIDE;
		
            RR_OVIRTUAL void Close() RR_OVERRIDE;

		    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE;

            RR_OVIRTUAL void PeriodicCleanupTask() RR_OVERRIDE;

            RR_OVIRTUAL uint32_t TransportCapability(boost::string_ref name) RR_OVERRIDE;

            RR_OVIRTUAL void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;

            virtual int32_t GetDefaultHeartbeatPeriod();
            virtual void SetDefaultHeartbeatPeriod(int32_t milliseconds);

            virtual bool GetDisableMessage4();
            virtual void SetDisableMessage4(bool d);

            virtual bool GetDisableStringTable();
            virtual void SetDisableStringTable(bool d);

            virtual bool GetDisableAsyncMessageIO();
            virtual void SetDisableAsyncMessageIO(bool d);
            
            virtual int32_t GetMaxMessageSize();            
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
}