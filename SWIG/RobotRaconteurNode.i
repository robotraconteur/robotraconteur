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

%shared_ptr(RobotRaconteur::RobotRaconteurNode);

%feature("director") ClientServiceListenerDirector;

%ignore ClientServiceListenerDirector::OuterCallback;
%inline
{
class ClientServiceListenerDirector
{
public:
	
	int32_t objectheapid;
	
	ClientServiceListenerDirector()
	{
		objectheapid=0;
	}
	
	void OuterCallback(const boost::shared_ptr<RobotRaconteur::ClientContext>& c,ClientServiceListenerEventType code,const boost::shared_ptr<void>& p)
	{
		if (code == ClientServiceListenerEventType_ServicePathReleased)
		{
			std::string p1 = *RR_STATIC_POINTER_CAST<std::string>(p);
			DIRECTOR_CALL2(Callback2(code,p1));
		}
		else
		{		
			DIRECTOR_CALL2(Callback(code));
		}
	}
	virtual void Callback(int32_t code) {};
	virtual void Callback2(int32_t code, const std::string& p) {};
		
	virtual ~ClientServiceListenerDirector()
	{
		
	}
	
};
}

namespace RobotRaconteur
{

enum RobotRaconteurObjectLockFlags
{
	RobotRaconteurObjectLockFlags_USER_LOCK = 0,
	RobotRaconteurObjectLockFlags_CLIENT_LOCK
};

//%nodefaultctor RobotRaconteurNode;
class RobotRaconteurNode
{
public:

	uint32_t RegisterTransport(const boost::shared_ptr<Transport>& transport);
	
	RR_PROPERTY(RequestTimeout)
	uint32_t GetRequestTimeout();
	void SetRequestTimeout(uint32_t timeout);
	
	RR_PROPERTY(TransportInactivityTimeout)
	uint32_t GetTransportInactivityTimeout();
	void SetTransportInactivityTimeout(uint32_t timeout);
	
	RR_PROPERTY(EndpointInactivityTimeout)
	uint32_t GetEndpointInactivityTimeout();
	void SetEndpointInactivityTimeout(uint32_t timeout);

	RR_PROPERTY(MemoryMaxTransferSize)
	uint32_t GetMemoryMaxTransferSize();
	void SetMemoryMaxTransferSize(uint32_t size);
	
	RR_PROPERTY(NodeDiscoveryMaxCacheCount)
	uint32_t GetNodeDiscoveryMaxCacheCount();
	void SetNodeDiscoveryMaxCacheCount(uint32_t count);

	void UnregisterServiceType(const std::string& type);
	
RR_RELEASE_GIL()
	
	RR_MAKE_METHOD_PRIVATE(ConnectService);
	
%extend {
	boost::shared_ptr<RobotRaconteur::WrappedServiceStub> ConnectService(const std::string& url, const std::string& username="", boost::intrusive_ptr<MessageElementData> credentials=boost::intrusive_ptr<MessageElementData>(), ClientServiceListenerDirector* listener=0, const std::string& objecttype="")
	{
		boost::shared_ptr<ClientServiceListenerDirector> listenerptr;
		if (listener)
		{
			listenerptr=boost::shared_ptr<ClientServiceListenerDirector>(listener,boost::bind(&ReleaseDirector<ClientServiceListenerDirector>,RR_BOOST_PLACEHOLDERS(_1),listener->objectheapid));
		}
	
		boost::intrusive_ptr<RRMap<std::string,RRValue> > credentials2;
		if (credentials) credentials2=rr_cast<RRMap<std::string,RRValue> >($self->UnpackMapType<std::string,RRValue>(rr_cast<MessageElementNestedElementList>(credentials)));
		boost::shared_ptr<WrappedServiceStub> stub;
		if (listener==0)
		{
			stub=rr_cast<WrappedServiceStub>($self->ConnectService(url,username,credentials2,RR_NULL_FN,objecttype));
		}
		else
		{
			
			stub=rr_cast<WrappedServiceStub>($self->ConnectService(url,username,credentials2,boost::bind(&ClientServiceListenerDirector::OuterCallback,listenerptr,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),RR_BOOST_PLACEHOLDERS(_3)),objecttype));
		}
		
		return stub;

	}

