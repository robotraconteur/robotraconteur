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

#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/ServiceFactory.h"
#include "RobotRaconteur/ServiceDefinition.h"
#include "RobotRaconteur/ServiceStructure.h"
#include <boost/atomic.hpp>

#pragma warning(push)
#pragma warning(disable:4996) 
#include <boost/signals2.hpp>

namespace RobotRaconteur
{
	class ROBOTRACONTEUR_CORE_API  ClientContext;
	class ROBOTRACONTEUR_CORE_API  PipeClientBase;
	class ROBOTRACONTEUR_CORE_API  WireClientBase;

	class ROBOTRACONTEUR_CORE_API  ServiceStub : public virtual RRObject, public RR_ENABLE_SHARED_FROM_THIS<ServiceStub>
	{
	
	public:
		ServiceStub(const std::string &path, RR_SHARED_PTR<ClientContext> c);

		std::string ServicePath;

		RR_WEAK_PTR<ClientContext> context;

		RR_SHARED_PTR<boost::recursive_mutex> RRMutex;

		RR_SHARED_PTR<ClientContext> GetContext();

		RR_INTRUSIVE_PTR<MessageEntry> ProcessRequest(RR_INTRUSIVE_PTR<MessageEntry> m);

		void AsyncProcessRequest(RR_INTRUSIVE_PTR<MessageEntry> m, RR_MOVE_ARG(boost::function<void(RR_INTRUSIVE_PTR<MessageEntry>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);

		virtual void DispatchEvent(RR_INTRUSIVE_PTR<MessageEntry> m) = 0;

		RR_SHARED_PTR<RRObject> FindObjRef(const std::string &n);

		void AsyncFindObjRef(const std::string &n, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);

		RR_SHARED_PTR<RRObject> FindObjRef(const std::string &n, const std::string &i);

		RR_SHARED_PTR<RRObject> FindObjRefTyped(const std::string &n, const std::string& objecttype);

		RR_SHARED_PTR<RRObject> FindObjRefTyped(const std::string &n, const std::string &i, const std::string& objecttype);

		void AsyncFindObjRef(const std::string &n, const std::string &i, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);

		void AsyncFindObjRefTyped(const std::string &n, const std::string& objecttype, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);

		void AsyncFindObjRefTyped(const std::string &n, const std::string &i, const std::string& objecttype, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);

		template <typename T>
		static void EndAsyncFindObjRef(RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<T>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);

		std::string FindObjectType(const std::string &n);

		std::string FindObjectType(const std::string &n, const std::string &i);

		void AsyncFindObjectType(const std::string &n, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);

		void AsyncFindObjectType(const std::string &n, const std::string &i, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);

		void AsyncSendPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, bool unreliable, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) >& handler);

		void SendWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m);

