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

#include <algorithm>
#include <mex.h>
#include <RobotRaconteur.h>

#pragma once

// NOLINTBEGIN(cppcoreguidelines-pro-type-cstyle-cast,cppcoreguidelines-avoid-c-arrays)

using namespace RobotRaconteur;

enum RobotRaconteurMexObjectTypes
{
    RR_MEX_STUB = 1,
    RR_MEX_PIPE_ENDPOINT,
    RR_MEX_WIRE_CONNECTION,
    RR_MEX_MEMORY_CLIENT,
    RR_MEX_SERVICEINFO2_SUBSCRIPTION,
    RR_MEX_SERVICE_SUBSCRIPTION,
    RR_MEX_WIRE_SUBSCRIPTION,
    RR_MEX_PIPE_SUBSCRIPTION,
    RR_MEX_GENERATOR_CLIENT
};

DataTypes mxClassIDToRRDataType(mxClassID type);
mxClassID rrDataTypeToMxClassID(DataTypes type);

RR_INTRUSIVE_PTR<RRBaseArray> GetRRArrayFromMxArray(const mxArray* pa);
mxArray* GetMxArrayFromRRArray(const RR_INTRUSIVE_PTR<RRBaseArray>& array_,
                               std::vector<mwSize> dims = std::vector<mwSize>());

RR_INTRUSIVE_PTR<MessageElement> PackMxArrayToMessageElement(const mxArray* pm,
                                                             const boost::shared_ptr<TypeDefinition>& tdef,
                                                             const RR_SHARED_PTR<ServiceStub>& stub,
                                                             bool allow_null = true);
mxArray* UnpackMessageElementToMxArray(const RR_INTRUSIVE_PTR<MessageElement>& m,
                                       const boost::shared_ptr<TypeDefinition>& tdef,
                                       const RR_SHARED_PTR<ServiceStub>& stub);

mxArray* ConnectClient(mxArray* url, mxArray* username, mxArray* credentials);

class MexServiceFactory : public virtual RobotRaconteur::ServiceFactory
{
  public:
    MexServiceFactory(const std::string& defstring);
    MexServiceFactory(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& def);

    RR_OVIRTUAL std::string GetServiceName() RR_OVERRIDE;
    RR_OVIRTUAL std::string DefString() RR_OVERRIDE;
    RR_OVIRTUAL RR_SHARED_PTR<ServiceDefinition> ServiceDef() RR_OVERRIDE;
    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::StructureStub> FindStructureStub(boost::string_ref s) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackStructure(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure>& structin) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> UnpackStructure(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& mstructin) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackPodArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RRPodBaseArray> UnpackPodArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackPodMultiDimArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> UnpackPodMultiDimArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackNamedArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RRNamedBaseArray> UnpackNamedArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackNamedMultiDimArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> UnpackNamedMultiDimArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure) RR_OVERRIDE;

    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::ServiceStub> CreateStub(
        boost::string_ref objecttype, boost::string_ref path,
        const RR_SHARED_PTR<RobotRaconteur::ClientContext>& context) RR_OVERRIDE;
    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::ServiceSkel> CreateSkel(
        boost::string_ref objecttype, boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::RRObject>& obj,
        const RR_SHARED_PTR<RobotRaconteur::ServerContext>& context) RR_OVERRIDE;

    RR_OVIRTUAL void DownCastAndThrowException(RobotRaconteurException& exp) RR_OVERRIDE { throw exp; }

    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteurException> DownCastException(
        const RR_SHARED_PTR<RobotRaconteurException>& exp) RR_OVERRIDE
    {
        return exp;
    }

  private:
    RR_SHARED_PTR<ServiceDefinition> servicedef;
    std::string defstring;
};

