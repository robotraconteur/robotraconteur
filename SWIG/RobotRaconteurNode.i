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
	
	void OuterCallback(boost::shared_ptr<RobotRaconteur::ClientContext> c,ClientServiceListenerEventType code,boost::shared_ptr<void> p)
	{
		
		DIRECTOR_CALL2(Callback(code));
	}
	virtual void Callback(int32_t code) {};
		
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

	uint32_t RegisterTransport(boost::shared_ptr<Transport> transport);
	
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
			listenerptr=boost::shared_ptr<ClientServiceListenerDirector>(listener,boost::bind(&ReleaseDirector<ClientServiceListenerDirector>,_1,listener->objectheapid));
		}
	
		boost::intrusive_ptr<RRMap<std::string,RRValue> > credentials2;
		if (credentials) credentials2=rr_cast<RRMap<std::string,RRValue> >($self->UnpackMapType<std::string,RRValue>(rr_cast<MessageElementMap<std::string> >(credentials)));
		boost::shared_ptr<WrappedServiceStub> stub;
		if (listener==0)
		{
			stub=rr_cast<WrappedServiceStub>($self->ConnectService(url,username,credentials2,NULL,objecttype));
		}
		else
		{
			
			stub=rr_cast<WrappedServiceStub>($self->ConnectService(url,username,credentials2,boost::bind(&ClientServiceListenerDirector::OuterCallback,listenerptr,_1,_2,_3),objecttype));
		}
		
		return stub;

	}

	boost::shared_ptr<RobotRaconteur::WrappedServiceStub> ConnectService(const std::vector<std::string>& url, const std::string& username = "", boost::intrusive_ptr<MessageElementData> credentials=boost::intrusive_ptr<MessageElementData>(), ClientServiceListenerDirector* listener=0, const std::string& objecttype="")
	{
	
		boost::shared_ptr<ClientServiceListenerDirector> listenerptr;
		if (listener)
		{
			listenerptr=boost::shared_ptr<ClientServiceListenerDirector>(listener,boost::bind(&ReleaseDirector<ClientServiceListenerDirector>,_1,listener->objectheapid));
		}
	
		boost::intrusive_ptr<RRMap<std::string,RRValue> > credentials2;
		if (credentials) credentials2=rr_cast<RRMap<std::string,RRValue> >($self->UnpackMapType<std::string,RRValue>(rr_cast<MessageElementMap<std::string> >(credentials)));
		boost::shared_ptr<WrappedServiceStub> stub;
		if (listener==0)
		{
			stub=rr_cast<WrappedServiceStub>($self->ConnectService(url,username,credentials2,NULL,objecttype));
		}
		else
		{
			
			stub=rr_cast<WrappedServiceStub>($self->ConnectService(url,username,credentials2,boost::bind(&ClientServiceListenerDirector::OuterCallback,listenerptr,_1,_2,_3),objecttype));
		}
		return stub;

	}
}

RR_KEEP_GIL()

	RR_MAKE_METHOD_PRIVATE(AsyncConnectService);

