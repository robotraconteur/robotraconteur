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

%shared_ptr(RobotRaconteur::WrappedRRObject);
%shared_ptr(RobotRaconteur::WrappedServiceSkel);
%shared_ptr(RobotRaconteur::ServerContext);
%shared_ptr(RobotRaconteur::ServerEndpoint);
%shared_ptr(RobotRaconteur::AuthenticatedUser);

%feature("director") RobotRaconteur::WrappedServiceSkelDirector;
%feature("director") RobotRaconteur::WrappedUserAuthenticatorDirector;
%feature("director") ServerServiceListenerDirector;

//ServiceSkel
namespace RobotRaconteur
{

RR_DIRECTOR_SHARED_PTR_RETURN_RESET(RobotRaconteur::MessageElement)
class WrappedServiceSkelDirector
{
public:
	virtual ~WrappedServiceSkelDirector() {}
	virtual void Init(boost::shared_ptr<RobotRaconteur::WrappedServiceSkel> skel);
	%rename (_CallGetProperty) CallGetProperty;
	virtual boost::intrusive_ptr<RobotRaconteur::MessageElement> CallGetProperty(const std::string& name);
	%rename (_CallSetProperty) CallSetProperty;
	virtual void CallSetProperty(const std::string& name, boost::intrusive_ptr<RobotRaconteur::MessageElement> m);
	%rename (_CallFunction) CallFunction;
	virtual boost::intrusive_ptr<RobotRaconteur::MessageElement> CallFunction(const std::string& name, const std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> >& m);
	%rename (_GetSubObj) GetSubObj;
	virtual boost::shared_ptr<RobotRaconteur::WrappedRRObject> GetSubObj(const std::string& name, const std::string& index);
	%rename (_GetArrayMemory) GetArrayMemory;
	virtual WrappedArrayMemoryDirector* GetArrayMemory(const std::string& name);
	%rename (_GetMultiDimArrayMemory) GetMultiDimArrayMemory;
	virtual WrappedMultiDimArrayMemoryDirector* GetMultiDimArrayMemory(const std::string& name);
	%rename (_GetPodArrayMemory) GetPodArrayMemory;
	virtual WrappedPodArrayMemoryDirector* GetPodArrayMemory(const std::string& name);
	%rename (_GetPodMultiDimArrayMemory) GetPodMultiDimArrayMemory;
	virtual WrappedPodMultiDimArrayMemoryDirector* GetPodMultiDimArrayMemory(const std::string& name);
	%rename (_GetNamedArrayMemory) GetNamedArrayMemory;
	virtual WrappedNamedArrayMemoryDirector* GetNamedArrayMemory(const std::string& name);
	%rename (_GetNamedMultiDimArrayMemory) GetNamedMultiDimArrayMemory;
	virtual WrappedNamedMultiDimArrayMemoryDirector* GetNamedMultiDimArrayMemory(const std::string& name);
	
	virtual void MonitorEnter(int32_t timeout) {};
	virtual void MonitorExit() {};
	virtual void ReleaseCastObject();	
};
RR_DIRECTOR_SHARED_PTR_RETURN_DEFAULT(RobotRaconteur::MessageElement)

class WrappedRRObject : public RRObject
{
	
public:
	WrappedRRObject(const std::string& type, WrappedServiceSkelDirector* RR_Director, int32_t id);
	virtual std::string RRType();
	std::string Type;
	
	virtual ~WrappedRRObject();
};

class WrappedServiceSkel
{
public:
	boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> Type;

	boost::shared_ptr<WrappedRRObject> castobj;
			
	virtual boost::shared_ptr<RobotRaconteur::WrappedPipeServer> GetPipe(const std::string& membername);
	virtual boost::shared_ptr<RobotRaconteur::WrappedWireServer> GetWire(const std::string& membername);
	
	virtual void WrappedDispatchEvent(const std::string& name, const std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> >& m);

RR_RELEASE_GIL()
	virtual boost::intrusive_ptr<RobotRaconteur::MessageElement> WrappedCallbackCall(const std::string& name, uint32_t endpoint, const std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> >& m);
RR_KEEP_GIL()

	boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> RRGetNode();
	virtual int32_t RegisterGeneratorServer(const std::string& function_name, WrappedGeneratorServerDirector* gen);
};
}

//ServerContext
%ignore ServerServiceListenerDirector::OuterCallback;

%inline
{
class ServerServiceListenerDirector
{
public:
	
	int32_t objectheapid;
	
	ServerServiceListenerDirector()
	{
		objectheapid=0;
	}
	
	void OuterCallback(boost::shared_ptr<ServerContext> c,ServerServiceListenerEventType code,boost::shared_ptr<void> p)
	{
		if (code==ServerServiceListenerEventType_ClientConnected || code==ServerServiceListenerEventType_ClientDisconnected)
		{
			boost::shared_ptr<uint32_t> p2=boost::static_pointer_cast<uint32_t>(p);
			
			DIRECTOR_CALL2(Callback(code,*p2));
		}
		else
		{
			
			DIRECTOR_CALL2(Callback(code,0));
		}
	}
	virtual void Callback(int32_t code, uint32_t endpoint) {};
	
	
	
	virtual ~ServerServiceListenerDirector()
	{
		
	}
	
};
}

namespace RobotRaconteur
{
%nodefaultctor ServerContext;
class ServerContext
{
public:
	static std::string GetCurrentServicePath() ;
	%rename ReleaseServicePath _ReleaseServicePath;
	void ReleaseServicePath(const std::string& path);
	RR_MAKE_METHOD_PRIVATE(ReleaseServicePath)
	void ReleaseServicePath(const std::string& path, std::vector<uint32_t> endpoints);
	static boost::shared_ptr<RobotRaconteur::ServerContext> GetCurrentServerContext();
	virtual void KickUser(const std::string& username);
	
	RR_MAKE_METHOD_PRIVATE(AddServerServiceListener)
	RR_MAKE_METHOD_PRIVATE(SetServiceAttributes)
	
	%extend
	{
	void AddServerServiceListener(ServerServiceListenerDirector* listener)
	{
		boost::shared_ptr<ServerServiceListenerDirector> listener2=boost::shared_ptr<ServerServiceListenerDirector>(listener,boost::bind(&ReleaseDirector<ServerServiceListenerDirector>, _1, listener->objectheapid));
		$self->ServerServiceListener.connect(boost::bind(&ServerServiceListenerDirector::OuterCallback,listener2,_1,_2,_3));
		
	}
	
	void SetServiceAttributes(boost::intrusive_ptr<MessageElement> attributes)
	{
		boost::intrusive_ptr<RRMap<std::string,RRValue> > mmap=rr_cast<RRMap<std::string,RRValue> >(RobotRaconteurNode::s()->UnpackMapType<std::string,RRValue>(attributes->CastData<MessageElementMap<std::string> >()));
		$self->SetAttributes(mmap->GetStorageContainer());		
		
	}	
	}
	
	boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> GetNode();	
};

%nodefaultctor AuthenticatedUser;
class AuthenticatedUser
{
public:
    
    RR_MAKE_METHOD_PRIVATE(GetUsername)
    const std::string GetUsername();
    
    RR_MAKE_METHOD_PRIVATE(GetPrivileges)    
    const std::vector<std::string> GetPrivileges();
    
	RR_MAKE_METHOD_PRIVATE(GetProperties)    
    const std::vector<std::string> GetProperties();
	
    RR_MAKE_METHOD_PRIVATE(GetLoginTime)    
    const boost::posix_time::ptime GetLoginTime();
    
    RR_MAKE_METHOD_PRIVATE(GetLastAccessTime)    
    const boost::posix_time::ptime GetLastAccessTime();
    
};

%nodefaultctor ServerEndpoint;
class ServerEndpoint
{
public:
	%extend
	{
		static uint32_t GetCurrentEndpoint()
		{
			return ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint();
		}
	
	}
    static boost::shared_ptr<RobotRaconteur::AuthenticatedUser> GetCurrentAuthenticatedUser();
};

}