class MexDynamicServiceFactory : public virtual DynamicServiceFactory
{
  public:
    RR_OVIRTUAL RR_SHARED_PTR<ServiceFactory> CreateServiceFactory(boost::string_ref def) RR_OVERRIDE;
    RR_OVIRTUAL std::vector<RR_SHARED_PTR<ServiceFactory> > CreateServiceFactories(const std::vector<std::string>& def)
        RR_OVERRIDE;
};

class MexEventConnection
{
  public:
    std::string membername;
    int32_t eventconnectionid;
    RR_SHARED_PTR<mxArray> functionhandle;

    MexEventConnection(const std::string& membername, int32_t eventconnectionid,
                       const RR_SHARED_PTR<mxArray>& functionhandle)
    {
        this->membername = membername;
        this->eventconnectionid = eventconnectionid;
        this->functionhandle = functionhandle;
    }
};

class MexCallbackCall
{
  public:
    AutoResetEvent ev;
    RR_INTRUSIVE_PTR<MessageEntry> request;
    RR_INTRUSIVE_PTR<MessageEntry> response;

    MexCallbackCall(const RR_INTRUSIVE_PTR<MessageEntry>& req) { this->request = req; }
};

class MexAsyncResult
{
  public:
    RR_SHARED_PTR<mxArray> handler;
    RR_SHARED_PTR<mxArray> param;
    RR_SHARED_PTR<RobotRaconteurException> error;

    RR_SHARED_PTR<TypeDefinition> return_type;
    RR_INTRUSIVE_PTR<MessageElement> return_value;

    MexAsyncResult(const RR_SHARED_PTR<mxArray>& handler, const RR_SHARED_PTR<mxArray>& param,
                   const RR_SHARED_PTR<RobotRaconteurException>& error,
                   const RR_SHARED_PTR<TypeDefinition>& return_type = RR_SHARED_PTR<TypeDefinition>(),
                   const RR_INTRUSIVE_PTR<MessageElement>& return_value = RR_INTRUSIVE_PTR<MessageElement>())
    {
        this->handler = handler;
        this->param = param;
        this->error = error;
        this->return_type = return_type;
        this->return_value = return_value;
    }
};

class MexPipeClient;
class MexWireClient;

class MexServiceStub : public virtual RobotRaconteur::ServiceStub
{
  public:
    MexServiceStub(boost::string_ref path, const RR_SHARED_PTR<ServiceEntryDefinition>& type,
                   const RR_SHARED_PTR<RobotRaconteur::ClientContext>& c);

    mxArray* PropertyGet(const std::string& PropertyName);
    void PropertySet(const std::string& PropertyName, const mxArray* value);
    mxArray* FunctionCall(const std::string& FunctionName, std::vector<const mxArray*> args);

    void AsyncPropertyGet(const std::string& PropertyName, const RR_SHARED_PTR<mxArray>& handler,
                          const RR_SHARED_PTR<mxArray>& param, uint32_t timeout);
    void AsyncPropertySet(const std::string& PropertyName, const mxArray* value, const RR_SHARED_PTR<mxArray>& handler,
                          const RR_SHARED_PTR<mxArray>& param, uint32_t timeout);
    void AsyncFunctionCall(const std::string& FunctionName, std::vector<const mxArray*> args,
                           const RR_SHARED_PTR<mxArray>& handler, const RR_SHARED_PTR<mxArray>& param,
                           uint32_t timeout);

    void EndAsyncPropertyGet(const RR_INTRUSIVE_PTR<MessageEntry>&,
                             const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&,
                             const RR_SHARED_PTR<mxArray>& handler, const RR_SHARED_PTR<mxArray>& param,
                             const RR_SHARED_PTR<PropertyDefinition>& def);
    void EndAsyncPropertySet(const RR_INTRUSIVE_PTR<MessageEntry>&,
                             const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&,
                             const RR_SHARED_PTR<mxArray>& handler, const RR_SHARED_PTR<mxArray>& param,
                             const RR_SHARED_PTR<PropertyDefinition>& def);
    void EndAsyncFunctionCall(const RR_INTRUSIVE_PTR<MessageEntry>&,
                              const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&,
                              const RR_SHARED_PTR<mxArray>& handler, const RR_SHARED_PTR<mxArray>& param,
                              const RR_SHARED_PTR<FunctionDefinition>& def);