%extend {
	void AsyncConnectService(const std::string& url, const std::string& username, boost::intrusive_ptr<MessageElementData> credentials, ClientServiceListenerDirector* listener, const std::string& objecttype, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id)
	{
		
		boost::shared_ptr<AsyncStubReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncStubReturnDirector>,_1,id));
	
		boost::intrusive_ptr<RRMap<std::string,RRValue> > credentials2;
		if (credentials) credentials2=rr_cast<RRMap<std::string,RRValue> >($self->UnpackMapType<std::string,RRValue>(rr_cast<MessageElementMap<std::string> >(credentials)));
		
		if (listener==0)
		{
			$self->AsyncConnectService(url,username,credentials2,NULL,objecttype,boost::bind(&AsyncStubReturn_handler,_1,_2,sphandler),timeout);
		}
		else
		{
			boost::shared_ptr<ClientServiceListenerDirector> listenerptr=boost::shared_ptr<ClientServiceListenerDirector>(listener);
			$self->AsyncConnectService(url,username,credentials2,boost::bind(&ClientServiceListenerDirector::OuterCallback,listenerptr,_1,_2,_3),objecttype,boost::bind(&AsyncStubReturn_handler,_1,_2,sphandler),timeout);
		}		
	}

	void AsyncConnectService(const std::vector<std::string>& url, const std::string& username, boost::intrusive_ptr<MessageElementData> credentials, ClientServiceListenerDirector* listener, const std::string& objecttype, int32_t timeout,  AsyncStubReturnDirector* handler, int32_t id)
	{
	
		boost::shared_ptr<AsyncStubReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncStubReturnDirector>,_1,id));
	
		boost::intrusive_ptr<RRMap<std::string,RRValue> > credentials2;
		if (credentials) credentials2=rr_cast<RRMap<std::string,RRValue> >($self->UnpackMapType<std::string,RRValue>(rr_cast<MessageElementMap<std::string> >(credentials)));
		
		if (listener==0)
		{
			$self->AsyncConnectService(url,username,credentials2,NULL,objecttype,boost::bind(&AsyncStubReturn_handler,_1,_2,sphandler),timeout);
		}
		else
		{
			boost::shared_ptr<ClientServiceListenerDirector> listenerptr=boost::shared_ptr<ClientServiceListenerDirector>(listener);
			$self->AsyncConnectService(url,username,credentials2,boost::bind(&ClientServiceListenerDirector::OuterCallback,listenerptr,_1,_2,_3),objecttype,boost::bind(&AsyncStubReturn_handler,_1,_2,sphandler),timeout);
		}
	}
}

RR_RELEASE_GIL()

	RR_MAKE_METHOD_PRIVATE(DisconnectService)

%extend {
	void DisconnectService(boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj)
	{
		self->DisconnectService(obj);
	}
}

RR_KEEP_GIL()

	RR_MAKE_METHOD_PRIVATE(AsyncDisconnectService)

%extend {
	void AsyncDisconnectService(boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj, AsyncVoidNoErrReturnDirector* handler, int32_t id)
	{
		boost::shared_ptr<AsyncVoidNoErrReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncVoidNoErrReturnDirector>,_1,id));
		self->AsyncDisconnectService(obj,boost::bind(&AsyncVoidNoErrReturn_handler,sphandler));
	}
}

	static std::string SelectRemoteNodeURL(const std::vector<std::string>& urls);

	RR_MAKE_METHOD_PRIVATE(RegisterServiceType)
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
		std::vector<std::string> names=RobotRaconteurNode::s()->GetRegisteredServiceTypes();
		for (std::vector<std::string>::iterator e=names.begin(); e!=names.end(); ++e)
		{
			if ((*e)!="RobotRaconteurServiceIndex")
			defs.push_back(RobotRaconteurNode::s()->GetServiceType(*e)->ServiceDef());
		}
		defs.push_back(def2);

		VerifyServiceDefinitions(defs);

		$self->RegisterServiceType(boost::make_shared<WrappedServiceFactory>(def2));
	}