	boost::shared_ptr<RobotRaconteur::WrappedServiceStub> ConnectService(const std::vector<std::string>& url, const std::string& username = "", boost::intrusive_ptr<MessageElementData> credentials=boost::intrusive_ptr<MessageElementData>(), ClientServiceListenerDirector* listener=0, const std::string& objecttype="")
	{
	
		boost::shared_ptr<ClientServiceListenerDirector> listenerptr;
		if (listener)
		{
			listenerptr=boost::shared_ptr<ClientServiceListenerDirector>(listener,boost::bind(&ReleaseDirector<ClientServiceListenerDirector>,RR_BOOST_PLACEHOLDERS(_1),listener->objectheapid));
		}
	
		boost::intrusive_ptr<RRMap<std::string,RRValue> > credentials2;
		if (credentials) credentials2=rr_cast<RRMap<std::string,RRValue> >($self->UnpackMapType<std::string,RRValue>(rr_cast<MessageElementNestedElementList >(credentials)));
		boost::shared_ptr<WrappedServiceStub> stub;
		if (listener==0)
		{
			stub=rr_cast<WrappedServiceStub>($self->ConnectService(url,username,credentials2,RR_NULL_FN,objecttype));
		}
		else
		{
			
			stub=rr_cast<WrappedServiceStub>($self->ConnectService(url,username,credentials2,boost::bind(&ClientServiceListenerDirector::OuterCallback,listenerptr,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),RR_BOOST_PLACEHOLDERS(_3)),objecttype));
		}
		return stub;

	}
}

RR_KEEP_GIL()

	RR_MAKE_METHOD_PRIVATE(AsyncConnectService);

%extend {
	void AsyncConnectService(const std::string& url, const std::string& username, const boost::intrusive_ptr<MessageElementData>& credentials, ClientServiceListenerDirector* listener, const std::string& objecttype, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id)
	{
		
		boost::shared_ptr<AsyncStubReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncStubReturnDirector>,RR_BOOST_PLACEHOLDERS(_1),id));
	
		boost::intrusive_ptr<RRMap<std::string,RRValue> > credentials2;
		if (credentials) credentials2=rr_cast<RRMap<std::string,RRValue> >($self->UnpackMapType<std::string,RRValue>(rr_cast<MessageElementNestedElementList >(credentials)));
		
		if (listener==0)
		{
			$self->AsyncConnectService(url,username,credentials2,RR_NULL_FN,objecttype,boost::bind(&AsyncStubReturn_handler,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),sphandler),timeout);
		}
		else
		{
			boost::shared_ptr<ClientServiceListenerDirector> listenerptr=boost::shared_ptr<ClientServiceListenerDirector>(listener);
			$self->AsyncConnectService(url,username,credentials2,boost::bind(&ClientServiceListenerDirector::OuterCallback,listenerptr,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),RR_BOOST_PLACEHOLDERS(_3)),objecttype,boost::bind(&AsyncStubReturn_handler,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),sphandler),timeout);
		}		
	}

	void AsyncConnectService(const std::vector<std::string>& url, const std::string& username, const boost::intrusive_ptr<MessageElementData>& credentials, ClientServiceListenerDirector* listener, const std::string& objecttype, int32_t timeout,  AsyncStubReturnDirector* handler, int32_t id)
	{
	
		boost::shared_ptr<AsyncStubReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncStubReturnDirector>,RR_BOOST_PLACEHOLDERS(_1),id));
	
		boost::intrusive_ptr<RRMap<std::string,RRValue> > credentials2;
		if (credentials) credentials2=rr_cast<RRMap<std::string,RRValue> >($self->UnpackMapType<std::string,RRValue>(rr_cast<MessageElementNestedElementList >(credentials)));
		
		if (listener==0)
		{
			$self->AsyncConnectService(url,username,credentials2,RR_NULL_FN,objecttype,boost::bind(&AsyncStubReturn_handler,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),sphandler),timeout);
		}
		else
		{
			boost::shared_ptr<ClientServiceListenerDirector> listenerptr=boost::shared_ptr<ClientServiceListenerDirector>(listener);
			$self->AsyncConnectService(url,username,credentials2,boost::bind(&ClientServiceListenerDirector::OuterCallback,listenerptr,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),RR_BOOST_PLACEHOLDERS(_3)),objecttype,boost::bind(&AsyncStubReturn_handler,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),sphandler),timeout);
		}
	}
}

