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

%shared_ptr(RobotRaconteur::WrappedWireConnection)
%shared_ptr(RobotRaconteur::WrappedWireClient)
%shared_ptr(RobotRaconteur::WrappedWireServer)
%shared_ptr(RobotRaconteur::WrappedWireBroadcaster)
%shared_ptr(RobotRaconteur::WrappedWireUnicastReceiver)

%feature("director") RobotRaconteur::WrappedWireConnectionDirector;
%feature("director") RobotRaconteur::AsyncWireConnectionReturnDirector;
%feature("director") RobotRaconteur::AsyncWirePeekReturnDirector;
%feature("director") RobotRaconteur::WrappedWireServerConnectDirector;
%feature("director") RobotRaconteur::WrappedWireServerPeekValueDirector;
%feature("director") RobotRaconteur::WrappedWireServerPokeValueDirector;
%feature("director") RobotRaconteur::WrappedWireBroadcasterPredicateDirector;

namespace RobotRaconteur
{

class TimeSpec;

class WrappedWireConnectionDirector
{
public:
	virtual ~WrappedWireConnectionDirector() {}
	virtual void WireValueChanged(boost::intrusive_ptr<RobotRaconteur::MessageElement> value, const TimeSpec& time) = 0;
	virtual void WireConnectionClosedCallback() = 0;
};

class AsyncWireConnectionReturnDirector
{
public:
	virtual ~AsyncWireConnectionReturnDirector();
	virtual void handler(const boost::shared_ptr<RobotRaconteur::WrappedWireConnection>& ep, HandlerErrorInfo& error) = 0;
};

class AsyncWirePeekReturnDirector
{
public:
	virtual ~AsyncWirePeekReturnDirector() {}
	virtual void handler(boost::intrusive_ptr<RobotRaconteur::MessageElement> value, const TimeSpec& ts, HandlerErrorInfo& error) = 0;
};

class TryGetValueResult
{
public:
	bool res;
	boost::intrusive_ptr<RobotRaconteur::MessageElement> value;
	TimeSpec ts;
};

%nodefaultctor WrappedWireConnection;
class WrappedWireConnection
{
public:

	virtual boost::intrusive_ptr<MessageElement> GetInValue();
	virtual boost::intrusive_ptr<MessageElement> GetOutValue();
	virtual void SetOutValue(const boost::intrusive_ptr<MessageElement>& value);
	
	//WrappedWireConnectionDirector* RR_Director;
	
	//WrappedWireConnectionDirector* GetRRDirector();
	
	void SetRRDirector(WrappedWireConnectionDirector* director, int32_t id);
	
	boost::shared_ptr<TypeDefinition> Type;
RR_RELEASE_GIL()
	virtual void Close();
RR_KEEP_GIL()

	virtual uint32_t GetEndpoint();

	TimeSpec GetLastValueReceivedTime();
	
	TimeSpec GetLastValueSentTime();
	
	bool GetInValueValid();

	bool GetOutValueValid();
	
	TryGetValueResult TryGetInValue();
	TryGetValueResult TryGetOutValue();

	void AsyncClose(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id);
	
	boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> GetNode();

	bool GetIgnoreInValue();
	void SetIgnoreInValue(bool ignore);

	int32_t GetInValueLifespan();
	void SetInValueLifespan(int32_t millis);

	int32_t GetOutValueLifespan();
	void SetOutValueLifespan(int32_t millis);

RR_RELEASE_GIL()
	bool WaitInValueValid(int32_t timeout = RR_TIMEOUT_INFINITE);
	bool WaitOutValueValid(int32_t timeout = RR_TIMEOUT_INFINITE);
RR_KEEP_GIL()

    MemberDefinition_Direction Direction();

	
};

%nodefaultctor WrappedWireClient;
class WrappedWireClient
{
public:

RR_RELEASE_GIL()
	virtual boost::shared_ptr<WrappedWireConnection> Connect();
RR_KEEP_GIL()