#endif
	
	void RegisterServiceType(boost::shared_ptr<RobotRaconteur::ServiceDefinition> def)
	{
#ifdef SWIGPYTHON
		std::vector<boost::shared_ptr<ServiceDefinition> > defs;
		std::vector<std::string> names=RobotRaconteurNode::s()->GetRegisteredServiceTypes();
		for (std::vector<std::string>::iterator e=names.begin(); e!=names.end(); ++e)
		{
			defs.push_back(RobotRaconteurNode::s()->GetServiceType(*e)->ServiceDef());
		}
		defs.push_back(def);

		VerifyServiceDefinitions(defs);
#endif
		$self->RegisterServiceType(boost::make_shared<WrappedServiceFactory>(def));
	}


	boost::shared_ptr<RobotRaconteur::ServiceDefinition> GetServiceType(const std::string& servicename)
	{
		return rr_cast<WrappedServiceFactory>(RobotRaconteurNode::s()->GetServiceType(servicename))->ServiceDef();
	}

	boost::shared_ptr<RobotRaconteur::ServiceDefinition> GetPulledServiceType(boost::shared_ptr<RRObject> obj,const std::string& servicename)
	{
		return rr_cast<WrappedServiceFactory>(RobotRaconteurNode::s()->GetPulledServiceType(obj,servicename))->ServiceDef();
	}
}

	bool IsServiceTypeRegistered(const std::string& servicename);	

	RR_MAKE_METHOD_PRIVATE(GetRegisteredServiceTypes)
	std::vector<std::string> GetRegisteredServiceTypes();
	
	RR_MAKE_METHOD_PRIVATE(GetPulledServiceTypes)
	std::vector<std::string> GetPulledServiceTypes(boost::shared_ptr<RRObject> obj);
	
	RR_MAKE_METHOD_PRIVATE(sp)
	%rename sp _get_s;
	static boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> sp();

	RR_MAKE_METHOD_PRIVATE(GetServicePath)
	std::string GetServicePath(boost::shared_ptr<RRObject> obj);
	
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

	std::string RequestObjectLock(boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj, RobotRaconteurObjectLockFlags flags)
	{
		return $self->RequestObjectLock(obj,flags);
	}
	std::string ReleaseObjectLock(boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj)
	{
		return $self->ReleaseObjectLock(obj);
	}
	void MonitorEnter(boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj, int32_t timeout = -1)
	{
		$self->MonitorEnter(obj,timeout);
	}
	void MonitorExit(boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj)
	{
		$self->MonitorExit(obj);
	}
}
		
	RR_MAKE_METHOD_PRIVATE(NowUTC)
	boost::posix_time::ptime NowUTC();


	
RR_KEEP_GIL()

	RR_MAKE_METHOD_PRIVATE(AsyncRequestObjectLock)
	RR_MAKE_METHOD_PRIVATE(AsyncReleaseObjectLock)

%extend {

	void AsyncRequestObjectLock(boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj, RobotRaconteurObjectLockFlags flags, int32_t timeout, AsyncStringReturnDirector* handler, int32_t id)
	{
		boost::shared_ptr<AsyncStringReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncStringReturnDirector>,_1,id));
		return $self->AsyncRequestObjectLock(obj,flags,boost::bind(&AsyncStringReturn_handler,_1,_2,sphandler),timeout);
	}
	void AsyncReleaseObjectLock(boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj, int32_t timeout, AsyncStringReturnDirector* handler, int32_t id)
	{
		boost::shared_ptr<AsyncStringReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncStringReturnDirector>,_1,id));
		return $self->AsyncReleaseObjectLock(obj,boost::bind(&AsyncStringReturn_handler,_1,_2,sphandler),timeout);
	}
}
	
	RR_MAKE_METHOD_PRIVATE(GetServiceAttributes)
	
%extend
{
	boost::intrusive_ptr<MessageElement> GetServiceAttributes(boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj)
		{
			boost::intrusive_ptr<RRMap<std::string,RRValue> > map=AllocateEmptyRRMap<std::string,RRValue>();
			map->GetStorageContainer()=$self->GetServiceAttributes(obj);
			boost::intrusive_ptr<MessageElementMap<std::string> > mmap=$self->PackMapType<std::string,RRValue>(map);
			return CreateMessageElement("value",mmap);
		}

}

	RR_MAKE_METHOD_PRIVATE(RegisterService)

	boost::shared_ptr<RobotRaconteur::ServerContext> RegisterService(const std::string& name, const std::string& servicetype, boost::shared_ptr<RobotRaconteur::RRObject> obj, boost::shared_ptr<RobotRaconteur::ServiceSecurityPolicy> securitypolicy = boost::shared_ptr<RobotRaconteur::ServiceSecurityPolicy>());

	void CloseService(const std::string &sname);

	RR_MAKE_METHOD_PRIVATE(NodeID)
	RR_MAKE_METHOD_PRIVATE(SetNodeID)
	
	const RobotRaconteur::NodeID NodeID();
	void SetNodeID(const RobotRaconteur::NodeID id);
	
	RR_MAKE_METHOD_PRIVATE(NodeName)
	RR_MAKE_METHOD_PRIVATE(SetNodeName)
	
	const std::string NodeName();
	void SetNodeName(const std::string name);
	
	RR_PROPERTY(ThreadPoolCount)
	int32_t GetThreadPoolCount();
	void SetThreadPoolCount(int32_t count);
	