    RR_OVIRTUAL void DispatchEvent(const RR_INTRUSIVE_PTR<MessageEntry>& m) RR_OVERRIDE;
    RR_OVIRTUAL void DispatchPipeMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m) RR_OVERRIDE;
    RR_OVIRTUAL void DispatchWireMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m) RR_OVERRIDE;
    /*virtual RR_INTRUSIVE_PTR<MessageEntry> CallbackCall(const RR_INTRUSIVE_PTR<MessageEntry>& m);
    virtual RR_SHARED_PTR<RobotRaconteur::MexPipeClient> GetPipe(const std::string& membername);
    virtual RR_SHARED_PTR<RobotRaconteur::MexWireClient> GetWire(const std::string& membername);
    virtual RR_SHARED_PTR<RobotRaconteur::ArrayMemoryBase> GetArrayMemory(const std::string& membername);
    virtual RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemoryBase> GetMultiDimArrayMemory(const std::string&
    membername);*/
    RR_OVIRTUAL void RRClose() RR_OVERRIDE;
    RR_OVIRTUAL void RRInitStub() RR_OVERRIDE;

    RR_OVIRTUAL RR_SHARED_PTR<PipeClientBase> RRGetPipeClient(boost::string_ref membername) RR_OVERRIDE;
    RR_OVIRTUAL RR_SHARED_PTR<WireClientBase> RRGetWireClient(boost::string_ref membername) RR_OVERRIDE;

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE;
    RR_SHARED_PTR<ServiceEntryDefinition> RR_objecttype;

    std::map<std::string, RR_SHARED_PTR<MexPipeClient> > pipes;
    std::map<std::string, RR_SHARED_PTR<MexWireClient> > wires;
    std::map<std::string, RR_SHARED_PTR<ArrayMemoryBase> > arraymemories;
    std::map<std::string, RR_SHARED_PTR<MultiDimArrayMemoryBase> > multidimarraymemories;

    int stubid;
    RR_SHARED_PTR<mxArray> stubptr;

    mxArray* subsref(const mxArray* S);
    void subsasgn(const mxArray* S, const mxArray* value);

    void MexProcessRequests();

    std::map<int32_t, boost::shared_ptr<MexEventConnection> > eventconnections;
    int32_t eventconnections_count;
    boost::recursive_mutex eventconnections_lock;

    bool EnableEvents;

    boost::recursive_mutex events_lock;
    std::deque<RR_INTRUSIVE_PTR<MessageEntry> > events;

    mxArray* addlistener(const mxArray* name, const mxArray* functionhandle);
    void deletelistener(const mxArray* eventid);

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageEntry> CallbackCall(const RR_INTRUSIVE_PTR<MessageEntry>& m) RR_OVERRIDE;

    std::deque<boost::shared_ptr<MexCallbackCall> > callback_requests;
    boost::recursive_mutex callback_lock;

    std::map<std::string, RR_SHARED_PTR<mxArray> > callbacks;

    mxArray* MemoryOp(const mxArray* member, const mxArray* command, const mxArray* S, const mxArray* data);
    void LockOp(const mxArray* command);

    boost::mutex async_results_lock;
    std::queue<RR_SHARED_PTR<MexAsyncResult> > async_results;
};

extern std::map<int32_t, boost::shared_ptr<MexServiceStub> > stubs;
extern int stubcount;
extern boost::recursive_mutex stubs_lock;

mxArray* ConnectClient(const mxArray* url, const mxArray* username, const mxArray* credentials);
mxArray* MatlabObjectFromMexStub(const boost::shared_ptr<MexServiceStub>& stub);