RR_RELEASE_GIL()

	RR_MAKE_METHOD_PRIVATE(DisconnectService)

%extend {
	void DisconnectService(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj)
	{
		self->DisconnectService(obj);
	}
}

RR_KEEP_GIL()

	RR_MAKE_METHOD_PRIVATE(AsyncDisconnectService)

%extend {
	void AsyncDisconnectService(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj, AsyncVoidNoErrReturnDirector* handler, int32_t id)
	{
		boost::shared_ptr<AsyncVoidNoErrReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncVoidNoErrReturnDirector>,RR_BOOST_PLACEHOLDERS(_1),id));
		self->AsyncDisconnectService(obj,boost::bind(&AsyncVoidNoErrReturn_handler,sphandler));
	}
}

	static std::string SelectRemoteNodeURL(const std::vector<std::string>& urls);

	RR_MAKE_METHOD_PRIVATE(RegisterServiceType)
	RR_MAKE_METHOD_PRIVATE(RegisterServiceTypes)
	RR_MAKE_METHOD_PRIVATE(GetServiceType)
	RR_MAKE_METHOD_PRIVATE(GetPulledServiceType)
	
%extend
{

#ifdef SWIGPYTHON
	void RegisterServiceType(const std::string& def)
	{
		boost::shared_ptr<RobotRaconteur::ServiceDefinition> def2=boost::make_shared<ServiceDefinition>();
		def2->FromString(def);
		std::vector<boost::shared_ptr<ServiceDefinition> > defs;
		std::vector<std::string> names=$self->GetRegisteredServiceTypes();
		for (std::vector<std::string>::iterator e=names.begin(); e!=names.end(); ++e)
		{
			if ((*e)!="RobotRaconteurServiceIndex")
			defs.push_back($self->GetServiceType(*e)->ServiceDef());
		}
		defs.push_back(def2);

		VerifyServiceDefinitions(defs);

		$self->RegisterServiceType(boost::make_shared<WrappedServiceFactory>(def2));
	}

	void RegisterServiceTypes(const std::vector<std::string>& defs_str)
	{
		
		std::vector<boost::shared_ptr<ServiceDefinition> > defs;
		std::vector<boost::shared_ptr<ServiceDefinition> > defs2;
		BOOST_FOREACH(const std::string& def_str, defs_str)
		{
			boost::shared_ptr<RobotRaconteur::ServiceDefinition> def2=boost::make_shared<ServiceDefinition>();
			def2->FromString(def_str);
			defs.push_back(def2);
			defs2.push_back(def2);
		}
		std::vector<std::string> names=$self->GetRegisteredServiceTypes();
		for (std::vector<std::string>::iterator e=names.begin(); e!=names.end(); ++e)
		{
			if ((*e)!="RobotRaconteurServiceIndex")
			defs2.push_back($self->GetServiceType(*e)->ServiceDef());
		}		

		VerifyServiceDefinitions(defs2);

		BOOST_FOREACH(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& def, defs)
		{
			$self->RegisterServiceType(boost::make_shared<WrappedServiceFactory>(def));
		}
	}

#endif
	
	void RegisterServiceType(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& def)
	{
#ifdef SWIGPYTHON
		std::vector<boost::shared_ptr<ServiceDefinition> > defs;
		std::vector<std::string> names=$self->GetRegisteredServiceTypes();
		for (std::vector<std::string>::iterator e=names.begin(); e!=names.end(); ++e)
		{
			defs.push_back($self->GetServiceType(*e)->ServiceDef());
		}
		defs.push_back(def);

		VerifyServiceDefinitions(defs);
#endif
		$self->RegisterServiceType(boost::make_shared<WrappedServiceFactory>(def));
	}

	void RegisterServiceTypes(const std::vector<boost::shared_ptr<RobotRaconteur::ServiceDefinition> >& defs)
	{
#ifdef SWIGPYTHON
		std::vector<boost::shared_ptr<ServiceDefinition> > defs2;
		std::vector<std::string> names=$self->GetRegisteredServiceTypes();
		for (std::vector<std::string>::iterator e=names.begin(); e!=names.end(); ++e)
		{
			defs2.push_back($self->GetServiceType(*e)->ServiceDef());
		}
		BOOST_FOREACH(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& def, defs)
		{
			defs2.push_back(def);
		}

		VerifyServiceDefinitions(defs2);
#endif
		BOOST_FOREACH(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& def, defs)
		{
			$self->RegisterServiceType(boost::make_shared<WrappedServiceFactory>(def));
		}
	}


	boost::shared_ptr<RobotRaconteur::ServiceDefinition> GetServiceType(const std::string& servicename)
	{
		return rr_cast<WrappedServiceFactory>($self->GetServiceType(servicename))->ServiceDef();
	}

	boost::shared_ptr<RobotRaconteur::ServiceDefinition> GetPulledServiceType(const boost::shared_ptr<RRObject>& obj,const std::string& servicename)
	{
		return rr_cast<WrappedServiceFactory>($self->GetPulledServiceType(obj,servicename))->ServiceDef();
	}
}

	bool IsServiceTypeRegistered(const std::string& servicename);	

	RR_MAKE_METHOD_PRIVATE(GetRegisteredServiceTypes)
	std::vector<std::string> GetRegisteredServiceTypes();
	
	RR_MAKE_METHOD_PRIVATE(GetPulledServiceTypes)
	std::vector<std::string> GetPulledServiceTypes(const boost::shared_ptr<RRObject>& obj);
	
	RR_MAKE_METHOD_PRIVATE(sp)
	%rename sp _get_s;
	static boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> sp();