	void AsyncConnect(int32_t timeout, AsyncWireConnectionReturnDirector* handler, int32_t id);
	virtual std::string GetMemberName();

RR_RELEASE_GIL()
    boost::intrusive_ptr<RobotRaconteur::MessageElement> PeekInValue(TimeSpec& ts);
	boost::intrusive_ptr<RobotRaconteur::MessageElement> PeekOutValue(TimeSpec& ts);
	void PokeOutValue(const boost::intrusive_ptr<RobotRaconteur::MessageElement>& value);
RR_KEEP_GIL()
		
	void AsyncPeekInValue(int32_t timeout, AsyncWirePeekReturnDirector* handler, int32_t id);
	void AsyncPeekOutValue(int32_t timeout, AsyncWirePeekReturnDirector* handler, int32_t id);
	void AsyncPokeOutValue(const boost::intrusive_ptr<RobotRaconteur::MessageElement>& value, int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id);
		
	
	boost::shared_ptr<RobotRaconteur::TypeDefinition> Type;
	
	boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> GetNode();

	MemberDefinition_Direction Direction();
};

class WrappedWireServerConnectDirector
{
public:
	virtual ~WrappedWireServerConnectDirector() {}
	virtual void WireConnectCallback(const boost::shared_ptr<WrappedWireConnection>& c) = 0;
};

class WrappedWireServerPeekValueDirector
{
public:	
    virtual ~WrappedWireServerPeekValueDirector() {}
	virtual boost::intrusive_ptr<RobotRaconteur::MessageElement> PeekValue(const uint32_t& ep) =0;
};

class WrappedWireServerPokeValueDirector
{
public:	
    virtual ~WrappedWireServerPokeValueDirector() {}
	virtual void PokeValue(boost::intrusive_ptr<RobotRaconteur::MessageElement> value, const RobotRaconteur::TimeSpec& ts, const uint32_t& ep) = 0;
};

%nodefaultctor WrappedWireServer;
class WrappedWireServer
{
public:
	
	virtual std::string GetMemberName();
	
	boost::shared_ptr<RobotRaconteur::TypeDefinition> Type;
	
	void SetWrappedWireConnectCallback(WrappedWireServerConnectDirector* director, int32_t id);
	
	boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> GetNode();
	
	void SetPeekInValueCallback(WrappedWireServerPeekValueDirector* director, int32_t id);
	void SetPeekOutValueCallback(WrappedWireServerPeekValueDirector* director, int32_t id);
	void SetPokeOutValueCallback(WrappedWireServerPokeValueDirector* director, int32_t id);
	
	MemberDefinition_Direction Direction();

};

class WrappedWireBroadcasterPredicateDirector
{
public:

	virtual bool Predicate(uint32_t client_endpoint) = 0;
	virtual ~WrappedWireBroadcasterPredicateDirector() {}
};

class WrappedWireBroadcaster
{
public:		

	void Init(const boost::shared_ptr<WrappedWireServer>& wire);

	void SetOutValue(const boost::intrusive_ptr<MessageElement>& value);
	
	size_t GetActiveWireConnectionCount();

	void SetPredicateDirector(WrappedWireBroadcasterPredicateDirector* f, int32_t id);

	int32_t GetOutValueLifespan();
	void SetOutValueLifespan(int32_t millis);

	void SetPeekInValueCallback(WrappedWireServerPeekValueDirector* director, int32_t id);
};

%apply uint32_t& OUTPUT {uint32_t& ep};

class WrappedWireUnicastReceiver
{
public:
	void Init(const boost::shared_ptr<WrappedWireServer>& wire);

	boost::intrusive_ptr<MessageElement> GetInValue(TimeSpec& ts, uint32_t& ep);	

	void AddInValueChangedListener(WrappedWireServerPokeValueDirector* director, int32_t id);

	int32_t GetInValueLifespan();
	void SetInValueLifespan(int32_t millis);

	bool TryGetInValue(WrappedService_typed_packet& val, TimeSpec& time);
};

}