class MexPipeEndpoint : public PipeEndpointBase
{

  public:
    uint32_t SendPacket(const RR_INTRUSIVE_PTR<MessageElement>& packet);
    RR_INTRUSIVE_PTR<MessageElement> ReceivePacket();
    RR_INTRUSIVE_PTR<MessageElement> PeekNextPacket();
    RR_INTRUSIVE_PTR<MessageElement> ReceivePacketWait(int32_t timeout);
    RR_INTRUSIVE_PTR<MessageElement> PeekNextPacketWait(int32_t timeout);
    bool TryReceivePacketWait(RR_INTRUSIVE_PTR<MessageElement>& packet, int32_t timeout, bool peek);
    MexPipeEndpoint(const RR_SHARED_PTR<PipeBase>& parent, int32_t index, uint32_t endpoint,
                    const RR_SHARED_PTR<TypeDefinition>& Type, bool unreliable, MemberDefinition_Direction direction);
    RR_SHARED_PTR<TypeDefinition> Type;

    mxArray* subsref(const mxArray* S);
    void subsasgn(const mxArray* S, const mxArray* value);

    int32_t pipeendpointid;

    RR_OVIRTUAL void Close() RR_OVERRIDE;

    RR_SHARED_PTR<ServiceStub> GetStub();

  protected:
    RR_OVIRTUAL void fire_PipeEndpointClosedCallback() RR_OVERRIDE;
    RR_OVIRTUAL void fire_PacketReceivedEvent() RR_OVERRIDE;
    RR_OVIRTUAL void fire_PacketAckReceivedEvent(uint32_t packetnum) RR_OVERRIDE;
};

class MexPipeClient : public virtual PipeClientBase
{
  public:
    // virtual boost::function<void(const RR_SHARED_PTR<MexPipeEndpoint>&)> GetPipeConnectCallback();
    // virtual void SetPipeConnectCallback(boost::function<void(const RR_SHARED_PTR<MexPipeEndpoint>&)> function);
    RR_SHARED_PTR<MexPipeEndpoint> Connect(int32_t index);
    MexPipeClient(const std::string& name, const RR_SHARED_PTR<ServiceStub>& stub,
                  const RR_SHARED_PTR<TypeDefinition>& Type, bool unreliable, MemberDefinition_Direction direction);

    RR_SHARED_PTR<TypeDefinition> Type;

  protected:
    RR_OVIRTUAL RR_SHARED_PTR<PipeEndpointBase> CreateNewPipeEndpoint(int32_t index, bool unreliable,
                                                                      MemberDefinition_Direction direction) RR_OVERRIDE;
};

extern boost::recursive_mutex pipeendpoints_lock;
extern int32_t pipeendpoints_count;
extern std::map<int32_t, boost::shared_ptr<MexPipeEndpoint> > pipeendpoints;

class MexWireConnection : public virtual WireConnectionBase
{
  public:
    RR_INTRUSIVE_PTR<MessageElement> GetInValue();
    RR_INTRUSIVE_PTR<MessageElement> GetOutValue();
    void SetOutValue(const RR_INTRUSIVE_PTR<MessageElement>& value);
    MexWireConnection(const RR_SHARED_PTR<WireBase>& parent, uint32_t endpoint,
                      const RR_SHARED_PTR<TypeDefinition>& Type, MemberDefinition_Direction direction);

    RR_OVIRTUAL void fire_WireValueChanged(const RR_INTRUSIVE_PTR<RRValue>& value, TimeSpec time) RR_OVERRIDE;
    RR_OVIRTUAL void fire_WireClosedCallback() RR_OVERRIDE;

    RR_SHARED_PTR<TypeDefinition> Type;
    int32_t wireconnectionid;

    RR_SHARED_PTR<ServiceStub> GetStub();

    RR_OVIRTUAL void Close() RR_OVERRIDE;

