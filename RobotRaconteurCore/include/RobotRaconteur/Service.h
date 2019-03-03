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
#include <istream>
#include <boost/atomic.hpp>

#pragma warning(push)
#pragma warning(disable:4996) 
#include <boost/signals2.hpp>


namespace RobotRaconteur
{
	
	class ROBOTRACONTEUR_CORE_API  ServerEndpoint;
	class ROBOTRACONTEUR_CORE_API  AuthenticatedUser;	
	class ROBOTRACONTEUR_CORE_API  ServerContext;
	class ROBOTRACONTEUR_CORE_API  ServerContext_ObjectLock;
	class ROBOTRACONTEUR_CORE_API  ServerContext_MonitorObjectSkel;
	
	class ROBOTRACONTEUR_CORE_API  GeneratorServerBase;


	class ROBOTRACONTEUR_CORE_API  IRobotRaconteurMonitorObject;

	class ROBOTRACONTEUR_CORE_API  ServiceSkel : public RR_ENABLE_SHARED_FROM_THIS<ServiceSkel>, private boost::noncopyable
	{

	public:

		virtual ~ServiceSkel() {}

		virtual void Init(const std::string &s, RR_SHARED_PTR<RRObject> o, RR_SHARED_PTR<ServerContext> c);

		virtual void InitCallbackServers(RR_SHARED_PTR<RRObject> o);



		std::string GetServicePath() const;
	protected:
		std::string m_ServicePath;

	public:
		RR_SHARED_PTR<ServerContext> GetContext() ;
	protected:
		RR_WEAK_PTR<ServerContext> m_context;

		RR_SHARED_PTR<RRObject> uncastobj;

		std::vector<boost::tuple<RobotRaconteurVersion, std::string> > object_type_ver;

		boost::unordered_map<int32_t, RR_SHARED_PTR<GeneratorServerBase> > generators;
		boost::mutex generators_lock;

		int32_t get_new_generator_index();

	public:
		RR_SHARED_PTR<RRObject> GetUncastObject() const;

		virtual RR_INTRUSIVE_PTR<MessageEntry> CallGetProperty(RR_INTRUSIVE_PTR<MessageEntry> m) = 0;


		virtual RR_INTRUSIVE_PTR<MessageEntry> CallSetProperty(RR_INTRUSIVE_PTR<MessageEntry> m) = 0;

		virtual RR_INTRUSIVE_PTR<MessageEntry> CallFunction(RR_INTRUSIVE_PTR<MessageEntry> m) = 0;

	protected:

		static void EndAsyncCallGetProperty(RR_WEAK_PTR<ServiceSkel> skel, RR_INTRUSIVE_PTR<MessageElement> value, RR_SHARED_PTR<RobotRaconteurException> err, RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep);

		static void EndAsyncCallSetProperty(RR_WEAK_PTR<ServiceSkel> skel, RR_SHARED_PTR<RobotRaconteurException> err, RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep);

		static void EndAsyncCallFunction(RR_WEAK_PTR<ServiceSkel> skel, RR_INTRUSIVE_PTR<MessageElement> ret, RR_SHARED_PTR<RobotRaconteurException> err, RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep);

	public:

		virtual RR_SHARED_PTR<RRObject> GetSubObj(const std::string &name, const std::string &ind) = 0;

		RR_SHARED_PTR<RRObject> GetSubObj(const std::string &name);

		virtual void RegisterEvents(RR_SHARED_PTR<RRObject> obj1);

		virtual void UnregisterEvents(RR_SHARED_PTR<RRObject> obj1);

		virtual void InitPipeServers(RR_SHARED_PTR<RRObject> obj1);

		virtual void InitWireServers(RR_SHARED_PTR<RRObject> obj1);

		void ObjRefChanged(const std::string &name);

		void SendEvent(RR_INTRUSIVE_PTR<MessageEntry> m);

		void ReleaseObject();

		virtual void ReleaseCastObject() = 0;