		virtual void DispatchPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m);

		virtual void DispatchWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m);

		virtual RR_INTRUSIVE_PTR<MessageEntry> CallbackCall(RR_INTRUSIVE_PTR<MessageEntry> m);

		virtual std::string RRType() { return "RobotRaconteur.ServiceStub"; }

		virtual void RRClose();

		virtual void RRInitStub() = 0;

		virtual RR_SHARED_PTR<PipeClientBase> RRGetPipeClient(const std::string& membername);

		virtual RR_SHARED_PTR<WireClientBase> RRGetWireClient(const std::string& membername);

		RR_SHARED_PTR<RobotRaconteurNode> RRGetNode();
		RR_WEAK_PTR<RobotRaconteurNode> RRGetNodeWeak();
	protected:
		RR_WEAK_PTR<RobotRaconteurNode> node;
	};

	class ROBOTRACONTEUR_CORE_API  ClientContext : public Endpoint, public RR_ENABLE_SHARED_FROM_THIS<ClientContext>
	{
	protected:
		RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceStub> > stubs;
		boost::mutex stubs_lock;
		std::list<std::string> active_stub_searches;
		std::list<boost::tuple<std::string, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> > > active_stub_searches_handlers;
		
		RR_SHARED_PTR<ServiceFactory> m_ServiceDef;

	public:
		RR_SHARED_PTR<ServiceFactory> GetServiceDef() const;

		ClientContext(RR_SHARED_PTR<RobotRaconteurNode> node);

		ClientContext(RR_SHARED_PTR<ServiceFactory> service_def, RR_SHARED_PTR<RobotRaconteurNode> node);

		RR_SHARED_PTR<RRObject> FindObjRef(const std::string &path, const std::string& objecttype = "");

		void AsyncFindObjRef(const std::string &path, const std::string& objecttype2, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);


		template <typename T>
		static void EndAsyncFindObjRef(RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<T>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
		{
			if (err)
			{
				handler(RR_SHARED_PTR<T>(), err);
				return;
			}
			else
			{
				RR_SHARED_PTR<T> castobj=RR_DYNAMIC_POINTER_CAST<T>(obj);
				if (!castobj)
				{
					handler(RR_SHARED_PTR<T>(), RR_MAKE_SHARED<DataTypeMismatchException>("Invalid object type returned"));
					return;
				}
				handler(castobj, RR_SHARED_PTR<RobotRaconteurException>());
			}

		}

	protected:
		void AsyncFindObjRef1(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, const std::string& path, const std::string& objecttype2, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler, int32_t timeout);

		void AsyncFindObjRef2(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > ret, RR_SHARED_PTR<RobotRaconteurException> err, const std::string& objecttype, const std::string& objectdef, const std::string& path, const std::string& objecttype2, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler, int32_t timeout);

		void AsyncFindObjRef3(RR_SHARED_PTR<RRObject> ret, RR_SHARED_PTR<RobotRaconteurException> err, const std::string& path, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);

	public:

		std::string FindObjectType(const std::string &path);

		void AsyncFindObjectType(const std::string &path, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);


		bool VerifyObjectImplements(const std::string& objecttype, const std::string& implementstype);

	protected:
		void AsyncFindObjectType1(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);

	private:
		
		boost::mutex pulled_service_defs_lock;
		std::map<std::string, RR_SHARED_PTR<ServiceDefinition> > pulled_service_defs;
		
		bool VerifyObjectImplements2(const std::string& objecttype, const std::string& implementstype);

		struct outstanding_request
		{
			RR_INTRUSIVE_PTR<MessageEntry> ret;
			RR_SHARED_PTR<AutoResetEvent> evt;
			boost::function<void(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException>)> handler;
			RR_SHARED_PTR<Timer> timer;
		};

		boost::mutex outstanding_requests_lock;
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_request> > outstanding_requests;


		boost::mutex FindObjRef_lock;

	public:
		RR_INTRUSIVE_PTR<MessageEntry> ProcessRequest(RR_INTRUSIVE_PTR<MessageEntry> m);
		
		void AsyncProcessRequest(RR_INTRUSIVE_PTR<MessageEntry> m, RR_MOVE_ARG(boost::function<void(RR_INTRUSIVE_PTR<MessageEntry>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);

		using Endpoint::SendMessage;
		void SendMessage(RR_INTRUSIVE_PTR<MessageEntry> m);

		using Endpoint::AsyncSendMessage;
		void AsyncSendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

	private:

		void AsyncProcessRequest_err(RR_SHARED_PTR<RobotRaconteurException> error, uint32_t requestid);

		void AsyncProcessRequest_timeout(const TimerEvent& error, uint32_t requestid);

		uint32_t request_number;
		
		RR_SHARED_PTR<Timer> connection_test_timer;

		void connection_test(const TimerEvent& ev);

	public:
		virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m);

	protected:
		void MessageEntryReceived(RR_INTRUSIVE_PTR<MessageEntry> m);
				
	protected:
		std::string m_ServiceName;
	public:
		std::string GetServiceName() const;

		bool GetConnected() const;
	protected:
		bool m_Connected;
		boost::mutex m_Connected_lock;
		boost::mutex close_lock;


	private:
		RR_SHARED_PTR<Transport> connecttransport;
		std::string connecturl;
		
	public:

		std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > GetAttributes();
		
	private:
		std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > m_Attributes;
		boost::mutex m_Attributes_lock;

	public:
		RR_SHARED_PTR<RRObject> ConnectService(RR_SHARED_PTR<Transport> c, const std::string &url, const std::string &username = "", RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials = (RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >()), const std::string& objecttype = "");

		RR_SHARED_PTR<RRObject> ConnectService(RR_SHARED_PTR<Transport> c, RR_SHARED_PTR<ITransportConnection> tc, const std::string &url, const std::string &username = "", RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials = (RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >()), const std::string& objecttype = "");

		void AsyncConnectService(RR_SHARED_PTR<Transport> c, const std::string &url, const std::string &username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);

		void AsyncConnectService(RR_SHARED_PTR<Transport> c, RR_SHARED_PTR<ITransportConnection> tc, const std::string &url, const std::string &username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);

	protected:

		//These functions are part of the connection process
		void AsyncConnectService1(RR_SHARED_PTR<Transport> c, RR_SHARED_PTR<ITransportConnection> tc, RR_SHARED_PTR<RobotRaconteurException> e, const std::string& url, const std::string& username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);

		void AsyncConnectService2(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, RR_SHARED_PTR<RobotRaconteurException> e, const std::string & username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);

		void AsyncConnectService3(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> e, const std::string& username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype, RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);

		void AsyncConnectService4(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, RR_SHARED_PTR<RobotRaconteurException> e, const std::string& username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype, const std::string& type, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);

		void AsyncConnectService5(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> e, const std::string& username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype, const std::string& type, RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);

		void AsyncConnectService6(RR_SHARED_PTR<std::string> ret, RR_SHARED_PTR<RobotRaconteurException> e, const std::string& type, RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);

		void AsyncConnectService7(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> e, const std::string& objecttype, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);

		boost::recursive_mutex connect_lock;

	public:
		
		void Close();

		void AsyncClose(RR_MOVE_ARG(boost::function<void()>) handler);
	protected:

		void AsyncClose1(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void()>& handler);

	public:

		void AsyncSendPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, bool unreliable, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) >& handler);


		void SendWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m);

		RR_SHARED_PTR<ServiceDefinition> PullServiceDefinition(const std::string &ServiceType = "");

		void AsyncPullServiceDefinition(const std::string &ServiceType, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<ServiceDefinition>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);
	protected:

		void AsyncPullServiceDefinition1(RR_INTRUSIVE_PTR<MessageEntry> ret3, RR_SHARED_PTR<RobotRaconteurException> err, const std::string& ServiceType, boost::function<void(RR_SHARED_PTR<ServiceDefinition>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);

	public:

		std::vector<RR_SHARED_PTR<ServiceDefinition> > PullServiceDefinitionAndImports(const std::string &servicetype = "");

		void AsyncPullServiceDefinitionAndImports(const std::string &servicetype, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > >, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);
	protected:
		void AsyncPullServiceDefinitionAndImports1(RR_SHARED_PTR<ServiceDefinition> root, RR_SHARED_PTR<RobotRaconteurException> err, const std::string& servicetype, boost::function<void(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > >, RR_SHARED_PTR<RobotRaconteurException>)>& handler, boost::posix_time::ptime timeout_time);
		void AsyncPullServiceDefinitionAndImports2(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > imported, RR_SHARED_PTR<RobotRaconteurException> err, boost::tuple<std::string, RR_SHARED_PTR<ServiceDefinition>, RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > >, std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator, boost::function<void(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > >, RR_SHARED_PTR<RobotRaconteurException>)>, boost::posix_time::ptime> args);
	public:

		std::vector<std::string> GetPulledServiceTypes();

		RR_SHARED_PTR<ServiceFactory> GetPulledServiceType(const std::string& type);

	private:

		boost::mutex pulled_service_types_lock;
		RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceFactory> > pulled_service_types;

		bool use_pulled_types;

	private:

		RR_SHARED_PTR<ITransportConnection> TransportConnection;

		boost::mutex m_Authentication_lock;

		bool m_UserAuthenticated;

	public:
		const bool GetUserAuthenticated();
	private:
		std::string m_AuthenticatedUsername;
	public:
		const std::string GetAuthenticatedUsername();

		std::string AuthenticateUser(const std::string &username, RR_INTRUSIVE_PTR<RRValue> credentials);

		void AsyncAuthenticateUser(const std::string &username, RR_INTRUSIVE_PTR<RRValue> credentials, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);

	protected:
		void AsyncAuthenticateUser2(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, const std::string& username, boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);
	public:

		std::string LogoutUser();

		std::string RequestObjectLock(RR_SHARED_PTR<RRObject> obj, RobotRaconteurObjectLockFlags flags);

		void AsyncRequestObjectLock(RR_SHARED_PTR<RRObject> obj, RobotRaconteurObjectLockFlags flags, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);
	protected:
		void EndAsyncLockOp(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);
	public:
		std::string ReleaseObjectLock(RR_SHARED_PTR<RRObject> obj);

		void AsyncReleaseObjectLock(RR_SHARED_PTR<RRObject> obj, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);

		std::string MonitorEnter(RR_SHARED_PTR<RRObject> obj, int32_t timeout);

		void MonitorExit(RR_SHARED_PTR<RRObject> obj);

	public:
		virtual void PeriodicCleanupTask();

		uint32_t CheckServiceCapability(const std::string &name);



		//		public event ClientServiceListenerDelegate ClientServiceListener;

		boost::signals2::signal<void(RR_SHARED_PTR<ClientContext>, ClientServiceListenerEventType, RR_SHARED_PTR<void>)> ClientServiceListener;

	protected:
		void ProcessCallbackCall(RR_INTRUSIVE_PTR<MessageEntry> m);

	protected:
		boost::atomic<bool> use_message3;
		boost::atomic<bool> use_combined_connection;

	public:
		bool UseMessage3();

	private:
		void InitializeInstanceFields();

	public:

		virtual void TransportConnectionClosed(uint32_t endpoint);

		virtual void TransportConnectionClosed1();

	};

	template <typename T>
	void ServiceStub::EndAsyncFindObjRef(RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<T>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		ClientContext::EndAsyncFindObjRef(obj, err, handler);
	}

}