    mxArray* subsref(const mxArray* S);
    void subsasgn(const mxArray* S, const mxArray* value);
};

class MexWireClient : public virtual WireClientBase
{
  public:
    RR_SHARED_PTR<MexWireConnection> Connect();
    MexWireClient(const std::string& name, const RR_SHARED_PTR<ServiceStub>& stub,
                  const RR_SHARED_PTR<TypeDefinition>& Type, MemberDefinition_Direction direction);

    RR_SHARED_PTR<TypeDefinition> Type;

    mxArray* PeekInValue();
    mxArray* PeekOutValue();
    void PokeOutValue(const mxArray* value);

  protected:
    RR_OVIRTUAL RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(MemberDefinition_Direction direction)
        RR_OVERRIDE;
};

extern boost::recursive_mutex wireconnections_lock;
extern int32_t wireconnections_count;
extern std::map<int32_t, boost::shared_ptr<MexWireConnection> > wireconnections;

class MexArrayMemoryClientUtil
{
  public:
    static mxArray* Read(const RR_SHARED_PTR<ArrayMemoryBase>& mem, uint64_t memorypos, uint64_t count);

    static void Write(const RR_SHARED_PTR<ArrayMemoryBase>& mem, uint64_t memorypos, const mxArray* buffer,
                      uint64_t bufferpos, uint64_t count);
};

class MexMultiDimArrayMemoryClientUtil
{
  public:
    static mxArray* Read(const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem, const std::vector<uint64_t>& memorypos,
                         const std::vector<uint64_t>& count);

    static void Write(const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem, const std::vector<uint64_t>& memorypos,
                      const mxArray* buffer, const std::vector<uint64_t>& bufferpos,
                      const std::vector<uint64_t>& count);
};

mxArray* FindService(const mxArray* name);

mxArray* FindNodeByID(const mxArray* name);

mxArray* FindNodeByName(const mxArray* name);

void UpdateDetectedNodes();
mxArray* GetDetectedNodes();

mxArray* ServiceDefinitionConstants(const boost::shared_ptr<ServiceDefinition>& def);

extern boost::recursive_mutex rate_lock;
extern int32_t rate_count;
extern std::map<int32_t, RR_SHARED_PTR<RobotRaconteur::Rate> > rates;

class MexRRObject : public RRObject
{
  public:
    boost::shared_ptr<mxArray> mxobj;
    boost::shared_ptr<ServiceEntryDefinition> type;
    std::string strtype;

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE { return strtype; }
};

class MexServiceSkel : public ServiceSkel
{

  public:
    boost::shared_ptr<MexRRObject> obj;

    boost::mutex request_lock;
    std::list<boost::tuple<RR_INTRUSIVE_PTR<MessageEntry>, boost::shared_ptr<ServerEndpoint> > > property_get_requests;
    std::list<boost::tuple<RR_INTRUSIVE_PTR<MessageEntry>, boost::shared_ptr<ServerEndpoint> > > property_set_requests;
    std::list<boost::tuple<RR_INTRUSIVE_PTR<MessageEntry>, boost::shared_ptr<ServerEndpoint> > > function_requests;

    RR_OVIRTUAL void Init(boost::string_ref s, const RR_SHARED_PTR<RRObject>& o,
                          const RR_SHARED_PTR<ServerContext>& c) RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageEntry> CallGetProperty(const RR_INTRUSIVE_PTR<MessageEntry>& m) RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageEntry> CallSetProperty(const RR_INTRUSIVE_PTR<MessageEntry>& m) RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageEntry> CallFunction(const RR_INTRUSIVE_PTR<MessageEntry>& m) RR_OVERRIDE;

    RR_OVIRTUAL void RegisterEvents(const RR_SHARED_PTR<RRObject>& obj1) RR_OVERRIDE;

    void DispatchMexEvent(const std::string& name, const mxArray* parameters);

