// Copyright 2011-2018 Wason Technology, LLC
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

#pragma once

#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/Endpoint.h"
#include "RobotRaconteur/PeriodicCleanupTask.h"
#include <boost/date_time.hpp>

#include "RobotRaconteur/Timer.h"

#pragma warning(push) 
#pragma warning(disable:4996) 
#include <boost/signals2.hpp>

#ifdef ROBOTRACONTEUR_WINDOWS
#undef SendMessage
#endif

namespace RobotRaconteur
{

	enum TransportListenerEventType
	{
		TransportListenerEventType_TransportClosed = 1,
		TransportListenerEventType_TransportConnectionClosed
	};

	class ROBOTRACONTEUR_CORE_API RobotRaconteurNode;
	class ROBOTRACONTEUR_CORE_API Timer;
	struct ROBOTRACONTEUR_CORE_API TimerEvent;
	class ROBOTRACONTEUR_CORE_API AutoResetEvent;

	class ROBOTRACONTEUR_CORE_API ITransportTimeProvider
	{
	public:
		virtual boost::posix_time::ptime NowUTC()=0;

		virtual RR_SHARED_PTR<Timer> CreateTimer(const boost::posix_time::time_duration& duration, boost::function<void(const TimerEvent&)>& handler, bool oneshot=false)=0;

		virtual RR_SHARED_PTR<Rate> CreateRate(double frequency)=0;

		virtual void Sleep(const boost::posix_time::time_duration& duration)=0;

		virtual RR_SHARED_PTR<AutoResetEvent> CreateAutoResetEvent()=0;

		virtual ~ITransportTimeProvider() {}
	};


	class ROBOTRACONTEUR_CORE_API ITransportConnection : boost::noncopyable
	{
	 public:

		virtual ~ITransportConnection() {}

		virtual void SendMessage(RR_INTRUSIVE_PTR<Message> m) = 0;

		virtual void AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )>& handler) = 0;

		virtual void Close() = 0;

		virtual void CheckConnection(uint32_t endpoint) = 0;

		virtual  uint32_t GetLocalEndpoint() = 0;

		virtual  uint32_t GetRemoteEndpoint() = 0;

		virtual  NodeID GetRemoteNodeID() = 0;

		virtual RR_SHARED_PTR<RobotRaconteurNode> GetNode() = 0;

		virtual bool CheckCapabilityActive(uint32_t flag) = 0;
	};


	class ROBOTRACONTEUR_CORE_API NodeDiscoveryInfo;

	class ROBOTRACONTEUR_CORE_API Transport : public IPeriodicCleanupTask, boost::noncopyable
	{
	public:
		friend class RobotRaconteurNode;
		friend class ITransport;
		virtual ~Transport() {}

	protected:

		RR_WEAK_PTR<RobotRaconteurNode> node;

	public:

		Transport(RR_SHARED_PTR<RobotRaconteurNode> node);
		
		static boost::thread_specific_ptr<std::string> m_CurrentThreadTransportConnectionURL;
	
		static std::string GetCurrentTransportConnectionURL();
		
	public:

		static boost::thread_specific_ptr<RR_SHARED_PTR<ITransportConnection> > m_CurrentThreadTransport;

	public:
	   static RR_SHARED_PTR<ITransportConnection> GetCurrentThreadTransport();

//		public event MessageHandler MessageReceivedEvent;

	public:
		uint32_t TransportID;

	public:
		virtual void CheckConnection(uint32_t endpoint)=0;

		virtual bool IsClient() const = 0;

		virtual bool IsServer() const = 0;

		virtual std::string GetUrlSchemeString() const = 0;

		//public abstract string Scheme { get; }

		virtual bool CanConnectService(const std::string& url) = 0;

		virtual RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(const std::string& url, RR_SHARED_PTR<Endpoint> e) = 0;

		virtual void AsyncCreateTransportConnection(const std::string& url, RR_SHARED_PTR<Endpoint> e, boost::function<void (RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException> ) >& handler)=0;

		virtual void CloseTransportConnection(RR_SHARED_PTR<Endpoint> e) = 0;

	
		virtual void SendMessage(RR_INTRUSIVE_PTR<Message> m)=0;

		virtual void AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )>& handler) = 0;

	public:
		virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m)=0;

		RR_INTRUSIVE_PTR<Message> SpecialRequest(RR_INTRUSIVE_PTR<Message> m, RR_SHARED_PTR<ITransportConnection> tc);

	public:
		virtual void Close();


		virtual void PeriodicCleanupTask();


		virtual uint32_t TransportCapability(const std::string& name);

		//typedef void (*TransportListenerDelegate)(const RR_SHARED_PTR<Transport> &transport, TransportListenerEventType ev, const RR_SHARED_PTR<void> &parameter);

		boost::signals2::signal<void (RR_SHARED_PTR<Transport> transport, TransportListenerEventType ev, RR_SHARED_PTR<void> parameter   )> TransportListeners;

		virtual RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		virtual std::vector<NodeDiscoveryInfo> GetDetectedNodes(const std::vector<std::string>& schemes);

		virtual void AsyncGetDetectedNodes(const std::vector<std::string>& schemes, boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)>& handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		
	protected:

		virtual void LocalNodeServicesChanged();

		void FireTransportEventListener(RR_SHARED_PTR<Transport> shared_this, TransportListenerEventType ev, RR_SHARED_PTR<void> parameter);

		void TransportConnectionClosed(uint32_t endpoint);
	};

	struct ROBOTRACONTEUR_CORE_API ParseConnectionURLResult
	{
		ParseConnectionURLResult() : port(0) {}

		std::string scheme;
		std::string host;
		int32_t port;
		std::string path;
		NodeID nodeid;
		std::string nodename;
		std::string service;
	};

	ROBOTRACONTEUR_CORE_API ParseConnectionURLResult ParseConnectionURL(const std::string& url);

}

#pragma warning(pop)