RR_RELEASE_GIL()
	
	RR_MAKE_METHOD_PRIVATE(Shutdown)
	void Shutdown();

RR_KEEP_GIL()

RR_RELEASE_GIL()

	RR_MAKE_METHOD_PRIVATE(RequestObjectLock)
	RR_MAKE_METHOD_PRIVATE(ReleaseObjectLock)
	RR_MAKE_METHOD_PRIVATE(MonitorEnter)
	RR_MAKE_METHOD_PRIVATE(MonitorExit)

%extend {

	std::string RequestObjectLock(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj, RobotRaconteurObjectLockFlags flags)
	{
		return $self->RequestObjectLock(obj,flags);
	}
	std::string ReleaseObjectLock(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj)
	{
		return $self->ReleaseObjectLock(obj);
	}
	void MonitorEnter(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj, int32_t timeout = -1)
	{
		$self->MonitorEnter(obj,timeout);
	}
	void MonitorExit(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj)
	{
		$self->MonitorExit(obj);
	}
}
		
	RR_MAKE_METHOD_PRIVATE(NowUTC)
	boost::posix_time::ptime NowUTC();

	RR_MAKE_METHOD_PRIVATE(NowTimeSpec)
	TimeSpec NowTimeSpec();

	RR_MAKE_METHOD_PRIVATE(NowNodeTime)
	boost::posix_time::ptime NowNodeTime();

	RR_MAKE_METHOD_PRIVATE(NodeSyncTimeUTC)
	boost::posix_time::ptime NodeSyncTimeUTC();

	RR_MAKE_METHOD_PRIVATE(NodeSyncTimeSpec)
	TimeSpec NodeSyncTimeSpec();


	
RR_KEEP_GIL()

	RR_MAKE_METHOD_PRIVATE(AsyncRequestObjectLock)
	RR_MAKE_METHOD_PRIVATE(AsyncReleaseObjectLock)