    RR_OVIRTUAL void ReleaseCastObject() RR_OVERRIDE {}

    RR_OVIRTUAL std::string GetObjectType() RR_OVERRIDE
    {
        return GetContext()->GetServiceDef()->GetServiceName() + "." + obj->type->Name;
    }

    void ProcessRequests();

    RR_OVIRTUAL RR_SHARED_PTR<RRObject> GetSubObj(boost::string_ref, boost::string_ref) RR_OVERRIDE
    {
        throw InvalidArgumentException("Object not found");
    }

    int skelid;

    MexServiceSkel() { skelid = 0; }
};

class MexServiceInfo2Subscription : public RR_ENABLE_SHARED_FROM_THIS<MexServiceInfo2Subscription>
{
  public:
    MexServiceInfo2Subscription(const boost::shared_ptr<ServiceInfo2Subscription>& subscription);

    int servicesubscriptionid;

    mxArray* subsref(const mxArray* S);
    void subsasgn(const mxArray* S, const mxArray* value);

    mxArray* GetDetectedServiceInfo2();

    void Close();

  protected:
    boost::shared_ptr<ServiceInfo2Subscription> subscription;
};

class MexServiceSubscription : public RR_ENABLE_SHARED_FROM_THIS<MexServiceSubscription>
{
  public:
    MexServiceSubscription();
    void Init(const boost::shared_ptr<ServiceSubscription>& subscription);

    int servicesubscriptionid;

    mxArray* subsref(const mxArray* S);
    void subsasgn(const mxArray* S, const mxArray* value);

    mxArray* GetConnectedClients();

    void Close();

  protected:
    boost::shared_ptr<ServiceSubscription> subscription;
    std::vector<std::string> connect_failures;

    static void ClientConnectFailed(RR_WEAK_PTR<MexServiceSubscription> this_,
                                    const boost::shared_ptr<ServiceSubscription>& subscription,
                                    const ServiceSubscriptionClientID& id, const std::vector<std::string>& url,
                                    const RR_SHARED_PTR<RobotRaconteurException>& err);
    void ClientConnectFailed1(const boost::shared_ptr<ServiceSubscription>& subscription,
                              const ServiceSubscriptionClientID& id, const std::vector<std::string>& url,
                              const RR_SHARED_PTR<RobotRaconteurException>& err);
    boost::mutex this_lock;
};

class MexWireSubscription
{
  public:
    MexWireSubscription(const boost::shared_ptr<MexServiceSubscription>& service_subscription,
                        const boost::shared_ptr<WireSubscription<RR_INTRUSIVE_PTR<MessageElement> > >& subscription);

    int wiresubscriptionid;

    mxArray* subsref(const mxArray* S);
    void subsasgn(const mxArray* S, const mxArray* value);

    boost::weak_ptr<MexServiceSubscription> service_subscription;

  protected:
    boost::shared_ptr<WireSubscription<RR_INTRUSIVE_PTR<MessageElement> > > subscription;
};

class MexPipeSubscription
{
  public:
    MexPipeSubscription(const boost::shared_ptr<MexServiceSubscription>& service_subscription,
                        const boost::shared_ptr<PipeSubscription<RR_INTRUSIVE_PTR<MessageElement> > >& subscription);

    int pipesubscriptionid;

    mxArray* subsref(const mxArray* S);
    void subsasgn(const mxArray* S, const mxArray* value);

    boost::weak_ptr<MexServiceSubscription> service_subscription;

  protected:
    boost::shared_ptr<PipeSubscription<RR_INTRUSIVE_PTR<MessageElement> > > subscription;
};

class MexGeneratorClient : public virtual GeneratorClientBase, public RR_ENABLE_SHARED_FROM_THIS<MexGeneratorClient>
{
  public:
    MexGeneratorClient(const std::string& name, int32_t id, const RR_SHARED_PTR<ServiceStub>& stub,
                       const RR_SHARED_PTR<TypeDefinition>& return_type,
                       const RR_SHARED_PTR<TypeDefinition>& param_type);