RR_RELEASE_GIL()

	RR_MAKE_METHOD_PRIVATE(FindObjectType)

	std::string FindObjectType(boost::shared_ptr<RobotRaconteur::RRObject> obj, const std::string &n);

	std::string FindObjectType(boost::shared_ptr<RobotRaconteur::RRObject> obj, const std::string &n, const std::string &i);

RR_KEEP_GIL()

	RR_MAKE_METHOD_PRIVATE(AsyncFindObjectType)

%extend {
	void AsyncFindObjectType(boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj, const std::string& n, int32_t timeout, AsyncStringReturnDirector* handler, int32_t id)
	{
		boost::shared_ptr<AsyncStringReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncStringReturnDirector>,_1,id));
		return $self->AsyncFindObjectType(obj,n,boost::bind(&AsyncStringReturn_handler,_1,_2,sphandler),timeout);
	}

	void AsyncFindObjectType(boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj, const std::string& n, const std::string &i, int32_t timeout, AsyncStringReturnDirector* handler, int32_t id)
	{
		boost::shared_ptr<AsyncStringReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncStringReturnDirector>,_1,id));
		return $self->AsyncFindObjectType(obj,n,i,boost::bind(&AsyncStringReturn_handler,_1,_2,sphandler),timeout);
	}

}

	RR_MAKE_METHOD_PRIVATE(SetExceptionHandler)
	RR_MAKE_METHOD_PRIVATE(ClearExceptionHandler)

%extend {
	void SetExceptionHandler(AsyncVoidReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncVoidReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncVoidReturnDirector>,_1,id));
		$self->SetExceptionHandler(boost::bind(&WrappedExceptionHandler,_1,sphandler));
	}
	
	void ClearExceptionHandler()
	{
		$self->SetExceptionHandler(NULL);
	}
}

RR_RELEASE_GIL()

void Sleep(const boost::posix_time::time_duration& duration);

RR_KEEP_GIL()

	RR_MAKE_METHOD_PRIVATE(CreateTimer)

%extend {	
	boost::shared_ptr<RobotRaconteur::Timer> CreateTimer(const boost::posix_time::time_duration& period, bool oneshot, AsyncTimerEventReturnDirector* handler, int32_t id)
	{		
		boost::shared_ptr<AsyncTimerEventReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncTimerEventReturnDirector>,_1,id));
		return $self->CreateTimer(period,boost::bind(&TimerHandlerFunc,_1,sphandler),oneshot);
	}
}

	boost::shared_ptr<RobotRaconteur::Rate> CreateRate(double frequency);

	boost::shared_ptr<RobotRaconteur::AutoResetEvent> CreateAutoResetEvent();


	RR_MAKE_METHOD_PRIVATE(PostToThreadPool)
	
%extend
{
	void PostToThreadPool(AsyncVoidNoErrReturnDirector* handler, int32_t id)
	{
		boost::shared_ptr<AsyncVoidNoErrReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncVoidNoErrReturnDirector>,_1,id));
		$self->GetThreadPool()->Post(boost::bind(&AsyncVoidNoErrReturn_handler,sphandler));
	}

}

	RR_MAKE_METHOD_PRIVATE(GetRobotRaconteurVersion)
	std::string GetRobotRaconteurVersion();

};
	
}