%extend {

	void AsyncRequestObjectLock(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj, RobotRaconteurObjectLockFlags flags, int32_t timeout, AsyncStringReturnDirector* handler, int32_t id)
	{
		boost::shared_ptr<AsyncStringReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncStringReturnDirector>,RR_BOOST_PLACEHOLDERS(_1),id));
		return $self->AsyncRequestObjectLock(obj,flags,boost::bind(&AsyncStringReturn_handler,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),sphandler),timeout);
	}
	void AsyncReleaseObjectLock(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj, int32_t timeout, AsyncStringReturnDirector* handler, int32_t id)
	{
		boost::shared_ptr<AsyncStringReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncStringReturnDirector>,RR_BOOST_PLACEHOLDERS(_1),id));
		return $self->AsyncReleaseObjectLock(obj,boost::bind(&AsyncStringReturn_handler,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),sphandler),timeout);
	}
}
	
	RR_MAKE_METHOD_PRIVATE(GetServiceAttributes)
	RR_MAKE_METHOD_PRIVATE(GetServiceNodeID)
	RR_MAKE_METHOD_PRIVATE(GetServiceNodeName)
	RR_MAKE_METHOD_PRIVATE(GetServiceName)
	RR_MAKE_METHOD_PRIVATE(GetObjectServicePath)
	RR_MAKE_METHOD_PRIVATE(GetObjectType)
	
%extend
{
	boost::intrusive_ptr<MessageElement> GetServiceAttributes(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj)
		{
			boost::intrusive_ptr<RRMap<std::string,RRValue> > map=AllocateEmptyRRMap<std::string,RRValue>();
			map->GetStorageContainer()=$self->GetServiceAttributes(obj);
			boost::intrusive_ptr<MessageElementNestedElementList > mmap=$self->PackMapType<std::string,RRValue>(map);
			return CreateMessageElement("value",mmap);
		}

	NodeID GetServiceNodeID(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj)
	{		
		return $self->GetServiceNodeID(obj);		
	}

	std::string GetServiceNodeName(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj)
	{		
		return $self->GetServiceNodeName(obj);		
	}

	std::string GetServiceName(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj)
	{		
		return $self->GetServiceName(obj);		
	}

	std::string GetObjectServicePath(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj)
	{		
		return $self->GetObjectServicePath(obj);		
	}

	std::string GetObjectType(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj)
	{		
		return $self->GetObjectType(obj);		
	}

}

	RR_MAKE_METHOD_PRIVATE(RegisterService)

	boost::shared_ptr<RobotRaconteur::ServerContext> RegisterService(const std::string& name, const std::string& servicetype, const boost::shared_ptr<RobotRaconteur::RRObject>& obj, boost::shared_ptr<RobotRaconteur::ServiceSecurityPolicy> securitypolicy = boost::shared_ptr<RobotRaconteur::ServiceSecurityPolicy>());

	void CloseService(const std::string &sname);

	RR_MAKE_METHOD_PRIVATE(NodeID)
	RR_MAKE_METHOD_PRIVATE(SetNodeID)
	
	const RobotRaconteur::NodeID NodeID();
	void SetNodeID(const RobotRaconteur::NodeID id);
	
	RR_MAKE_METHOD_PRIVATE(NodeName)
	RR_MAKE_METHOD_PRIVATE(SetNodeName)
	
	const std::string NodeName();
	void SetNodeName(const std::string& name);
	
	RR_PROPERTY(ThreadPoolCount)
	int32_t GetThreadPoolCount();
	void SetThreadPoolCount(int32_t count);
	
RR_RELEASE_GIL()

	RR_MAKE_METHOD_PRIVATE(FindObjectType)

	std::string FindObjectType(const boost::shared_ptr<RobotRaconteur::RRObject>& obj, const std::string &n);

	std::string FindObjectType(const boost::shared_ptr<RobotRaconteur::RRObject>& obj, const std::string &n, const std::string &i);

RR_KEEP_GIL()

	RR_MAKE_METHOD_PRIVATE(AsyncFindObjectType)