    RR_SHARED_PTR<TypeDefinition> param_type;
    RR_SHARED_PTR<TypeDefinition> return_type;
    int32_t generatorid;

    mxArray* subsref(const mxArray* S);
    void subsasgn(const mxArray* S, const mxArray* value);

    static std::string GetFunctionName(const mxArray* S);

    void EndAsyncNext(const RR_INTRUSIVE_PTR<MessageElement>&,
                      const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&,
                      const RR_SHARED_PTR<mxArray>& handler, const RR_SHARED_PTR<mxArray>& param);
    void EndAsyncClose(const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&,
                       const RR_SHARED_PTR<mxArray>& handler, const RR_SHARED_PTR<mxArray>& param);
};

class MexPodArrayMemoryClient : public virtual ArrayMemoryClientBase, public virtual ArrayMemoryBase
{
  public:
    MexPodArrayMemoryClient(const std::string& membername, const RR_SHARED_PTR<ServiceStub>& stub, size_t element_size,
                            MemberDefinition_Direction direction, const RR_SHARED_PTR<TypeDefinition>& type);
    mxArray* Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count);
    void Write(uint64_t memorypos, const mxArray* buffer, uint64_t bufferpos, uint64_t count);
    RR_OVIRTUAL uint64_t Length() RR_OVERRIDE;
    RR_OVIRTUAL DataTypes ElementTypeID() RR_OVERRIDE;

  protected:
    RR_OVIRTUAL void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer, uint64_t bufferpos,
                                      uint64_t count) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer, uint64_t bufferpos,
                                                                      uint64_t count) RR_OVERRIDE;
    RR_OVIRTUAL size_t GetBufferLength(const void* buffer) RR_OVERRIDE;

    RR_SHARED_PTR<TypeDefinition> type;
};

class MexPodMultiDimArrayMemoryClient : public virtual MultiDimArrayMemoryClientBase,
                                        public virtual MultiDimArrayMemoryBase
{
  public:
    MexPodMultiDimArrayMemoryClient(const std::string& membername, const RR_SHARED_PTR<ServiceStub>& stub,
                                    size_t element_size, MemberDefinition_Direction direction,
                                    const RR_SHARED_PTR<TypeDefinition>& type);
    mxArray* Read(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos,
                  const std::vector<uint64_t>& count);
    void Write(const std::vector<uint64_t>& memorypos, const mxArray* buffer, const std::vector<uint64_t>& bufferpos,
               const std::vector<uint64_t>& count);
    RR_OVIRTUAL std::vector<uint64_t> Dimensions() RR_OVERRIDE;
    RR_OVIRTUAL uint64_t DimCount() RR_OVERRIDE;
    RR_OVIRTUAL DataTypes ElementTypeID() RR_OVERRIDE;

  protected:
    RR_OVIRTUAL void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer,
                                      const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count,
                                      uint64_t elemcount) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer,
                                                                      const std::vector<uint64_t>& bufferpos,
                                                                      const std::vector<uint64_t>& count,
                                                                      uint64_t elemcount) RR_OVERRIDE;

    RR_SHARED_PTR<TypeDefinition> type;
};

