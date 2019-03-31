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

%shared_ptr(RobotRaconteur::WrappedPipeEndpoint)
%shared_ptr(RobotRaconteur::WrappedPipeClient)
%shared_ptr(RobotRaconteur::WrappedPipeServer)
%shared_ptr(RobotRaconteur::WrappedPipeBroadcaster)

%feature("director") RobotRaconteur::WrappedPipeEndpointDirector;
%feature("director") RobotRaconteur::AsyncPipeEndpointReturnDirector;
%feature("director") RobotRaconteur::WrappedPipeServerConnectDirector;
%feature("director") RobotRaconteur::WrappedPipeBroadcasterPredicateDirector;

namespace RobotRaconteur
{
class WrappedPipeEndpointDirector
{
public:
	virtual ~WrappedPipeEndpointDirector() {}
	virtual void PipeEndpointClosedCallback();
	virtual void PacketReceivedEvent();
	virtual void PacketAckReceivedEvent(uint32_t packetnum);

};

class AsyncPipeEndpointReturnDirector
{
public:
	virtual ~AsyncPipeEndpointReturnDirector();
	virtual void handler(boost::shared_ptr<RobotRaconteur::WrappedPipeEndpoint> ep, uint32_t error_code, const std::string& errorname, const std::string& errormessage);
};

%nodefaultctor WrappedPipeEndpoint;
class WrappedPipeEndpoint
{

public:
RR_RELEASE_GIL()
	virtual uint32_t SendPacket(boost::intrusive_ptr<RobotRaconteur::MessageElement> packet);
RR_KEEP_GIL()
	virtual boost::intrusive_ptr<RobotRaconteur::MessageElement> ReceivePacket();
	virtual boost::intrusive_ptr<RobotRaconteur::MessageElement> PeekNextPacket();
RR_RELEASE_GIL()

	boost::intrusive_ptr<MessageElement> ReceivePacketWait(int32_t timeout = RR_TIMEOUT_INFINITE);
	boost::intrusive_ptr<MessageElement> PeekNextPacketWait(int32_t timeout = RR_TIMEOUT_INFINITE);	
	bool TryReceivePacketWait(boost::intrusive_ptr<MessageElement>& packet, int32_t timeout = RR_TIMEOUT_INFINITE, bool peek = false);
	
	virtual void Close();
RR_KEEP_GIL()
	virtual int32_t GetIndex();
	virtual uint32_t GetEndpoint();
	size_t Available();
	bool GetRequestPacketAck();
	void SetRequestPacketAck(bool v);
	boost::shared_ptr<TypeDefinition> Type;
	//WrappedPipeEndpointDirector* RR_Director;
	//WrappedPipeEndpointDirector* GetRRDirector();
	void SetRRDirector(WrappedPipeEndpointDirector* director, int32_t id);
	
	bool IsUnreliable();
	MemberDefinition_Direction Direction();
	
	virtual void AsyncSendPacket(boost::intrusive_ptr<RobotRaconteur::MessageElement> packet, AsyncUInt32ReturnDirector* handler, int32_t id);
	void AsyncClose(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id);
	boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> GetNode();

	bool GetIgnoreReceived();
	void SetIgnoreReceived(bool ignore);
};

%nodefaultctor WrappedPipeClient;
class WrappedPipeClient 
{
public:	

RR_RELEASE_GIL()

	virtual boost::shared_ptr<RobotRaconteur::WrappedPipeEndpoint> Connect(int32_t index);
	
RR_KEEP_GIL()
	
	void AsyncConnect(int32_t index, int32_t timeout, AsyncPipeEndpointReturnDirector* handler, int32_t id);
	std::string GetMemberName();
	boost::shared_ptr<RobotRaconteur::TypeDefinition> Type;
	
	boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> GetNode();

	MemberDefinition_Direction Direction();
};

class WrappedPipeServerConnectDirector
{
public:
	virtual ~WrappedPipeServerConnectDirector() {}
	virtual void PipeConnectCallback(boost::shared_ptr<WrappedPipeEndpoint> pipeendpoint) {};
};

%nodefaultctor WrappedPipeServer;
class WrappedPipeServer 
{
public:	
		
	std::string GetMemberName();
	boost::shared_ptr<RobotRaconteur::TypeDefinition> Type;
	void SetWrappedPipeConnectCallback(WrappedPipeServerConnectDirector* director, int32_t id);
	boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> GetNode();

	MemberDefinition_Direction Direction();
};

class WrappedPipeBroadcasterPredicateDirector
{
public:
	virtual bool Predicate(uint32_t client_endpoint, int32_t index) = 0;		
	virtual ~WrappedPipeBroadcasterPredicateDirector() {}
};

class WrappedPipeBroadcaster
{
public:

	void Init(boost::shared_ptr<WrappedPipeServer> pipe, int32_t maximum_backlog = -1);

RR_RELEASE_GIL()
	void SendPacket(boost::intrusive_ptr<MessageElement> packet);
RR_KEEP_GIL()

	void AsyncSendPacket(boost::intrusive_ptr<MessageElement> packet, AsyncVoidNoErrReturnDirector* handler, int32_t id);	

	size_t GetActivePipeEndpointCount();

	void SetPredicateDirector(WrappedPipeBroadcasterPredicateDirector* f, int32_t id);

};

}