		void AsyncSendPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e, bool unreliable, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) >) handler);

		void SendWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

		virtual void DispatchPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

		virtual void DispatchWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

		
		virtual RR_INTRUSIVE_PTR<MessageEntry> CallPipeFunction(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

		virtual RR_INTRUSIVE_PTR<MessageEntry> CallWireFunction(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

		virtual RR_SHARED_PTR<void> GetCallbackFunction(uint32_t endpoint, const std::string& membername);

		virtual RR_INTRUSIVE_PTR<MessageEntry> CallMemoryFunction(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e);

		RR_WEAK_PTR<ServerContext_ObjectLock> objectlock;
		boost::mutex objectlock_lock;

		RR_SHARED_PTR<ServerContext_MonitorObjectSkel> monitorlock;

		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerContext_MonitorObjectSkel> > monitorlocks;
		boost::mutex monitorlocks_lock;

		bool IsLocked() ;

		bool IsMonitorLocked() ;

		virtual std::string GetObjectType() = 0;

		virtual std::string GetObjectType(RobotRaconteurVersion client_version);

		RR_SHARED_PTR<RobotRaconteurNode> RRGetNode();
		RR_WEAK_PTR<RobotRaconteurNode> RRGetNodeWeak();

		virtual void CallGeneratorNext(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e);

		void SendGeneratorResponse(int32_t index, RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep);

	protected:
		RR_WEAK_PTR<RobotRaconteurNode> node;

	private:
		void InitializeInstanceFields();
	};

	
	class ROBOTRACONTEUR_CORE_API  UserAuthenticator;
	
	class ROBOTRACONTEUR_CORE_API  ServerContext : public RR_ENABLE_SHARED_FROM_THIS<ServerContext>, boost::noncopyable
	{		
	public:

		std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > GetAttributes();
		void SetAttributes(const std::map<std::string, RR_INTRUSIVE_PTR<RRValue> >& attr);

	private:
		std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > m_Attributes;
		boost::mutex m_Attributes_lock;
	public:
		RR_SHARED_PTR<ServiceFactory> GetServiceDef() const;

		RR_SHARED_PTR<ServiceFactory> GetRootObjectServiceDef(RobotRaconteurVersion client_version);

		virtual ~ServerContext() {}

	protected:
		RR_SHARED_PTR<ServiceFactory> m_ServiceDef;

	public:
		std::string GetServiceName() const;

	protected:
		std::string m_ServiceName;


		RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceSkel> > skels;
		boost::mutex skels_lock;

		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> > client_endpoints;
		boost::mutex client_endpoints_lock;

	public:
		std::string GetRootObjectType(RobotRaconteurVersion client_version);

	protected:
		std::string m_RootObjectType;


		RR_WEAK_PTR<RobotRaconteurNode> node;


		/*public Message SendRequest(Message m)
		{
		    return null;
		}*/

	public:
		ServerContext(RR_SHARED_PTR<ServiceFactory> f, RR_SHARED_PTR<RobotRaconteurNode> node);

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		virtual void SendEvent(RR_INTRUSIVE_PTR<MessageEntry> m);
#undef SendMessage
		virtual void SendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e);

		virtual void SendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

		virtual void AsyncSendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )>& callback);

		virtual void AsyncSendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )>& callback);

		virtual void AsyncSendUnreliableMessage(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> e, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

		virtual void AsyncSendPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e, bool unreliable, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

		virtual void SendWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);


	private:
		//boost::mutex rec_sync;



		bool base_object_set;

	protected:
		void SetSecurityPolicy(RR_SHARED_PTR<ServiceSecurityPolicy> policy);


	public:
		virtual void SetBaseObject(const std::string &name, RR_SHARED_PTR<RRObject> o, RR_SHARED_PTR<ServiceSecurityPolicy> policy = RR_SHARED_PTR<ServiceSecurityPolicy>());

		virtual RR_SHARED_PTR<ServiceSkel> GetObjectSkel(const std::string &servicepath);


		virtual void ReplaceObject(const std::string &path);

		virtual std::string GetObjectType(const std::string &servicepath, RobotRaconteurVersion client_version);

		static RR_SHARED_PTR<ServerContext> GetCurrentServerContext();
	private:

		static boost::thread_specific_ptr<RR_SHARED_PTR<ServerContext> > m_CurrentServerContext;

	public:
		static std::string GetCurrentServicePath() ;
	private:

		static boost::thread_specific_ptr<std::string> m_CurrentServicePath;


	public:
		virtual RR_INTRUSIVE_PTR<MessageEntry> ProcessMessageEntry(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> c);

		virtual void Close();

		virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m, RR_SHARED_PTR<ServerEndpoint> e);

		virtual void AddClient(RR_SHARED_PTR<ServerEndpoint> cendpoint);

		virtual void RemoveClient(RR_SHARED_PTR<ServerEndpoint> cendpoint);


		virtual void KickUser(const std::string& username);
		


	private:
		RR_SHARED_PTR<UserAuthenticator> user_authenticator;
		std::map<std::string, std::string> security_policies;

		bool m_RequireValidUser;
		bool AllowObjectLock;

		virtual RR_INTRUSIVE_PTR<MessageEntry> ClientSessionOp(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> e);

	public:

		virtual bool RequireValidUser();

		virtual RR_SHARED_PTR<AuthenticatedUser> AuthenticateUser(const std::string &username, std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > &credentials);



	private:
		boost::mutex ClientLockOp_lockobj;

		virtual void ClientLockOp(RR_INTRUSIVE_PTR<MessageEntry> m, RR_INTRUSIVE_PTR<MessageEntry> ret);

	protected:
		void check_lock(RR_SHARED_PTR<ServiceSkel> skel);

		void check_monitor_lock(RR_SHARED_PTR<ServiceSkel> skel);

		RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServerContext_ObjectLock> > active_object_locks;

	public:
		virtual void PeriodicCleanupTask();

	protected:
		RR_INTRUSIVE_PTR<MessageEntry> CheckServiceCapability(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> c);

	public:
		boost::signals2::signal<void (RR_SHARED_PTR<ServerContext>, ServerServiceListenerEventType, RR_SHARED_PTR<void>)> ServerServiceListener;

	protected:
		void ReleaseServicePath1(const std::string &path);
	public:
		void ReleaseServicePath(const std::string &path);

		void ReleaseServicePath(const std::string& path, const std::vector<uint32_t>& endpoints);

		void AsyncProcessCallbackRequest(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t endpoint, RR_MOVE_ARG(boost::function<void ( RR_INTRUSIVE_PTR<MessageEntry>, RR_SHARED_PTR<RobotRaconteurException>  )>) handler, int32_t timeout=RR_TIMEOUT_INFINITE);

	private:
		
		void AsyncProcessCallbackRequest_err(RR_SHARED_PTR<RobotRaconteurException> error, uint32_t requestid);

		void AsyncProcessCallbackRequest_timeout(const TimerEvent&, uint32_t requestid);

		uint32_t request_number;

		struct outstanding_request
		{
			RR_INTRUSIVE_PTR<MessageEntry> ret;
			RR_SHARED_PTR<AutoResetEvent> evt;
			boost::function<void ( RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> error)> handler;
			RR_SHARED_PTR<Timer> timer;
		};
		
		boost::mutex outstanding_requests_lock;
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_request> > outstanding_requests;


	public:
		RR_INTRUSIVE_PTR<MessageEntry> ProcessCallbackRequest(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t endpointid);
	
		int32_t GetMonitorThreadPoolCount();

		void SetMonitorThreadPoolCount(int32_t count);

		RR_SHARED_PTR<ThreadPool> GetMonitorThreadPool();

	protected:

		RR_SHARED_PTR<ThreadPool> monitor_thread_pool;
		boost::mutex monitor_thread_pool_lock;

	public:

		bool UseMessage3(uint32_t ep);

	private:
		void InitializeInstanceFields();
	};

	


	class ROBOTRACONTEUR_CORE_API  ServerEndpoint : public Endpoint, public RR_ENABLE_SHARED_FROM_THIS<ServerEndpoint>
	{
	public:
		RR_SHARED_PTR<ServerContext> service;

	private:

		static boost::thread_specific_ptr<RR_SHARED_PTR<ServerEndpoint> > m_CurrentEndpoint;

	public:
		static RR_SHARED_PTR<ServerEndpoint> GetCurrentEndpoint() ;

	private:

		static boost::thread_specific_ptr<RR_SHARED_PTR<AuthenticatedUser> > m_CurrentAuthenticatedUser;

	public:
		static RR_SHARED_PTR<AuthenticatedUser> GetCurrentAuthenticatedUser();

	private:
		RR_SHARED_PTR<AuthenticatedUser> endpoint_authenticated_user;

	public:
		const std::string GetAuthenticatedUsername() const;


		virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m);

		void AuthenticateUser(const std::string &username, std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > &credentials);

		void LogoutUser();


		virtual void PeriodicCleanupTask();

	private:

		boost::atomic<bool> use_message3;

	public:

		bool UseMessage3();
		virtual void SetTransportConnection(RR_SHARED_PTR<ITransportConnection> c);


	private:
		void InitializeInstanceFields();

public:
		ServerEndpoint(RR_SHARED_PTR<RobotRaconteurNode> node) : Endpoint(node)
		{
			InitializeInstanceFields();
		}
	};
	
	class ROBOTRACONTEUR_CORE_API  IRobotRaconteurMonitorObject
	{
	 public:
		 virtual void RobotRaconteurMonitorEnter() = 0;

		 virtual void RobotRaconteurMonitorEnter(int32_t timeout) = 0;

		 virtual void RobotRaconteurMonitorExit() = 0;

		 virtual ~IRobotRaconteurMonitorObject() {}

	};


}

#pragma warning(pop)