%extend {
	void AsyncFindObjectType(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj, const std::string& n, int32_t timeout, AsyncStringReturnDirector* handler, int32_t id)
	{
		boost::shared_ptr<AsyncStringReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncStringReturnDirector>,RR_BOOST_PLACEHOLDERS(_1),id));
		return $self->AsyncFindObjectType(obj,n,boost::bind(&AsyncStringReturn_handler,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),sphandler),timeout);
	}

	void AsyncFindObjectType(const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj, const std::string& n, const std::string &i, int32_t timeout, AsyncStringReturnDirector* handler, int32_t id)
	{
		boost::shared_ptr<AsyncStringReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncStringReturnDirector>,RR_BOOST_PLACEHOLDERS(_1),id));
		return $self->AsyncFindObjectType(obj,n,i,boost::bind(&AsyncStringReturn_handler,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),sphandler),timeout);
	}

}

	RR_MAKE_METHOD_PRIVATE(SetExceptionHandler)
	RR_MAKE_METHOD_PRIVATE(ClearExceptionHandler)

%extend {
	void SetExceptionHandler(AsyncVoidReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncVoidReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncVoidReturnDirector>,RR_BOOST_PLACEHOLDERS(_1),id));
		$self->SetExceptionHandler(boost::bind(&WrappedExceptionHandler,RR_BOOST_PLACEHOLDERS(_1),sphandler));
	}
	
	void ClearExceptionHandler()
	{
		$self->SetExceptionHandler(RR_NULL_FN);
	}
}

RR_RELEASE_GIL()

void Sleep(const boost::posix_time::time_duration& duration);

RR_KEEP_GIL()

	RR_MAKE_METHOD_PRIVATE(CreateTimer)

%extend {	
	boost::shared_ptr<RobotRaconteur::Timer> CreateTimer(const boost::posix_time::time_duration& period, bool oneshot, AsyncTimerEventReturnDirector* handler, int32_t id)
	{		
		boost::shared_ptr<AsyncTimerEventReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncTimerEventReturnDirector>,RR_BOOST_PLACEHOLDERS(_1),id));
		return $self->CreateTimer(period,boost::bind(&TimerHandlerFunc,RR_BOOST_PLACEHOLDERS(_1),sphandler),oneshot);
	}
}

	boost::shared_ptr<RobotRaconteur::Rate> CreateRate(double frequency);

	boost::shared_ptr<RobotRaconteur::AutoResetEvent> CreateAutoResetEvent();


	RR_MAKE_METHOD_PRIVATE(PostToThreadPool)
	
%extend
{
	void PostToThreadPool(AsyncVoidNoErrReturnDirector* handler, int32_t id)
	{
		boost::shared_ptr<AsyncVoidNoErrReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncVoidNoErrReturnDirector>,RR_BOOST_PLACEHOLDERS(_1),id));
		$self->GetThreadPool()->Post(boost::bind(&AsyncVoidNoErrReturn_handler,sphandler));
	}

}

	RR_MAKE_METHOD_PRIVATE(GetRobotRaconteurVersion)
	std::string GetRobotRaconteurVersion();

	bool CompareLogLevel(RobotRaconteur_LogLevel log_level);
	void LogMessage(RobotRaconteur_LogLevel level, const std::string& message);
	void LogRecord(const RRLogRecord& record);
	RobotRaconteur_LogLevel GetLogLevel();
	void SetLogLevel(RobotRaconteur_LogLevel level);
	RobotRaconteur_LogLevel SetLogLevelFromString(const std::string& level);
	RobotRaconteur_LogLevel SetLogLevelFromEnvVariable(const std::string& env_variable_name = "ROBOTRACONTEUR_LOG_LEVEL");
	boost::shared_ptr<RobotRaconteur::LogRecordHandler> GetLogRecordHandler();
	void SetLogRecordHandler(const boost::shared_ptr<RobotRaconteur::LogRecordHandler>& handler);

	void CheckConnection(uint32_t client);


};
	
}