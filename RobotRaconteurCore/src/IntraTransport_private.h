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

#include "RobotRaconteur/IntraTransport.h"

#pragma once

namespace RobotRaconteur
{
    class ROBOTRACONTEUR_CORE_API IntraTransportConnection : public ITransportConnection, public boost::enable_shared_from_this<IntraTransportConnection>
	{
	public:

		IntraTransportConnection(RR_SHARED_PTR<IntraTransport> parent, bool server, uint32_t local_endpoint);
		
		virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m);	

        virtual void AcceptMessage(RR_INTRUSIVE_PTR<Message> m);	
				
	public:

        virtual void SendMessage(RR_INTRUSIVE_PTR<Message> m);

		virtual void AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )>& handler);

		virtual void Close();

		virtual  uint32_t GetLocalEndpoint();

		virtual  uint32_t GetRemoteEndpoint();

        virtual  NodeID GetRemoteNodeID();

        virtual RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		virtual void CheckConnection(uint32_t endpoint);

        virtual bool CheckCapabilityActive(uint32_t flag);

        void SetPeer(RR_SHARED_PTR<IntraTransportConnection> peer);

        bool IsConnected();

        virtual RR_SHARED_PTR<Transport> GetTransport();

	protected:

        static void ProcessNextRecvMessage(RR_WEAK_PTR<IntraTransportConnection> c);

        void SimpleAsyncEndSendMessage(RR_SHARED_PTR<RobotRaconteurException> err);

        void RemoteClose();

        static void RemoteClose1(RR_WEAK_PTR<IntraTransportConnection> c);

		bool server;

		RR_WEAK_PTR<IntraTransport> parent;
        RR_WEAK_PTR<RobotRaconteurNode> node;

		uint32_t m_RemoteEndpoint;
		uint32_t m_LocalEndpoint;
        NodeID RemoteNodeID;
		boost::shared_mutex RemoteNodeID_lock;

		boost::recursive_mutex close_lock;

        RR_WEAK_PTR<IntraTransportConnection> peer;
        RR_SHARED_PTR<IntraTransportConnection> peer_storage;

        boost::atomic<bool> connected;

        boost::mutex recv_queue_lock;
        std::list<RR_INTRUSIVE_PTR<Message> > recv_queue;
        bool recv_queue_post_requested;
		
	};
}