class MexNamedArrayMemoryClient : public virtual ArrayMemoryClientBase, public virtual ArrayMemoryBase
{
  public:
    MexNamedArrayMemoryClient(const std::string& membername, const RR_SHARED_PTR<ServiceStub>& stub,
                              boost::tuple<DataTypes, size_t> array_info, MemberDefinition_Direction direction,
                              const RR_SHARED_PTR<TypeDefinition>& type);
    mxArray* Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count);
    void Write(uint64_t memorypos, const mxArray* buffer, uint64_t bufferpos, uint64_t count);
    RR_OVIRTUAL uint64_t Length() RR_OVERRIDE;
    RR_OVIRTUAL DataTypes ElementTypeID() RR_OVERRIDE;

  protected:
    RR_OVIRTUAL void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer, uint64_t bufferpos,
                                      uint64_t count) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer, uint64_t bufferpos,
                                                                      uint64_t count) RR_OVERRIDE;
    RR_OVIRTUAL size_t GetBufferLength(const void* buffer) RR_OVERRIDE;

    RR_SHARED_PTR<TypeDefinition> type;

    DataTypes array_elementtype;
    size_t array_elementcount;
    std::string type_string;
};

class MexNamedMultiDimArrayMemoryClient : public virtual MultiDimArrayMemoryClientBase,
                                          public virtual MultiDimArrayMemoryBase
{
  public:
    MexNamedMultiDimArrayMemoryClient(const std::string& membername, const RR_SHARED_PTR<ServiceStub>& stub,
                                      boost::tuple<DataTypes, size_t> array_info, MemberDefinition_Direction direction,
                                      const RR_SHARED_PTR<TypeDefinition>& type);
    mxArray* Read(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos,
                  const std::vector<uint64_t>& count);
    void Write(const std::vector<uint64_t>& memorypos, const mxArray* buffer, const std::vector<uint64_t>& bufferpos,
               const std::vector<uint64_t>& count);
    RR_OVIRTUAL std::vector<uint64_t> Dimensions() RR_OVERRIDE;
    RR_OVIRTUAL uint64_t DimCount() RR_OVERRIDE;
    RR_OVIRTUAL DataTypes ElementTypeID() RR_OVERRIDE;

  protected:
    RR_OVIRTUAL void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer,
                                      const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count,
                                      uint64_t elemcount) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer,
                                                                      const std::vector<uint64_t>& bufferpos,
                                                                      const std::vector<uint64_t>& count,
                                                                      uint64_t elemcount) RR_OVERRIDE;

    RR_SHARED_PTR<TypeDefinition> type;

    DataTypes array_elementtype;
    size_t array_elementcount;
    std::string type_string;
};

mxArray* CreateEmptyStructure(const std::string& type_str, const boost::shared_ptr<ServiceStub>& obj);

extern boost::recursive_mutex servicesubscriptions_lock;
extern int32_t serviceinfo2subscriptions_count;
extern std::map<int32_t, boost::shared_ptr<MexServiceInfo2Subscription> > serviceinfo2subscriptions;
extern int32_t servicesubscriptions_count;
extern std::map<int32_t, boost::shared_ptr<MexServiceSubscription> > servicesubscriptions;
extern int32_t wiresubscriptions_count;
extern std::map<int32_t, boost::shared_ptr<MexWireSubscription> > wiresubscriptions;
extern int32_t pipesubscriptions_count;
extern std::map<int32_t, boost::shared_ptr<MexPipeSubscription> > pipesubscriptions;

extern mxArray* SubscribeServiceInfo2(const mxArray* service_types, const mxArray* filter);
extern mxArray* SubscribeServiceByType(const mxArray* service_types, const mxArray* filter);
extern mxArray* SubscribeService(const mxArray* url, const mxArray* username, const mxArray* credentials);

extern std::map<int, boost::weak_ptr<MexServiceSkel> > skels;
extern boost::mutex skels_lock;
extern int skelscount;
extern AutoResetEvent skels_waiting;

extern boost::weak_ptr<TcpTransport> tcp_transport;
extern boost::weak_ptr<LocalTransport> local_transport;
extern boost::weak_ptr<HardwareTransport> usb_transport;

extern boost::mutex generators_lock;
extern int32_t generators_count;
extern std::map<int32_t, boost::shared_ptr<MexGeneratorClient> > generators;

// NOLINTEND(cppcoreguidelines-pro-type-cstyle-cast,cppcoreguidelines-avoid-c-arrays)