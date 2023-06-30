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

#include <RobotRaconteur.h>

#ifdef RR_PYTHON
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#endif
#pragma once

namespace RobotRaconteur
{
#ifdef RR_PYTHON

// Use -threads SWIG option instead of manually releasing and ensuring GIL
class RR_Release_GIL
{
  public:
    RR_Release_GIL() { _save = PyEval_SaveThread(); }

    ~RR_Release_GIL() { PyEval_RestoreThread(_save); }

  protected:
    PyThreadState* _save;
};

class RR_Ensure_GIL
{
  public:
    RR_Ensure_GIL() { gstate = PyGILState_Ensure(); }

    ~RR_Ensure_GIL() { PyGILState_Release(gstate); }

  private:
    PyGILState_STATE gstate;
};

#define DIRECTOR_CALL(dirtype, command)                                                                                \
    {                                                                                                                  \
        boost::shared_lock<boost::shared_mutex> lock(RR_Director_lock);                                                \
        /*RR_Ensure_GIL gil;*/                                                                                         \
        boost::shared_ptr<dirtype> RR_Director2(this->RR_Director);                                                    \
                                                                                                                       \
        lock.unlock();                                                                                                 \
        if (!RR_Director2)                                                                                             \
            throw InvalidOperationException("Director has been released");                                             \
        {                                                                                                              \
            command;                                                                                                   \
        }                                                                                                              \
    }

#define DIRECTOR_CALL2(command)                                                                                        \
    {                                                                                                                  \
        /*RR_Ensure_GIL gil;*/                                                                                         \
        {                                                                                                              \
            command;                                                                                                   \
        }                                                                                                              \
    }

#define DIRECTOR_CALL3(dirtype, command)                                                                               \
    {                                                                                                                  \
        /*RR_Ensure_GIL gil;*/                                                                                         \
        boost::shared_lock<boost::shared_mutex> lock(RR_Director_lock);                                                \
        boost::shared_ptr<dirtype> RR_Director2(this->RR_Director);                                                    \
                                                                                                                       \
        lock.unlock();                                                                                                 \
        if (RR_Director2)                                                                                              \
        {                                                                                                              \
            command;                                                                                                   \
        }                                                                                                              \
    }

#define DIRECTOR_DELETE(var)                                                                                           \
    {                                                                                                                  \
        if (var)                                                                                                       \
        {                                                                                                              \
            delete var;                                                                                                \
            var = NULL;                                                                                                \
        }                                                                                                              \
    }

extern bool PythonTracebackPrintExc;
void InitPythonTracebackPrintExc();
void SetPythonTracebackPrintExc(bool value);

#else
#define DIRECTOR_CALL(dirtype, command)                                                                                \
    {                                                                                                                  \
        boost::shared_lock<boost::shared_mutex> lock(RR_Director_lock);                                                \
        boost::shared_ptr<dirtype> RR_Director2(this->RR_Director);                                                    \
                                                                                                                       \
        lock.unlock();                                                                                                 \
        if (!RR_Director2)                                                                                             \
            throw InvalidOperationException("Director has been released");                                             \
        RRDirectorExceptionHelper::Reset();                                                                            \
        {                                                                                                              \
            command;                                                                                                   \
        }                                                                                                              \
        if (RobotRaconteur::RRDirectorExceptionHelper::IsErrorPending())                                               \
            RobotRaconteurExceptionUtil::ThrowMessageEntryException(                                                   \
                RobotRaconteur::RRDirectorExceptionHelper::GetError());                                                \
    }

#define DIRECTOR_CALL2(command)                                                                                        \
    RRDirectorExceptionHelper::Reset();                                                                                \
    {                                                                                                                  \
        command;                                                                                                       \
    }                                                                                                                  \
    if (RobotRaconteur::RRDirectorExceptionHelper::IsErrorPending())                                                   \
        RobotRaconteurExceptionUtil::ThrowMessageEntryException(RobotRaconteur::RRDirectorExceptionHelper::GetError());

#define DIRECTOR_CALL3(dirtype, command)                                                                               \
    {                                                                                                                  \
        boost::shared_lock<boost::shared_mutex> lock(RR_Director_lock);                                                \
        boost::shared_ptr<dirtype> RR_Director2(this->RR_Director);                                                    \
                                                                                                                       \
        lock.unlock();                                                                                                 \
        if (!RR_Director2)                                                                                             \
            return;                                                                                                    \
        RRDirectorExceptionHelper::Reset();                                                                            \
        {                                                                                                              \
            command;                                                                                                   \
        }                                                                                                              \
        if (RobotRaconteur::RRDirectorExceptionHelper::IsErrorPending())                                               \
            RobotRaconteurExceptionUtil::ThrowMessageEntryException(                                                   \
                RobotRaconteur::RRDirectorExceptionHelper::GetError());                                                \
    }

#define DIRECTOR_DELETE(var) RR_UNUSED(var)
#endif

class RRDirectorExceptionHelper
{
  protected:
    static boost::thread_specific_ptr<RR_INTRUSIVE_PTR<MessageEntry> > last_err;

  public:
    static void Reset();
    static void SetError(const RR_INTRUSIVE_PTR<MessageEntry>& err, const std::string& exception_str);
    static bool IsErrorPending();
    static RR_INTRUSIVE_PTR<MessageEntry> GetError();
};

class RRNativeObjectHeapSupport
{
  protected:
    static boost::mutex support_lock;
    static RRNativeObjectHeapSupport* support;

  public:
    static void Set_Support(RRNativeObjectHeapSupport* s)
    {
        boost::mutex::scoped_lock lock(support_lock);

        support = s;
    }

    virtual void DeleteObject_i(int32_t id) = 0;

    static void DeleteObject(int32_t id)
    {
        boost::mutex::scoped_lock lock(support_lock);
        if (support)
            support->DeleteObject_i(id);
    }

    virtual ~RRNativeObjectHeapSupport() {}
};

#ifdef RR_PYTHON
class RRNativeDirectorSupport
{
  protected:
    static bool running;
    static boost::shared_mutex running_lock;

  public:
    static void Start()
    {
        boost::unique_lock<boost::shared_mutex> lock(running_lock);
        running = true;
    }
    static void Stop()
    {
        boost::unique_lock<boost::shared_mutex> lock(running_lock);
        running = false;
    }
    static bool IsRunning()
    {
        boost::shared_lock<boost::shared_mutex> lock(running_lock);
        return running;
    }
};

#endif

template <typename T>
void ReleaseDirector(T* ptr, int32_t id)
{
#ifdef RR_PYTHON
    if (RRNativeDirectorSupport::IsRunning())
    {
        DIRECTOR_CALL2(DIRECTOR_DELETE(ptr));
    }
#else
    DIRECTOR_CALL2(DIRECTOR_DELETE(ptr));
#endif

    if (id != 0)
        RRNativeObjectHeapSupport::DeleteObject(id);
}

class WrappedServiceFactory : public virtual RobotRaconteur::ServiceFactory
{
  public:
    WrappedServiceFactory(const std::string& defstring);
    WrappedServiceFactory(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& def);
    RR_OVIRTUAL ~WrappedServiceFactory() RR_OVERRIDE {}

    RR_OVIRTUAL std::string GetServiceName() RR_OVERRIDE;
    RR_OVIRTUAL std::string DefString() RR_OVERRIDE;
    RR_OVIRTUAL RR_SHARED_PTR<ServiceDefinition> ServiceDef() RR_OVERRIDE;
    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::StructureStub> FindStructureStub(boost::string_ref s) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackStructure(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure>& structin) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> UnpackStructure(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& mstructin) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackPodArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray> UnpackPodArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackPodMultiDimArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray> UnpackPodMultiDimArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackNamedArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray> UnpackNamedArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackNamedMultiDimArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray> UnpackNamedMultiDimArray(
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

class WrappedDynamicServiceFactory : public virtual DynamicServiceFactory
{
  public:
    RR_OVIRTUAL ~WrappedDynamicServiceFactory() RR_OVERRIDE {}
    RR_OVIRTUAL RR_SHARED_PTR<ServiceFactory> CreateServiceFactory(boost::string_ref def) RR_OVERRIDE;
    RR_OVIRTUAL std::vector<RR_SHARED_PTR<ServiceFactory> > CreateServiceFactories(const std::vector<std::string>& def)
        RR_OVERRIDE;
};

/*class AsyncHandlerDirector
{
public:
    virtual ~AsyncHandlerDirector() {}
    virtual void handler(void* m, uint32_t error_code, const std::string& errorname, const std::string& errormessage)
{};

};*/

class HandlerErrorInfo
{
  public:
    uint32_t error_code;
    std::string errorname;
    std::string errormessage;
    std::string errorsubname;
    boost::intrusive_ptr<RobotRaconteur::MessageElement> param_;

    HandlerErrorInfo();
    HandlerErrorInfo(const RobotRaconteurException& exp);
    HandlerErrorInfo(const boost::shared_ptr<RobotRaconteurException>& exp);
    HandlerErrorInfo(const boost::intrusive_ptr<MessageEntry>& m);
    HandlerErrorInfo(uint32_t error_code, const std::string& errorname, const std::string& errormessage,
                     const std::string& errorsubname = "",
                     const boost::intrusive_ptr<RobotRaconteur::MessageElement>& param_ = NULL);

    void ToMessageEntry(const RR_INTRUSIVE_PTR<MessageEntry>& m) const;
    RR_SHARED_PTR<RobotRaconteurException> ToException() const;
};

class AsyncRequestDirector
{
  public:
    virtual ~AsyncRequestDirector() {}
    virtual void handler(RR_INTRUSIVE_PTR<MessageElement> ret, HandlerErrorInfo& error) = 0;
};

class AsyncVoidReturnDirector
{
  public:
    virtual ~AsyncVoidReturnDirector() {}
    virtual void handler(HandlerErrorInfo& error) = 0;
};

class AsyncVoidNoErrReturnDirector
{
  public:
    virtual ~AsyncVoidNoErrReturnDirector() {}
    virtual void handler() = 0;
};

class AsyncStringReturnDirector
{
  public:
    virtual ~AsyncStringReturnDirector() {}
    virtual void handler(const std::string& ret, HandlerErrorInfo& error) = 0;
};

class AsyncUInt32ReturnDirector
{
  public:
    virtual ~AsyncUInt32ReturnDirector() {}
    virtual void handler(uint32_t ret, HandlerErrorInfo& error) = 0;
};

class AsyncTimerEventReturnDirector
{
  public:
    virtual ~AsyncTimerEventReturnDirector() {}
    virtual void handler(const TimerEvent& ret, HandlerErrorInfo& error) = 0;
};

class WrappedServiceStubDirector
{
  public:
    virtual ~WrappedServiceStubDirector() {}
    virtual void DispatchEvent(const std::string& EventName, std::vector<RR_INTRUSIVE_PTR<MessageElement> > args) = 0;
    virtual RR_INTRUSIVE_PTR<MessageElement> CallbackCall(const std::string& CallbackName,
                                                          std::vector<RR_INTRUSIVE_PTR<MessageElement> > args) = 0;
};

class WrappedPipeClient;
class WrappedWireClient;
class WrappedServiceStub;
class WrappedGeneratorClient;
class AsyncGeneratorClientReturnDirector;
class WrappedPodArrayMemoryClient;
class WrappedPodMultiDimArrayMemoryClient;
class WrappedNamedArrayMemoryClient;
class WrappedNamedMultiDimArrayMemoryClient;

class AsyncStubReturnDirector
{
  public:
    virtual ~AsyncStubReturnDirector() {}
    virtual void handler(const boost::shared_ptr<WrappedServiceStub>& stub, HandlerErrorInfo& error) = 0;
};

class WrappedServiceStub : public virtual RobotRaconteur::ServiceStub
{
  public:
    WrappedServiceStub(boost::string_ref path, const RR_SHARED_PTR<ServiceEntryDefinition>& type,
                       const RR_SHARED_PTR<RobotRaconteur::ClientContext>& c);
    RR_OVIRTUAL ~WrappedServiceStub() RR_OVERRIDE;

    RR_INTRUSIVE_PTR<MessageElement> PropertyGet(const std::string& PropertyName);
    void PropertySet(const std::string& PropertyName, const RR_INTRUSIVE_PTR<MessageElement>& value);
    RR_INTRUSIVE_PTR<MessageElement> FunctionCall(const std::string& FunctionName,
                                                  const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& args);
    RR_SHARED_PTR<WrappedGeneratorClient> GeneratorFunctionCall(
        const std::string& FunctionName, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& args);

    void async_PropertyGet(const std::string& PropertyName, int32_t timeout, AsyncRequestDirector* handler, int32_t id);
    void async_PropertySet(const std::string& PropertyName, const RR_INTRUSIVE_PTR<MessageElement>& value,
                           int32_t timeout, AsyncRequestDirector* handler, int32_t id);
    void async_FunctionCall(const std::string& FunctionName, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& args,
                            int32_t timeout, AsyncRequestDirector* handler, int32_t id);
    void async_GeneratorFunctionCall(const std::string& FunctionName,
                                     const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& args, int32_t timeout,
                                     AsyncGeneratorClientReturnDirector* handler, int32_t id);
    void async_FindObjRef(const std::string& path, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id);
    void async_FindObjRef(const std::string& path, const std::string& ind, int32_t timeout,
                          AsyncStubReturnDirector* handler, int32_t id);
    void async_FindObjRefTyped(const std::string& path, const std::string& type, int32_t timeout,
                               AsyncStubReturnDirector* handler, int32_t id);
    void async_FindObjRefTyped(const std::string& path, const std::string& ind, const std::string& type,
                               int32_t timeout, AsyncStubReturnDirector* handler, int32_t id);

  protected:
    void async_PropertyGet_handler(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m,
                                   const RR_SHARED_PTR<RobotRaconteurException>& err,
                                   const RR_SHARED_PTR<AsyncRequestDirector>& handler);
    void async_PropertySet_handler(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m,
                                   const RR_SHARED_PTR<RobotRaconteurException>& err,
                                   const RR_SHARED_PTR<AsyncRequestDirector>& handler);
    void async_FunctionCall_handler(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m,
                                    const RR_SHARED_PTR<RobotRaconteurException>& err,
                                    const RR_SHARED_PTR<AsyncRequestDirector>& handler);
    void async_GeneratorFunctionCall_handler(const std::string& FunctionName,
                                             const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m,
                                             const RR_SHARED_PTR<RobotRaconteurException>& err,
                                             const RR_SHARED_PTR<AsyncGeneratorClientReturnDirector>& handler);
    void async_FindObjRef_handler(const RR_SHARED_PTR<RRObject>& stub,
                                  const RR_SHARED_PTR<RobotRaconteurException>& err,
                                  const RR_SHARED_PTR<AsyncStubReturnDirector>& handler);

  public:
    RR_OVIRTUAL void DispatchEvent(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;
    RR_OVIRTUAL void DispatchPipeMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;
    RR_OVIRTUAL void DispatchWireMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallbackCall(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;
    RR_SHARED_PTR<RobotRaconteur::WrappedPipeClient> GetPipe(const std::string& membername);
    RR_SHARED_PTR<RobotRaconteur::WrappedWireClient> GetWire(const std::string& membername);
    RR_OVIRTUAL RR_SHARED_PTR<PipeClientBase> RRGetPipeClient(boost::string_ref membername) RR_OVERRIDE;
    RR_OVIRTUAL RR_SHARED_PTR<WireClientBase> RRGetWireClient(boost::string_ref membername) RR_OVERRIDE;
    RR_SHARED_PTR<RobotRaconteur::ArrayMemoryBase> GetArrayMemory(const std::string& membername);
    RR_SHARED_PTR<WrappedPodArrayMemoryClient> GetPodArrayMemory(const std::string& membername);
    RR_SHARED_PTR<WrappedNamedArrayMemoryClient> GetNamedArrayMemory(const std::string& membername);
    RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemoryBase> GetMultiDimArrayMemory(const std::string& membername);
    RR_SHARED_PTR<WrappedPodMultiDimArrayMemoryClient> GetPodMultiDimArrayMemory(const std::string& membername);
    RR_SHARED_PTR<WrappedNamedMultiDimArrayMemoryClient> GetNamedMultiDimArrayMemory(const std::string& membername);
    RR_OVIRTUAL void RRClose() RR_OVERRIDE;
    RR_OVIRTUAL void RRInitStub() RR_OVERRIDE;

    RR_SHARED_PTR<ServiceEntryDefinition> RR_objecttype;

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE;

  protected:
    boost::shared_ptr<WrappedServiceStubDirector> RR_Director;
    boost::shared_mutex RR_Director_lock;

    int objectheapid;

  public:
    // WrappedServiceStubDirector* GetRRDirector();
    void SetRRDirector(WrappedServiceStubDirector* director, int32_t id);

    std::map<std::string, RR_SHARED_PTR<WrappedPipeClient> > pipes;
    std::map<std::string, RR_SHARED_PTR<WrappedWireClient> > wires;
    std::map<std::string, RR_SHARED_PTR<ArrayMemoryBase> > arraymemories;
    std::map<std::string, RR_SHARED_PTR<MultiDimArrayMemoryBase> > multidimarraymemories;
    std::map<std::string, RR_SHARED_PTR<WrappedPodArrayMemoryClient> > pod_arraymemories;
    std::map<std::string, RR_SHARED_PTR<WrappedPodMultiDimArrayMemoryClient> > pod_multidimarraymemories;
    std::map<std::string, RR_SHARED_PTR<WrappedNamedArrayMemoryClient> > namedarray_arraymemories;
    std::map<std::string, RR_SHARED_PTR<WrappedNamedMultiDimArrayMemoryClient> > namedarray_multidimarraymemories;

    int GetObjectHeapID();

    // int32_t objectheapid;

#ifdef RR_PYTHON
  protected:
    boost::mutex pystub_lock;
    PyObject* pystub;

  public:
    PyObject* GetPyStub()
    {
        boost::mutex::scoped_lock lock(pystub_lock);

        RR_Ensure_GIL py_gil;

        if (pystub != NULL)
        {
            Py_XINCREF(pystub);
            return pystub;
        }
        else
        {
            PyObject* n = Py_None;
            Py_XINCREF(n);
            return n;
        }
    }

    void SetPyStub(PyObject* stub)
    {
        boost::mutex::scoped_lock lock(pystub_lock);

        RR_Ensure_GIL py_gil;

        if (pystub != NULL)
        {
            Py_XDECREF(pystub);
        }
        if (stub == Py_None)
        {
            pystub = NULL;
        }
        else
        {
            Py_XINCREF(stub);
            pystub = stub;
        }
    }

#endif
};

class WrappedStubCallbackDirector
{
  public:
    virtual ~WrappedStubCallbackDirector() {}
    virtual void Callback(ClientServiceListenerEventType) = 0;
};

class WrappedPipeBroadcaster;
class WrappedPipeEndpoint;
class WrappedPipeEndpointDirector
{
  public:
    virtual ~WrappedPipeEndpointDirector() {}
    virtual void PipeEndpointClosedCallback() = 0;
    virtual void PacketReceivedEvent() = 0;
    virtual void PacketAckReceivedEvent(uint32_t packetnum) = 0;
};

class WrappedTryReceivePacketWaitResult
{
  public:
    bool res;
    RR_INTRUSIVE_PTR<MessageElement> packet;

    WrappedTryReceivePacketWaitResult() : res(false) {}
};

class WrappedPipeEndpoint : public PipeEndpointBase
{

  public:
    friend class WrappedPipeBroadcaster;

    RR_OVIRTUAL ~WrappedPipeEndpoint() RR_OVERRIDE;

    uint32_t SendPacket(const RR_INTRUSIVE_PTR<MessageElement>& packet);
    void AsyncSendPacket(const RR_INTRUSIVE_PTR<MessageElement>& packet, AsyncUInt32ReturnDirector* handler,
                         int32_t id);
    RR_INTRUSIVE_PTR<MessageElement> ReceivePacket();
    RR_INTRUSIVE_PTR<MessageElement> PeekNextPacket();
    RR_INTRUSIVE_PTR<MessageElement> ReceivePacketWait(int32_t timeout = RR_TIMEOUT_INFINITE);
    RR_INTRUSIVE_PTR<MessageElement> PeekNextPacketWait(int32_t timeout = RR_TIMEOUT_INFINITE);
    WrappedTryReceivePacketWaitResult TryReceivePacketWait(int32_t timeout = RR_TIMEOUT_INFINITE, bool peek = false);

    WrappedPipeEndpoint(const RR_SHARED_PTR<PipeBase>& parent, int32_t index, uint32_t endpoint,
                        const RR_SHARED_PTR<TypeDefinition>& Type, bool unreliable,
                        MemberDefinition_Direction direction);
    RR_SHARED_PTR<TypeDefinition> Type;

  protected:
    RR_OVIRTUAL void fire_PipeEndpointClosedCallback() RR_OVERRIDE;
    RR_OVIRTUAL void fire_PacketReceivedEvent() RR_OVERRIDE;
    RR_OVIRTUAL void fire_PacketAckReceivedEvent(uint32_t packetnum) RR_OVERRIDE;

    boost::shared_ptr<WrappedPipeEndpointDirector> RR_Director;
    boost::shared_mutex RR_Director_lock;

    static void send_handler(
        uint32_t packetnumber, const RR_SHARED_PTR<RobotRaconteurException>& err,
        boost::function<void(const RR_SHARED_PTR<uint32_t>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler)
    {
        handler(RR_MAKE_SHARED<uint32_t>(packetnumber), err);
    }

    void AsyncSendPacket_handler(uint32_t id, const RR_SHARED_PTR<RobotRaconteurException>& err,
                                 const RR_SHARED_PTR<AsyncUInt32ReturnDirector>& handler);

  public:
    // WrappedPipeEndpointDirector* GetRRDirector();
    void SetRRDirector(WrappedPipeEndpointDirector* director, int32_t id);
    RR_SHARED_PTR<WrappedServiceStub> GetStub();

    // int32_t objectheapid;

    RR_OVIRTUAL void Close() RR_OVERRIDE;
    using PipeEndpointBase::AsyncClose;
    void AsyncClose(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id);

  protected:
    void AsyncClose_handler(const RR_SHARED_PTR<RobotRaconteurException>& err,
                            const RR_SHARED_PTR<AsyncVoidReturnDirector>& handler);

    void SetPipeBroadcaster(const RR_SHARED_PTR<WrappedPipeBroadcaster>& broadcaster,
                            const RR_SHARED_PTR<detail::PipeBroadcasterBase_connected_endpoint>& cep);

    RR_WEAK_PTR<WrappedPipeBroadcaster> broadcaster;
    RR_WEAK_PTR<detail::PipeBroadcasterBase_connected_endpoint> broadcaster_cep;
};

class AsyncPipeEndpointReturnDirector
{
  public:
    virtual ~AsyncPipeEndpointReturnDirector() {}
    virtual void handler(const boost::shared_ptr<WrappedPipeEndpoint>& ep, HandlerErrorInfo& error) = 0;
};

class WrappedPipeClient : public virtual PipeClientBase
{
  public:
    RR_OVIRTUAL ~WrappedPipeClient() RR_OVERRIDE {}

    // virtual boost::function<void(const RR_SHARED_PTR<WrappedPipeEndpoint>&)> GetPipeConnectCallback();
    // virtual void SetPipeConnectCallback(boost::function<void(const RR_SHARED_PTR<WrappedPipeEndpoint>&)> function);
    RR_SHARED_PTR<WrappedPipeEndpoint> Connect(int32_t index);
    void AsyncConnect(int32_t index, int32_t timeout, AsyncPipeEndpointReturnDirector* handler, int32_t id);
    WrappedPipeClient(const std::string& name, const RR_SHARED_PTR<ServiceStub>& stub,
                      const RR_SHARED_PTR<TypeDefinition>& Type, bool unreliable, MemberDefinition_Direction direction);

    RR_SHARED_PTR<TypeDefinition> Type;

  protected:
    RR_OVIRTUAL RR_SHARED_PTR<PipeEndpointBase> CreateNewPipeEndpoint(int32_t index, bool unreliable,
                                                                      MemberDefinition_Direction direction) RR_OVERRIDE;
    void AsyncConnect_handler(const RR_SHARED_PTR<PipeEndpointBase>& ep,
                              const RR_SHARED_PTR<RobotRaconteurException>& err,
                              const RR_SHARED_PTR<AsyncPipeEndpointReturnDirector>& handler);
};

class WrappedPipeServerConnectDirector
{
  public:
    virtual ~WrappedPipeServerConnectDirector() {}

    virtual void PipeConnectCallback(const boost::shared_ptr<WrappedPipeEndpoint>& e) = 0;

    void PipeConnectCallbackFire(const RR_SHARED_PTR<PipeEndpointBase>& e);
};

class WrappedPipeServer : public virtual PipeServerBase
{
  public:
    WrappedPipeServer(const std::string& name, const RR_SHARED_PTR<ServiceSkel>& skel,
                      const RR_SHARED_PTR<TypeDefinition>& Type, bool unreliable, MemberDefinition_Direction direction);
    RR_OVIRTUAL ~WrappedPipeServer() RR_OVERRIDE;

    RR_SHARED_PTR<TypeDefinition> Type;

  protected:
    boost::function<void(const RR_SHARED_PTR<PipeEndpointBase>&)> callback;

    boost::mutex callback_lock;

  public:
    // WrappedPipeServerDirector* GetRRDirector();
    void SetWrappedPipeConnectCallback(WrappedPipeServerConnectDirector* director, int32_t id);

    // int32_t objectheapid;

    boost::function<void(const RR_SHARED_PTR<PipeEndpointBase>&)> GetPipeConnectCallback();

    void SetPipeConnectCallback(boost::function<void(const RR_SHARED_PTR<PipeEndpointBase>&)> function);

    RR_OVIRTUAL void Shutdown() RR_OVERRIDE;

  protected:
    RR_OVIRTUAL RR_SHARED_PTR<PipeEndpointBase> CreateNewPipeEndpoint(int32_t index, uint32_t endpoint, bool unreliable,
                                                                      MemberDefinition_Direction direction) RR_OVERRIDE;

    RR_OVIRTUAL void fire_PipeConnectCallback(const RR_SHARED_PTR<PipeEndpointBase>& e) RR_OVERRIDE;
};

class WrappedPipeBroadcasterPredicateDirector
{
  public:
    virtual bool Predicate(uint32_t client_endpoint, int32_t index) = 0;
    bool CallPredicate(uint32_t client_endpoint, int32_t index);
    virtual ~WrappedPipeBroadcasterPredicateDirector() {}
};

class WrappedPipeBroadcaster : public PipeBroadcasterBase
{
  public:
    friend class WrappedPipeEndpoint;

    void Init(const RR_SHARED_PTR<WrappedPipeServer>& pipe, int32_t maximum_backlog = -1);

    void SendPacket(const RR_INTRUSIVE_PTR<MessageElement>& packet);

    void AsyncSendPacket(const RR_INTRUSIVE_PTR<MessageElement>& packet, AsyncVoidNoErrReturnDirector* handler,
                         int32_t id);

    void AsyncSendPacket_handler(const RR_SHARED_PTR<AsyncVoidNoErrReturnDirector>& handler);

    void SetPredicateDirector(WrappedPipeBroadcasterPredicateDirector* f, int32_t id);

  protected:
    RR_OVIRTUAL void AttachPipeServerEvents(const RR_SHARED_PTR<PipeServerBase>& p) RR_OVERRIDE;

    RR_OVIRTUAL void AttachPipeEndpointEvents(const RR_SHARED_PTR<PipeEndpointBase>& ep,
                                              const RR_SHARED_PTR<detail::PipeBroadcasterBase_connected_endpoint>& cep)
        RR_OVERRIDE;
};

class WrappedWireConnection;
class WrappedWireBroadcaster;
class WrappedWireUnicastReceiver;
class WrappedWireConnectionDirector
{
  public:
    virtual ~WrappedWireConnectionDirector() {}
    virtual void WireValueChanged(RR_INTRUSIVE_PTR<MessageElement> value, const TimeSpec& time) = 0;
    virtual void WireConnectionClosedCallback() = 0;
};

class TryGetValueResult
{
  public:
    bool res;
    RR_INTRUSIVE_PTR<MessageElement> value;
    TimeSpec ts;

    TryGetValueResult() : res(false) {}
};

class WrappedWireConnection : public virtual WireConnectionBase
{
  public:
    friend class WrappedWireBroadcaster;
    friend class WrappedWireUnicastReceiver;

    RR_OVIRTUAL ~WrappedWireConnection() RR_OVERRIDE;
    RR_INTRUSIVE_PTR<MessageElement> GetInValue();
    RR_INTRUSIVE_PTR<MessageElement> GetOutValue();
    void SetOutValue(const RR_INTRUSIVE_PTR<MessageElement>& value);

    TryGetValueResult TryGetInValue();
    TryGetValueResult TryGetOutValue();

    WrappedWireConnection(const RR_SHARED_PTR<WireBase>& parent, uint32_t endpoint,
                          const RR_SHARED_PTR<TypeDefinition>& Type, MemberDefinition_Direction direction);

    RR_OVIRTUAL void fire_WireValueChanged(const RR_INTRUSIVE_PTR<RRValue>& value, TimeSpec time) RR_OVERRIDE;
    RR_OVIRTUAL void fire_WireClosedCallback() RR_OVERRIDE;

    RR_SHARED_PTR<TypeDefinition> Type;

  protected:
    boost::shared_ptr<WrappedWireConnectionDirector> RR_Director;
    boost::shared_mutex RR_Director_lock;

  public:
    // WrappedWireConnectionDirector* GetRRDirector();
    void SetRRDirector(WrappedWireConnectionDirector* director, int32_t id);
    RR_SHARED_PTR<WrappedServiceStub> GetStub();

    // int32_t objectheapid;
    RR_OVIRTUAL void Close() RR_OVERRIDE;
    using WireConnectionBase::AsyncClose;
    void AsyncClose(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id);

  protected:
    void AsyncClose_handler(const RR_SHARED_PTR<RobotRaconteurException>& err,
                            const RR_SHARED_PTR<AsyncVoidReturnDirector>& handler);

    void SetWireBroadcaster(const RR_SHARED_PTR<WrappedWireBroadcaster>& broadcaster,
                            const RR_SHARED_PTR<detail::WireBroadcaster_connected_connection>& cep);
    void SetWireUnicastReceiver(const RR_SHARED_PTR<WrappedWireUnicastReceiver>& receiver);

    RR_WEAK_PTR<WrappedWireBroadcaster> broadcaster;
    RR_WEAK_PTR<detail::WireBroadcaster_connected_connection> broadcaster_cep;

    RR_WEAK_PTR<WrappedWireUnicastReceiver> unicast_receiver;
};

class AsyncWireConnectionReturnDirector
{
  public:
    virtual ~AsyncWireConnectionReturnDirector() {}
    virtual void handler(const boost::shared_ptr<WrappedWireConnection>& ep, HandlerErrorInfo& error) = 0;
};

class AsyncWirePeekReturnDirector
{
  public:
    virtual ~AsyncWirePeekReturnDirector() {}
    virtual void handler(RR_INTRUSIVE_PTR<MessageElement> value, const TimeSpec& ts, HandlerErrorInfo& error) = 0;
};

class WrappedWireClient : public virtual WireClientBase
{
  public:
    RR_OVIRTUAL ~WrappedWireClient() RR_OVERRIDE {}

    boost::function<void(const RR_SHARED_PTR<WrappedWireConnection>&)> GetWireConnectCallback();
    void SetWireConnectCallback(boost::function<void(const RR_SHARED_PTR<WrappedWireConnection>&)> function);
    RR_SHARED_PTR<WrappedWireConnection> Connect();
    void AsyncConnect(int32_t timeout, AsyncWireConnectionReturnDirector* handler, int32_t id);
    WrappedWireClient(const std::string& name, const RR_SHARED_PTR<ServiceStub>& stub,
                      const RR_SHARED_PTR<TypeDefinition>& Type, MemberDefinition_Direction direction);

    RR_INTRUSIVE_PTR<MessageElement> PeekInValue(TimeSpec& ts);
    RR_INTRUSIVE_PTR<MessageElement> PeekOutValue(TimeSpec& ts);
    void PokeOutValue(const RR_INTRUSIVE_PTR<MessageElement>& value);

    void AsyncPeekInValue(int32_t timeout, AsyncWirePeekReturnDirector* handler, int32_t id);
    void AsyncPeekOutValue(int32_t timeout, AsyncWirePeekReturnDirector* handler, int32_t id);
    void AsyncPokeOutValue(const RR_INTRUSIVE_PTR<MessageElement>& value, int32_t timeout,
                           AsyncVoidReturnDirector* handler, int32_t id);

    void AsyncPeekValue_handler(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts,
                                const RR_SHARED_PTR<RobotRaconteurException>& err,
                                const RR_SHARED_PTR<AsyncWirePeekReturnDirector>& handler);
    void AsyncPokeValue_handler(const RR_SHARED_PTR<RobotRaconteurException>& err,
                                const RR_SHARED_PTR<AsyncVoidReturnDirector>& handler);

    RR_SHARED_PTR<TypeDefinition> Type;

  protected:
    RR_OVIRTUAL RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(MemberDefinition_Direction direction)
        RR_OVERRIDE;
    void AsyncConnect_handler(const RR_SHARED_PTR<WireConnectionBase>& ep,
                              const RR_SHARED_PTR<RobotRaconteurException>& err,
                              const RR_SHARED_PTR<AsyncWireConnectionReturnDirector>& handler);
};

class WrappedWireServerConnectDirector
{
  public:
    virtual ~WrappedWireServerConnectDirector() {}
    virtual void WireConnectCallback(const boost::shared_ptr<WrappedWireConnection>& c) = 0;
    void WireConnectCallbackFire(const RR_SHARED_PTR<WireConnectionBase>& e);
};

class IWrappedWirePeekPokeCallbacks;

class WrappedWireServerPeekValueDirector
{
  public:
    virtual ~WrappedWireServerPeekValueDirector() {}
    virtual RR_INTRUSIVE_PTR<MessageElement> PeekValue(const uint32_t& ep) = 0;
};

class WrappedWireServerPokeValueDirector
{
  public:
    virtual ~WrappedWireServerPokeValueDirector() {}
    virtual void PokeValue(RR_INTRUSIVE_PTR<MessageElement> value, const TimeSpec& ts, const uint32_t& ep) = 0;
};

class WrappedWireServer : public WireServerBase
{
  public:
    WrappedWireServer(const std::string& name, const RR_SHARED_PTR<ServiceSkel>& skel,
                      const RR_SHARED_PTR<TypeDefinition>& Type, MemberDefinition_Direction direction);
    RR_OVIRTUAL ~WrappedWireServer() RR_OVERRIDE;
    RR_SHARED_PTR<TypeDefinition> Type;

  protected:
    RR_OVIRTUAL RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(
        uint32_t endpoint, MemberDefinition_Direction direction) RR_OVERRIDE;
    RR_OVIRTUAL void fire_WireConnectCallback(const RR_SHARED_PTR<WireConnectionBase>& e) RR_OVERRIDE;

    boost::function<void(const RR_SHARED_PTR<WireConnectionBase>&)> callback;

    boost::mutex callback_lock;

    RR_WEAK_PTR<IWrappedWirePeekPokeCallbacks> wrapped_peek_poke_callbacks;

  public:
    // WrappedWireServerDirector* GetRRDirector();
    void SetWrappedWireConnectCallback(WrappedWireServerConnectDirector* director, int32_t id);

    // int32_t objectheapid;

    void SetWireConnectCallback(boost::function<void(const RR_SHARED_PTR<WireConnectionBase>&)> function);

    // For WireBroadcaster and WireUnicastReceiver
    void SetWireWrappedPeekPokeCallbacks(const RR_SHARED_PTR<IWrappedWirePeekPokeCallbacks>& peek_poke_callbacks);

    RR_OVIRTUAL void Shutdown() RR_OVERRIDE;

    void SetPeekInValueCallback(WrappedWireServerPeekValueDirector* director, int32_t id);
    void SetPeekOutValueCallback(WrappedWireServerPeekValueDirector* director, int32_t id);
    void SetPokeOutValueCallback(WrappedWireServerPokeValueDirector* director, int32_t id);

  protected:
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RRValue> do_PeekInValue(const uint32_t& ep) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RRValue> do_PeekOutValue(const uint32_t& ep) RR_OVERRIDE;
    RR_OVIRTUAL void do_PokeOutValue(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts,
                                     const uint32_t& ep) RR_OVERRIDE;

    RR_SHARED_PTR<WrappedWireServerPeekValueDirector> peek_invalue_director;
    RR_SHARED_PTR<WrappedWireServerPeekValueDirector> peek_outvalue_director;
    RR_SHARED_PTR<WrappedWireServerPokeValueDirector> poke_outvalue_director;
};

class IWrappedWirePeekPokeCallbacks
{
  public:
    virtual RR_INTRUSIVE_PTR<RRValue> do_PeekInValue(const uint32_t& ep) = 0;
    virtual RR_INTRUSIVE_PTR<RRValue> do_PeekOutValue(const uint32_t& ep) = 0;
    virtual void do_PokeOutValue(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts, const uint32_t& ep) = 0;

    virtual ~IWrappedWirePeekPokeCallbacks() {}
};

class WrappedWireBroadcasterPredicateDirector
{
  public:
    virtual bool Predicate(uint32_t client_endpoint) = 0;
    bool CallPredicate(uint32_t client_endpoint);
    virtual ~WrappedWireBroadcasterPredicateDirector() {}
};

class WrappedWireBroadcaster : public WireBroadcasterBase, public IWrappedWirePeekPokeCallbacks
{
  public:
    friend class WrappedWireConnection;

    void Init(const RR_SHARED_PTR<WrappedWireServer>& wire);

    void SetOutValue(const RR_INTRUSIVE_PTR<MessageElement>& value);

    void SetPredicateDirector(WrappedWireBroadcasterPredicateDirector* f, int32_t id);

    void SetPeekInValueCallback(WrappedWireServerPeekValueDirector* director, int32_t id);

  protected:
    RR_OVIRTUAL void AttachWireServerEvents(const RR_SHARED_PTR<WireServerBase>& w) RR_OVERRIDE;

    RR_OVIRTUAL void AttachWireConnectionEvents(const RR_SHARED_PTR<WireConnectionBase>& w,
                                                const RR_SHARED_PTR<detail::WireBroadcaster_connected_connection>& cep)
        RR_OVERRIDE;

  public:
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RRValue> do_PeekInValue(const uint32_t& ep) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RRValue> do_PeekOutValue(const uint32_t& ep) RR_OVERRIDE;
    RR_OVIRTUAL void do_PokeOutValue(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts,
                                     const uint32_t& ep) RR_OVERRIDE;

  protected:
    RR_SHARED_PTR<WrappedWireServerPeekValueDirector> peek_invalue_director;
};

namespace detail
{
template <>
class Wire_traits<WrappedWireServer>
{
  public:
    typedef WrappedWireConnection wireconnection_type;
    typedef WrappedWireClient wireclient_type;
    typedef WrappedWireServer wireserver_type;
};

} // namespace detail

class WrappedService_typed_packet;

class WrappedWireUnicastReceiver : public WireUnicastReceiverBase<WrappedWireServer, RR_INTRUSIVE_PTR<MessageElement> >,
                                   public IWrappedWirePeekPokeCallbacks
{
  public:
    friend class WrappedWireConnection;

    void Init(const RR_SHARED_PTR<WrappedWireServer>& server);

    void WrappedConnectionConnected(const RR_SHARED_PTR<WireConnectionBase>& connection);

    RR_OVIRTUAL RR_INTRUSIVE_PTR<RRValue> do_PeekInValue(const uint32_t& ep) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RRValue> do_PeekOutValue(const uint32_t& ep) RR_OVERRIDE;
    RR_OVIRTUAL void do_PokeOutValue(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts,
                                     const uint32_t& ep) RR_OVERRIDE;

    void AddInValueChangedListener(WrappedWireServerPokeValueDirector* director, int32_t id);
    static void ValueChanged(const RR_INTRUSIVE_PTR<MessageElement>& m, const TimeSpec& ts, const uint32_t& ep,
                             const RR_SHARED_PTR<WrappedWireServerPokeValueDirector>& spdirector);

    bool TryGetInValue(WrappedService_typed_packet& val, TimeSpec& time);
};

class AsyncGeneratorClientReturnDirector
{
  public:
    virtual ~AsyncGeneratorClientReturnDirector() {}
    virtual void handler(const boost::shared_ptr<WrappedGeneratorClient>& ret, HandlerErrorInfo& error) = 0;
};

class WrappedGeneratorClient_TryGetNextResult
{
  public:
    RR_INTRUSIVE_PTR<MessageElement> value;
    bool res;
    WrappedGeneratorClient_TryGetNextResult() : res(false) {}
};

class WrappedGeneratorClient : public GeneratorClientBase
{
  public:
    WrappedGeneratorClient(const std::string& name, int32_t id, const RR_SHARED_PTR<ServiceStub>& stub);

    RR_INTRUSIVE_PTR<MessageElement> Next(const RR_INTRUSIVE_PTR<MessageElement>& v);
    WrappedGeneratorClient_TryGetNextResult TryNext(const RR_INTRUSIVE_PTR<MessageElement>& v);
    void AsyncNext(const RR_INTRUSIVE_PTR<MessageElement>& v, int32_t timeout, AsyncRequestDirector* handler,
                   int32_t id);

    void AsyncAbort(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id);
    void AsyncClose(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id);

    std::vector<RR_INTRUSIVE_PTR<MessageElement> > NextAll();

  protected:
    static void AsyncNext_handler(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>& m,
                                  const RR_SHARED_PTR<RobotRaconteurException>& err,
                                  const RR_SHARED_PTR<AsyncRequestDirector>& handler);
    static void AsyncAbort_handler(const RR_SHARED_PTR<RobotRaconteurException>& err,
                                   const RR_SHARED_PTR<AsyncVoidReturnDirector>& handler);
};

class WrappedServiceSkelAsyncAdapter;
class WrappedGeneratorServerDirector
{
  public:
    WrappedGeneratorServerDirector() { objectheapid = 0; }
    virtual ~WrappedGeneratorServerDirector() {}

    virtual RR_INTRUSIVE_PTR<MessageElement> Next(
        RR_INTRUSIVE_PTR<MessageElement> m, const boost::shared_ptr<WrappedServiceSkelAsyncAdapter>& async_adapter) = 0;

    virtual void Abort(const boost::shared_ptr<WrappedServiceSkelAsyncAdapter>& async_adapter) = 0;
    virtual void Close(const boost::shared_ptr<WrappedServiceSkelAsyncAdapter>& async_adapter) = 0;

    int32_t objectheapid;
};

class WrappedGeneratorServer : public GeneratorServerBase
{
  public:
    WrappedGeneratorServer(const std::string& name, int32_t index, const RR_SHARED_PTR<ServiceSkel>& skel,
                           const RR_SHARED_PTR<ServerEndpoint>& ep, WrappedGeneratorServerDirector* director);

    RR_OVIRTUAL void CallNext(const RR_INTRUSIVE_PTR<MessageEntry>& m) RR_OVERRIDE;

    boost::shared_ptr<WrappedGeneratorServerDirector> RR_Director;
};

class RRMultiDimArrayUntyped
{
  public:
    RR_INTRUSIVE_PTR<RRBaseArray> Dims;

    RR_INTRUSIVE_PTR<RRBaseArray> Array;
};

class WrappedArrayMemoryClientUtil
{
  public:
    static RR_INTRUSIVE_PTR<RRBaseArray> Read(const RR_SHARED_PTR<ArrayMemoryBase>& mem, uint64_t memorypos,
                                              uint64_t count);

    static void Write(const RR_SHARED_PTR<ArrayMemoryBase>& mem, uint64_t memorypos,
                      const RR_INTRUSIVE_PTR<RRBaseArray>& buffer, uint64_t bufferpos, uint64_t count);

    static MemberDefinition_Direction Direction(const RR_SHARED_PTR<ArrayMemoryBase>& mem);
};

class WrappedMultiDimArrayMemoryClientUtil
{
  public:
    static RR_SHARED_PTR<RRMultiDimArrayUntyped> Read(const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem,
                                                      std::vector<uint64_t> memorypos, std::vector<uint64_t> count);

    static void Write(const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem, std::vector<uint64_t> memorypos,
                      const RR_SHARED_PTR<RRMultiDimArrayUntyped>& buffer, std::vector<uint64_t> bufferpos,
                      std::vector<uint64_t> count);

    static MemberDefinition_Direction Direction(const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem);
};

class WrappedPodArrayMemoryClientBuffer
{
  public:
    virtual void UnpackReadResult(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> res,
                                  uint64_t bufferpos, uint64_t count) = 0;
    virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackWriteRequest(uint64_t bufferpos,
                                                                                               uint64_t count) = 0;
    virtual uint64_t GetBufferLength() = 0;
    virtual ~WrappedPodArrayMemoryClientBuffer() {}
};

class WrappedPodArrayMemoryClient : public virtual ArrayMemoryClientBase
{
  public:
    WrappedPodArrayMemoryClient(const std::string& membername, const RR_SHARED_PTR<ServiceStub>& stub,
                                size_t element_size, MemberDefinition_Direction direction);
    void Read(uint64_t memorypos, WrappedPodArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);
    void Write(uint64_t memorypos, WrappedPodArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);

  protected:
    RR_OVIRTUAL void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer, uint64_t bufferpos,
                                      uint64_t count) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer, uint64_t bufferpos,
                                                                      uint64_t count) RR_OVERRIDE;
    RR_OVIRTUAL size_t GetBufferLength(const void* buffer) RR_OVERRIDE;
};

class WrappedPodMultiDimArrayMemoryClientBuffer
{
  public:
    virtual void UnpackReadResult(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> res,
                                  const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
    virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackWriteRequest(
        const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
    virtual ~WrappedPodMultiDimArrayMemoryClientBuffer() {}
};

class WrappedPodMultiDimArrayMemoryClient : public virtual MultiDimArrayMemoryClientBase
{
  public:
    WrappedPodMultiDimArrayMemoryClient(const std::string& membername, const RR_SHARED_PTR<ServiceStub>& stub,
                                        size_t element_size, MemberDefinition_Direction direction);
    void Read(const std::vector<uint64_t>& memorypos, WrappedPodMultiDimArrayMemoryClientBuffer* buffer,
              const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
    void Write(const std::vector<uint64_t>& memorypos, WrappedPodMultiDimArrayMemoryClientBuffer* buffer,
               const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);

  protected:
    RR_OVIRTUAL void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer,
                                      const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count,
                                      uint64_t elemcount) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer,
                                                                      const std::vector<uint64_t>& bufferpos,
                                                                      const std::vector<uint64_t>& count,
                                                                      uint64_t elemcount) RR_OVERRIDE;
};

// namedarray memory
class WrappedNamedArrayMemoryClientBuffer
{
  public:
    virtual void UnpackReadResult(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> res,
                                  uint64_t bufferpos, uint64_t count) = 0;
    virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackWriteRequest(uint64_t bufferpos,
                                                                                               uint64_t count) = 0;
    virtual uint64_t GetBufferLength() = 0;
    virtual ~WrappedNamedArrayMemoryClientBuffer() {}
};

class WrappedNamedArrayMemoryClient : public virtual ArrayMemoryClientBase
{
  public:
    WrappedNamedArrayMemoryClient(const std::string& membername, const RR_SHARED_PTR<ServiceStub>& stub,
                                  size_t element_size, MemberDefinition_Direction direction);
    void Read(uint64_t memorypos, WrappedNamedArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);
    void Write(uint64_t memorypos, WrappedNamedArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);

  protected:
    RR_OVIRTUAL void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer, uint64_t bufferpos,
                                      uint64_t count) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer, uint64_t bufferpos,
                                                                      uint64_t count) RR_OVERRIDE;
    RR_OVIRTUAL size_t GetBufferLength(const void* buffer) RR_OVERRIDE;
};

class WrappedNamedMultiDimArrayMemoryClientBuffer
{
  public:
    virtual void UnpackReadResult(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> res,
                                  const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
    virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackWriteRequest(
        const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
    virtual ~WrappedNamedMultiDimArrayMemoryClientBuffer() {}
};

class WrappedNamedMultiDimArrayMemoryClient : public virtual MultiDimArrayMemoryClientBase
{
  public:
    WrappedNamedMultiDimArrayMemoryClient(const std::string& membername, const RR_SHARED_PTR<ServiceStub>& stub,
                                          size_t element_size, MemberDefinition_Direction direction);
    void Read(const std::vector<uint64_t>& memorypos, WrappedNamedMultiDimArrayMemoryClientBuffer* buffer,
              const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
    void Write(const std::vector<uint64_t>& memorypos, WrappedNamedMultiDimArrayMemoryClientBuffer* buffer,
               const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);

  protected:
    RR_OVIRTUAL void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer,
                                      const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count,
                                      uint64_t elemcount) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer,
                                                                      const std::vector<uint64_t>& bufferpos,
                                                                      const std::vector<uint64_t>& count,
                                                                      uint64_t elemcount) RR_OVERRIDE;
};
//

class ServiceInfo2Wrapped
{
  public:
    std::string Name;
    std::string RootObjectType;
    std::vector<std::string> RootObjectImplements;
    std::vector<std::string> ConnectionURL;
    boost::intrusive_ptr<RobotRaconteur::MessageElement> Attributes;
    RobotRaconteur::NodeID NodeID;
    std::string NodeName;

    ServiceInfo2Wrapped() {}
    ServiceInfo2Wrapped(const ServiceInfo2& value);
};

std::vector<RobotRaconteur::ServiceInfo2Wrapped> WrappedFindServiceByType(
    const RR_SHARED_PTR<RobotRaconteurNode>& node, const std::string& servicetype,
    const std::vector<std::string>& transportschemes);

std::vector<RobotRaconteur::NodeInfo2> WrappedFindNodeByID(const RR_SHARED_PTR<RobotRaconteurNode>& node,
                                                           const NodeID& id,
                                                           const std::vector<std::string>& transportschemes);

std::vector<RobotRaconteur::NodeInfo2> WrappedFindNodeByName(const RR_SHARED_PTR<RobotRaconteurNode>& node,
                                                             const std::string& name,
                                                             const std::vector<std::string>& transportschemes);

class AsyncServiceInfo2VectorReturnDirector
{
  public:
    virtual ~AsyncServiceInfo2VectorReturnDirector() {}
    virtual void handler(const std::vector<ServiceInfo2Wrapped>& ret) = 0;
};

class AsyncNodeInfo2VectorReturnDirector
{
  public:
    virtual ~AsyncNodeInfo2VectorReturnDirector() {}
    virtual void handler(const std::vector<NodeInfo2>& ret) = 0;
};

void AsyncServiceInfo2VectorReturn_handler(const RR_SHARED_PTR<std::vector<ServiceInfo2> >& ret,
                                           const RR_SHARED_PTR<AsyncServiceInfo2VectorReturnDirector>& handler);
void AsyncNodeInfo2VectorReturn_handler(const RR_SHARED_PTR<std::vector<NodeInfo2> >& ret,
                                        const RR_SHARED_PTR<AsyncNodeInfo2VectorReturnDirector>& handler);

void AsyncWrappedFindServiceByType(const RR_SHARED_PTR<RobotRaconteurNode>& node, const std::string& servicetype,
                                   const std::vector<std::string>& transportschemes, int32_t timeout,
                                   AsyncServiceInfo2VectorReturnDirector* handler, int32_t id1);

void AsyncWrappedFindNodeByID(const RR_SHARED_PTR<RobotRaconteurNode>& node, const NodeID& id,
                              const std::vector<std::string>& transportschemes, int32_t timeout,
                              AsyncNodeInfo2VectorReturnDirector* handler, int32_t id1);

void AsyncWrappedFindNodeByName(const RR_SHARED_PTR<RobotRaconteurNode>& node, const std::string& name,
                                const std::vector<std::string>& transportschemes, int32_t timeout,
                                AsyncNodeInfo2VectorReturnDirector* handler, int32_t id);

void WrappedUpdateDetectedNodes(const RR_SHARED_PTR<RobotRaconteurNode>& node, const std::vector<std::string>& schemes);

void AsyncWrappedUpdateDetectedNodes(const RR_SHARED_PTR<RobotRaconteurNode>& node,
                                     const std::vector<std::string>& schemes, int32_t timeout,
                                     AsyncVoidNoErrReturnDirector* handler, int32_t id1);

std::vector<std::string> WrappedGetDetectedNodes(const RR_SHARED_PTR<RobotRaconteurNode>& node);

NodeInfo2 WrappedGetDetectedNodeCacheInfo(const boost::shared_ptr<RobotRaconteurNode>& node,
                                          const RobotRaconteur::NodeID& nodeid);

bool WrappedTryGetDetectedNodeCacheInfo(const boost::shared_ptr<RobotRaconteurNode>& node,
                                        const RobotRaconteur::NodeID& nodeid, NodeInfo2& nodeinfo2);

class WrappedRRObject;
class WrappedServiceSkel;
class WrappedArrayMemoryDirector;
class WrappedMultiDimArrayMemoryDirector;
class WrappedPodArrayMemoryDirector;
class WrappedPodMultiDimArrayMemoryDirector;
class WrappedPodArrayMemoryServiceSkel;
class WrappedPodMultiDimArrayMemoryServiceSkel;
class WrappedNamedArrayMemoryDirector;
class WrappedNamedMultiDimArrayMemoryDirector;
class WrappedNamedArrayMemoryServiceSkel;
class WrappedNamedMultiDimArrayMemoryServiceSkel;
class WrappedGeneratorServerDirector;
class WrappedServiceSkelDirector
{
  public:
    virtual ~WrappedServiceSkelDirector() {}
    virtual void Init(const boost::shared_ptr<WrappedServiceSkel>& skel) = 0;
    virtual RR_INTRUSIVE_PTR<MessageElement> CallGetProperty(
        const std::string& name, const boost::shared_ptr<WrappedServiceSkelAsyncAdapter>& async_adapter) = 0;
    virtual void CallSetProperty(const std::string& name, RR_INTRUSIVE_PTR<MessageElement> m,
                                 const boost::shared_ptr<WrappedServiceSkelAsyncAdapter>& async_adapter) = 0;
    ;
    virtual RR_INTRUSIVE_PTR<MessageElement> CallFunction(
        const std::string& name, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& m,
        const boost::shared_ptr<WrappedServiceSkelAsyncAdapter>& async_adapter) = 0;
    virtual boost::shared_ptr<WrappedRRObject> GetSubObj(const std::string& name, const std::string& index) = 0;
    virtual WrappedArrayMemoryDirector* GetArrayMemory(const std::string& name) = 0;
    virtual WrappedMultiDimArrayMemoryDirector* GetMultiDimArrayMemory(const std::string& name) = 0;
    virtual WrappedPodArrayMemoryDirector* GetPodArrayMemory(const std::string& name) = 0;
    virtual WrappedPodMultiDimArrayMemoryDirector* GetPodMultiDimArrayMemory(const std::string& name) = 0;
    virtual WrappedNamedArrayMemoryDirector* GetNamedArrayMemory(const std::string& name) = 0;
    virtual WrappedNamedMultiDimArrayMemoryDirector* GetNamedMultiDimArrayMemory(const std::string& name) = 0;

    virtual void MonitorEnter(int32_t timeout) = 0;
    virtual void MonitorExit() = 0;
    virtual void ReleaseCastObject() = 0;
};

class WrappedServiceSkel : public virtual ServiceSkel
{
  public:
    /*WrappedServiceSkel()
    {
        RR_Director=NULL;
    }*/

    RR_OVIRTUAL ~WrappedServiceSkel() RR_OVERRIDE {}
    boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> Type;

    boost::shared_ptr<WrappedRRObject> castobj;
    RR_OVIRTUAL void Init(boost::string_ref s, const RR_SHARED_PTR<RRObject>& o,
                          const RR_SHARED_PTR<ServerContext>& c) RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageEntry> CallGetProperty(const RR_INTRUSIVE_PTR<MessageEntry>& m) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageEntry> CallSetProperty(const RR_INTRUSIVE_PTR<MessageEntry>& m) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageEntry> CallFunction(const RR_INTRUSIVE_PTR<MessageEntry>& m) RR_OVERRIDE;
    RR_OVIRTUAL RR_SHARED_PTR<RRObject> GetSubObj(boost::string_ref name, boost::string_ref ind) RR_OVERRIDE;
    RR_OVIRTUAL void ReleaseCastObject() RR_OVERRIDE;

    RR_OVIRTUAL std::string GetObjectType() RR_OVERRIDE;

  protected:
    boost::shared_ptr<WrappedServiceSkelDirector> RR_Director;
    boost::shared_mutex RR_Director_lock;

  public:
    // WrappedServiceSkelDirector* GetRRDirector();
    // void SetRRDirector(WrappedServiceSkelDirector* director, int32_t id);

    RR_SHARED_PTR<RobotRaconteur::WrappedPipeServer> GetPipe(const std::string& membername);
    RR_SHARED_PTR<RobotRaconteur::WrappedWireServer> GetWire(const std::string& membername);

    std::map<std::string, RR_SHARED_PTR<WrappedPipeServer> > pipes;
    std::map<std::string, RR_SHARED_PTR<WrappedWireServer> > wires;

    RR_OVIRTUAL void DispatchPipeMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m,
                                         uint32_t endpoint) RR_OVERRIDE;
    RR_OVIRTUAL void DispatchWireMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m,
                                         uint32_t endpoint) RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageEntry> CallPipeFunction(const RR_INTRUSIVE_PTR<MessageEntry>& m,
                                                                uint32_t e) RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageEntry> CallWireFunction(const RR_INTRUSIVE_PTR<MessageEntry>& m,
                                                                uint32_t e) RR_OVERRIDE;

    void WrappedDispatchEvent(const std::string& name,
                              const std::vector<RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> >& m);

    RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> WrappedCallbackCall(
        const std::string& name, uint32_t endpoint,
        const std::vector<RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> >& m);

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageEntry> CallMemoryFunction(const RR_INTRUSIVE_PTR<MessageEntry>& m,
                                                                  const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE;

    std::map<std::string, boost::shared_ptr<void> > memories;
    std::map<std::string, boost::shared_ptr<WrappedPodArrayMemoryServiceSkel> > pod_memories;
    std::map<std::string, boost::shared_ptr<WrappedPodMultiDimArrayMemoryServiceSkel> > pod_multidimmemories;
    std::map<std::string, boost::shared_ptr<WrappedNamedArrayMemoryServiceSkel> > namedarray_memories;
    std::map<std::string, boost::shared_ptr<WrappedNamedMultiDimArrayMemoryServiceSkel> > namedarray_multidimmemories;
    std::map<std::string, boost::shared_ptr<MemoryDefinition> > memorytypes;

    int32_t RegisterGeneratorServer(const std::string& function_name, WrappedGeneratorServerDirector* gen);

    std::map<std::string, std::set<MessageEntryType> > nolocks;
    RR_OVIRTUAL bool IsRequestNoLock(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;
};

class WrappedServiceSkelAsyncAdapter
{
  protected:
    boost::function<void(const RR_INTRUSIVE_PTR<MessageElement>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
        handler;
    bool is_async;

  public:
    WrappedServiceSkelAsyncAdapter();
    void SetHandler(
        boost::function<void(const RR_INTRUSIVE_PTR<MessageElement>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);
    void MakeAsync();
    bool IsAsync();
    void End(const HandlerErrorInfo& err);
    void End(const RR_INTRUSIVE_PTR<MessageElement>& ret, const HandlerErrorInfo& err);
};

class WrappedRRObject : public RRObject, public IRobotRaconteurMonitorObject
{
  public:
    WrappedRRObject(const std::string& type, WrappedServiceSkelDirector* RR_Director, int32_t id);
    RR_OVIRTUAL std::string RRType() RR_OVERRIDE;
    std::string Type;

  protected:
    boost::shared_ptr<WrappedServiceSkelDirector> RR_Director;
    boost::shared_mutex RR_Director_lock;

  public:
    boost::shared_ptr<WrappedServiceSkelDirector> GetRRDirector();
    // void SetRRDirector(WrappedServiceSkelDirector* director, int32_t id);

    RR_OVIRTUAL void RobotRaconteurMonitorEnter() RR_OVERRIDE
    {
        DIRECTOR_CALL(WrappedServiceSkelDirector, RR_Director2->MonitorEnter(-1));
    }

    RR_OVIRTUAL void RobotRaconteurMonitorEnter(int32_t timeout) RR_OVERRIDE
    {

        DIRECTOR_CALL(WrappedServiceSkelDirector, RR_Director2->MonitorEnter(timeout));
    }

    RR_OVIRTUAL void RobotRaconteurMonitorExit() RR_OVERRIDE
    {
        DIRECTOR_CALL(WrappedServiceSkelDirector, RR_Director2->MonitorExit());
    }

    RR_OVIRTUAL ~WrappedRRObject() RR_OVERRIDE;
};

class WrappedArrayMemoryDirector
{
  public:
    WrappedArrayMemoryDirector() { objectheapid = 0; }

    virtual ~WrappedArrayMemoryDirector() {}
    virtual uint64_t Length() = 0;
    virtual void Read(uint64_t memorypos, boost::intrusive_ptr<RRBaseArray> buffer, uint64_t bufferpos,
                      uint64_t count) = 0;
    virtual void Write(uint64_t memorypos, boost::intrusive_ptr<RRBaseArray> buffer, uint64_t bufferpos,
                       uint64_t count) = 0;

    int32_t objectheapid;
};

template <class T>
class WrappedArrayMemory : public virtual ArrayMemory<T>
{
  public:
    boost::shared_ptr<WrappedArrayMemoryDirector> RR_Director;

    boost::shared_mutex RR_Director_lock;

    WrappedArrayMemory(WrappedArrayMemoryDirector* RR_Director)
    {
        // this->RR_Director=0;
        if (!RR_Director)
            throw InvalidArgumentException("RR_Director cannot be null");
        this->RR_Director.reset(RR_Director, boost::bind(&ReleaseDirector<WrappedArrayMemoryDirector>,
                                                         RR_BOOST_PLACEHOLDERS(_1), RR_Director->objectheapid));
    }

    RR_OVIRTUAL ~WrappedArrayMemory() RR_OVERRIDE {}

    RR_OVIRTUAL uint64_t Length() RR_OVERRIDE
    {
        if (!RR_Director)
            throw InvalidOperationException("Director not set");
        uint64_t len = 0;
        DIRECTOR_CALL(WrappedArrayMemoryDirector, len = RR_Director2->Length());
        return len;
    }
    RR_OVIRTUAL void Read(uint64_t memorypos, boost::intrusive_ptr<RRArray<T> >& buffer, uint64_t bufferpos,
                          uint64_t count) RR_OVERRIDE
    {
        if (!RR_Director)
            throw InvalidOperationException("Director not set");
        boost::intrusive_ptr<RRBaseArray> buffer2 = rr_cast<RRBaseArray>(buffer);
        DIRECTOR_CALL(WrappedArrayMemoryDirector, RR_Director2->Read(memorypos, buffer2, bufferpos, count));
    }
    RR_OVIRTUAL void Write(uint64_t memorypos, const boost::intrusive_ptr<RRArray<T> >& buffer, uint64_t bufferpos,
                           uint64_t count) RR_OVERRIDE
    {
        if (!RR_Director)
            throw InvalidOperationException("Director not set");
        boost::intrusive_ptr<RRBaseArray> buffer2 = rr_cast<RRBaseArray>(buffer);
        DIRECTOR_CALL(WrappedArrayMemoryDirector, RR_Director2->Write(memorypos, buffer2, bufferpos, count));
    }
};

class WrappedMultiDimArrayMemoryParams
{
  public:
    std::vector<uint64_t> memorypos;
    boost::shared_ptr<RRMultiDimArrayUntyped> buffer;
    std::vector<uint64_t> bufferpos;
    std::vector<uint64_t> count;
};

class WrappedMultiDimArrayMemoryDirector
{
  public:
    /*WrappedMultiDimArrayMemoryDirector()
    {
        objectheapid=0;
    }*/

    virtual ~WrappedMultiDimArrayMemoryDirector(){};
    virtual std::vector<uint64_t> Dimensions() = 0;
    virtual uint64_t DimCount() = 0;
    virtual void Read(WrappedMultiDimArrayMemoryParams* p) = 0;
    virtual void Write(WrappedMultiDimArrayMemoryParams* p) = 0;

    int32_t objectheapid;

    WrappedMultiDimArrayMemoryDirector() : objectheapid(0) {}
};

template <class T>
class WrappedMultiDimArrayMemory : public virtual MultiDimArrayMemory<T>
{
  public:
    boost::shared_ptr<WrappedMultiDimArrayMemoryDirector> RR_Director;
    boost::shared_mutex RR_Director_lock;
    WrappedMultiDimArrayMemory(WrappedMultiDimArrayMemoryDirector* RR_Director)
    {
        // this->RR_Director=0;
        if (!RR_Director)
            throw InvalidArgumentException("RR_Director cannot be null");
        this->RR_Director.reset(RR_Director, boost::bind(&ReleaseDirector<WrappedMultiDimArrayMemoryDirector>,
                                                         RR_BOOST_PLACEHOLDERS(_1), RR_Director->objectheapid));
    }

    virtual ~WrappedMultiDimArrayMemory() {}

    RR_OVIRTUAL std::vector<uint64_t> Dimensions() RR_OVERRIDE
    {
        if (!RR_Director)
            throw InvalidOperationException("Director not set");
        std::vector<uint64_t> dims;
        DIRECTOR_CALL(WrappedMultiDimArrayMemoryDirector, dims = RR_Director2->Dimensions());
        return dims;
    }

    RR_OVIRTUAL uint64_t DimCount() RR_OVERRIDE
    {
        if (!RR_Director)
            throw InvalidOperationException("Director not set");
        uint64_t dim = 0;
        DIRECTOR_CALL(WrappedMultiDimArrayMemoryDirector, dim = RR_Director2->DimCount());
        return dim;
    }

    RR_OVIRTUAL void Read(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& buffer,
                          const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) RR_OVERRIDE
    {
        if (!RR_Director)
            throw InvalidOperationException("Director not set");
        WrappedMultiDimArrayMemoryParams p;
        p.memorypos = memorypos;
        boost::shared_ptr<RRMultiDimArrayUntyped> buffer2 = boost::make_shared<RRMultiDimArrayUntyped>();
        buffer2->Dims = buffer->Dims;
        buffer2->Array = buffer->Array;

        p.buffer = buffer2;
        p.bufferpos = bufferpos;
        p.count = count;

        DIRECTOR_CALL(WrappedMultiDimArrayMemoryDirector, RR_Director2->Read(&p))
    }

    RR_OVIRTUAL void Write(const std::vector<uint64_t>& memorypos, const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& buffer,
                           const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) RR_OVERRIDE
    {
        if (!RR_Director)
            throw InvalidOperationException("Director not set");
        WrappedMultiDimArrayMemoryParams p;
        p.memorypos = memorypos;
        boost::shared_ptr<RRMultiDimArrayUntyped> buffer2 = boost::make_shared<RRMultiDimArrayUntyped>();
        buffer2->Dims = buffer->Dims;
        buffer2->Array = buffer->Array;

        p.buffer = buffer2;
        p.bufferpos = bufferpos;
        p.count = count;

        DIRECTOR_CALL(WrappedMultiDimArrayMemoryDirector, RR_Director2->Write(&p))
    }
};

class WrappedPodArrayMemoryDirector
{
  public:
    WrappedPodArrayMemoryDirector() { objectheapid = 0; }

    virtual ~WrappedPodArrayMemoryDirector() {}
    virtual uint64_t Length() = 0;
    virtual RR_INTRUSIVE_PTR<MessageElementNestedElementList> Read(uint64_t memorypos, uint64_t bufferpos,
                                                                   uint64_t count) = 0;
    virtual void Write(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementNestedElementList> buffer, uint64_t bufferpos,
                       uint64_t count) = 0;
    int32_t objectheapid;
};

class WrappedPodArrayMemory : public ArrayMemoryBase
{
  public:
    WrappedPodArrayMemory(WrappedPodArrayMemoryDirector* RR_Director);

    RR_OVIRTUAL uint64_t Length() RR_OVERRIDE;
    RR_OVIRTUAL DataTypes ElementTypeID() RR_OVERRIDE;

    RR_INTRUSIVE_PTR<MessageElementNestedElementList> Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count);
    void Write(uint64_t memorypos, const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& buffer, uint64_t bufferpos,
               uint64_t count);

    boost::shared_ptr<WrappedPodArrayMemoryDirector> RR_Director;
    boost::shared_mutex RR_Director_lock;
};

class WrappedPodArrayMemoryServiceSkel : public ArrayMemoryServiceSkelBase
{
  public:
    WrappedPodArrayMemoryServiceSkel(const std::string& membername, const RR_SHARED_PTR<ServiceSkel>& skel,
                                     size_t element_size, MemberDefinition_Direction direction);
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count,
                                                            const RR_SHARED_PTR<ArrayMemoryBase>& mem) RR_OVERRIDE;
    RR_OVIRTUAL void DoWrite(uint64_t memorypos, const RR_INTRUSIVE_PTR<MessageElementData>& buffer, uint64_t bufferpos,
                             uint64_t count, const RR_SHARED_PTR<ArrayMemoryBase>& mem) RR_OVERRIDE;
};

class WrappedPodMultiDimArrayMemoryDirector
{
  public:
    virtual ~WrappedPodMultiDimArrayMemoryDirector() {}
    virtual std::vector<uint64_t> Dimensions() = 0;
    virtual uint64_t DimCount() = 0;
    virtual RR_INTRUSIVE_PTR<MessageElementNestedElementList> Read(const std::vector<uint64_t>& memorypos,
                                                                   const std::vector<uint64_t>& bufferpos,
                                                                   const std::vector<uint64_t>& count) = 0;
    virtual void Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementNestedElementList> buffer,
                       const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;

    int32_t objectheapid;
    WrappedPodMultiDimArrayMemoryDirector() : objectheapid(0) {}
};

class WrappedPodMultiDimArrayMemory : public MultiDimArrayMemoryBase
{
  public:
    WrappedPodMultiDimArrayMemory(WrappedPodMultiDimArrayMemoryDirector* RR_Director);

    RR_OVIRTUAL std::vector<uint64_t> Dimensions() RR_OVERRIDE;
    RR_OVIRTUAL uint64_t DimCount() RR_OVERRIDE;
    RR_OVIRTUAL DataTypes ElementTypeID() RR_OVERRIDE;

    RR_INTRUSIVE_PTR<MessageElementNestedElementList> Read(const std::vector<uint64_t>& memorypos,
                                                           const std::vector<uint64_t>& bufferpos,
                                                           const std::vector<uint64_t>& count);
    void Write(const std::vector<uint64_t>& memorypos, const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& buffer,
               const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);

    boost::shared_ptr<WrappedPodMultiDimArrayMemoryDirector> RR_Director;
    boost::shared_mutex RR_Director_lock;
};

class WrappedPodMultiDimArrayMemoryServiceSkel : public MultiDimArrayMemoryServiceSkelBase
{
  public:
    WrappedPodMultiDimArrayMemoryServiceSkel(const std::string& membername, const RR_SHARED_PTR<ServiceSkel>& skel,
                                             size_t element_size, MemberDefinition_Direction direction);
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> DoRead(
        const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos,
        const std::vector<uint64_t>& count, uint32_t elem_count,
        const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem) RR_OVERRIDE;
    RR_OVIRTUAL void DoWrite(const std::vector<uint64_t>& memorypos, const RR_INTRUSIVE_PTR<MessageElementData>& buffer,
                             const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count,
                             uint32_t elem_count, const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem) RR_OVERRIDE;
};

class WrappedNamedArrayMemory : public ArrayMemoryBase
{
  public:
    WrappedNamedArrayMemory(WrappedNamedArrayMemoryDirector* RR_Director);

    RR_OVIRTUAL uint64_t Length() RR_OVERRIDE;
    RR_OVIRTUAL DataTypes ElementTypeID() RR_OVERRIDE;

    RR_INTRUSIVE_PTR<MessageElementNestedElementList> Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count);
    void Write(uint64_t memorypos, const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& buffer, uint64_t bufferpos,
               uint64_t count);

    boost::shared_ptr<WrappedNamedArrayMemoryDirector> RR_Director;
    boost::shared_mutex RR_Director_lock;
};

class WrappedNamedArrayMemoryDirector
{
  public:
    WrappedNamedArrayMemoryDirector() { objectheapid = 0; }

    virtual ~WrappedNamedArrayMemoryDirector() {}
    virtual uint64_t Length() = 0;
    virtual RR_INTRUSIVE_PTR<MessageElementNestedElementList> Read(uint64_t memorypos, uint64_t bufferpos,
                                                                   uint64_t count) = 0;
    virtual void Write(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementNestedElementList> buffer, uint64_t bufferpos,
                       uint64_t count) = 0;
    int32_t objectheapid;
};

class WrappedNamedArrayMemoryServiceSkel : public ArrayMemoryServiceSkelBase
{
  public:
    WrappedNamedArrayMemoryServiceSkel(const std::string& membername, const RR_SHARED_PTR<ServiceSkel>& skel,
                                       size_t element_size, MemberDefinition_Direction direction);
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count,
                                                            const RR_SHARED_PTR<ArrayMemoryBase>& mem) RR_OVERRIDE;
    RR_OVIRTUAL void DoWrite(uint64_t memorypos, const RR_INTRUSIVE_PTR<MessageElementData>& buffer, uint64_t bufferpos,
                             uint64_t count, const RR_SHARED_PTR<ArrayMemoryBase>& mem) RR_OVERRIDE;
};

class WrappedNamedMultiDimArrayMemoryDirector
{
  public:
    virtual ~WrappedNamedMultiDimArrayMemoryDirector() {}
    virtual std::vector<uint64_t> Dimensions() = 0;
    virtual uint64_t DimCount() = 0;
    virtual RR_INTRUSIVE_PTR<MessageElementNestedElementList> Read(const std::vector<uint64_t>& memorypos,
                                                                   const std::vector<uint64_t>& bufferpos,
                                                                   const std::vector<uint64_t>& count) = 0;
    virtual void Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementNestedElementList> buffer,
                       const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;

    int32_t objectheapid;
    WrappedNamedMultiDimArrayMemoryDirector() : objectheapid(0) {}
};

class WrappedNamedMultiDimArrayMemory : public MultiDimArrayMemoryBase
{
  public:
    WrappedNamedMultiDimArrayMemory(WrappedNamedMultiDimArrayMemoryDirector* RR_Director);

    RR_OVIRTUAL std::vector<uint64_t> Dimensions() RR_OVERRIDE;
    RR_OVIRTUAL uint64_t DimCount() RR_OVERRIDE;
    RR_OVIRTUAL DataTypes ElementTypeID() RR_OVERRIDE;

    RR_INTRUSIVE_PTR<MessageElementNestedElementList> Read(const std::vector<uint64_t>& memorypos,
                                                           const std::vector<uint64_t>& bufferpos,
                                                           const std::vector<uint64_t>& count);
    void Write(const std::vector<uint64_t>& memorypos, const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& buffer,
               const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);

    boost::shared_ptr<WrappedNamedMultiDimArrayMemoryDirector> RR_Director;
    boost::shared_mutex RR_Director_lock;
};

class WrappedNamedMultiDimArrayMemoryServiceSkel : public MultiDimArrayMemoryServiceSkelBase
{
  public:
    WrappedNamedMultiDimArrayMemoryServiceSkel(const std::string& membername, const RR_SHARED_PTR<ServiceSkel>& skel,
                                               size_t element_size, MemberDefinition_Direction direction);
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> DoRead(
        const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos,
        const std::vector<uint64_t>& count, uint32_t elem_count,
        const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem) RR_OVERRIDE;
    RR_OVIRTUAL void DoWrite(const std::vector<uint64_t>& memorypos, const RR_INTRUSIVE_PTR<MessageElementData>& buffer,
                             const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count,
                             uint32_t elem_count, const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem) RR_OVERRIDE;
};

void AsyncStubReturn_handler(const RR_SHARED_PTR<RRObject>& obj, const RR_SHARED_PTR<RobotRaconteurException>& err,
                             const RR_SHARED_PTR<AsyncStubReturnDirector>& handler);

void AsyncVoidNoErrReturn_handler(const RR_SHARED_PTR<AsyncVoidNoErrReturnDirector>& handler);

void AsyncStringReturn_handler(const RR_SHARED_PTR<std::string>& str, const RR_SHARED_PTR<RobotRaconteurException>& err,
                               const RR_SHARED_PTR<AsyncStringReturnDirector>& handler);

void WrappedExceptionHandler(const std::exception* err, const RR_SHARED_PTR<AsyncVoidReturnDirector>& handler);

void TimerHandlerFunc(const TimerEvent& ev, const RR_SHARED_PTR<AsyncTimerEventReturnDirector>& d);

class WrappedUserAuthenticatorDirector
{
  public:
    virtual boost::shared_ptr<AuthenticatedUser> AuthenticateUser(const std::string& username,
                                                                  RR_INTRUSIVE_PTR<MessageElement> credentials,
                                                                  const boost::shared_ptr<ServerContext>& context) = 0;

    virtual ~WrappedUserAuthenticatorDirector() {}
};

class WrappedUserAuthenticator : public UserAuthenticator
{

    boost::shared_ptr<WrappedUserAuthenticatorDirector> RR_Director;

  public:
    void SetRRDirector(WrappedUserAuthenticatorDirector* director, int id);

    RR_OVIRTUAL RR_SHARED_PTR<AuthenticatedUser> AuthenticateUser(
        boost::string_ref username, const std::map<std::string, RR_INTRUSIVE_PTR<RRValue> >& credentials,
        const RR_SHARED_PTR<ServerContext>& context, const RR_SHARED_PTR<ITransportConnection>& tc) RR_OVERRIDE;
};

// Subscriptions

class WrappedServiceSubscriptionFilterPredicateDirector
{
  public:
    virtual bool Predicate(const ServiceInfo2Wrapped& info) = 0;

    bool CallPredicate(const ServiceInfo2& info);

    virtual ~WrappedServiceSubscriptionFilterPredicateDirector() {}
};

class WrappedServiceSubscriptionFilterNode
{
  public:
    ::RobotRaconteur::NodeID NodeID;
    std::string NodeName;
    std::string Username;
    RR_INTRUSIVE_PTR<MessageElementData> Credentials;
};

class WrappedServiceSubscriptionFilter
{
  public:
    std::vector<RR_SHARED_PTR<WrappedServiceSubscriptionFilterNode> > Nodes;
    std::vector<std::string> ServiceNames;
    std::vector<std::string> TransportSchemes;
    RR_SHARED_PTR<WrappedServiceSubscriptionFilterPredicateDirector> Predicate;
    void SetRRPredicateDirector(WrappedServiceSubscriptionFilterPredicateDirector* director, int32_t id);
    uint32_t MaxConnections;

    WrappedServiceSubscriptionFilter() : MaxConnections(0) {}
};

class WrappedServiceInfo2Subscription;

class WrappedServiceInfo2SubscriptionDirector
{
  public:
    virtual void ServiceDetected(const boost::shared_ptr<WrappedServiceInfo2Subscription>& subscription,
                                 const ServiceSubscriptionClientID& id, const ServiceInfo2Wrapped& service) = 0;
    virtual void ServiceLost(const boost::shared_ptr<WrappedServiceInfo2Subscription>& subscription,
                             const ServiceSubscriptionClientID& id, const ServiceInfo2Wrapped& service) = 0;

    virtual ~WrappedServiceInfo2SubscriptionDirector() {}
};

class WrappedServiceInfo2Subscription : public RR_ENABLE_SHARED_FROM_THIS<WrappedServiceInfo2Subscription>
{
  public:
    WrappedServiceInfo2Subscription(const RR_SHARED_PTR<ServiceInfo2Subscription>& subscription);

    std::map<ServiceSubscriptionClientID, ServiceInfo2Wrapped> GetDetectedServiceInfo2();

    void Close();

    void SetRRDirector(WrappedServiceInfo2SubscriptionDirector* director, int32_t id);

    boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> GetNode();

  protected:
    RR_SHARED_PTR<ServiceInfo2Subscription> subscription;
    RR_SHARED_PTR<WrappedServiceInfo2SubscriptionDirector> RR_Director;
    boost::shared_mutex RR_Director_lock;
    boost::initialized<bool> events_connected;

    static void ServiceDetected(RR_WEAK_PTR<WrappedServiceInfo2Subscription> this_,
                                const RR_SHARED_PTR<ServiceInfo2Subscription>& subscription,
                                const ServiceSubscriptionClientID& id, const ServiceInfo2& info);
    static void ServiceLost(RR_WEAK_PTR<WrappedServiceInfo2Subscription> this_,
                            const RR_SHARED_PTR<ServiceInfo2Subscription>& subscription,
                            const ServiceSubscriptionClientID& id, const ServiceInfo2& info);
    void ServiceDetected1(const RR_SHARED_PTR<ServiceInfo2Subscription>& subscription,
                          const ServiceSubscriptionClientID& id, const ServiceInfo2& info);
    void ServiceLost1(const RR_SHARED_PTR<ServiceInfo2Subscription>& subscription,
                      const ServiceSubscriptionClientID& id, const ServiceInfo2& info);
};

class WrappedService_typed_packet
{
  public:
    RR_INTRUSIVE_PTR<MessageElement> packet;
    RR_SHARED_PTR<TypeDefinition> type;
    RR_SHARED_PTR<WrappedServiceStub> stub;
    RR_SHARED_PTR<ClientContext> context;
    uint32_t client;

    WrappedService_typed_packet() : client(0) {}
};

class WrappedServiceSubscription;

class WrappedServiceSubscriptionDirector
{
  public:
    virtual void ClientConnected(const boost::shared_ptr<WrappedServiceSubscription>& subscription,
                                 const ServiceSubscriptionClientID& id,
                                 const boost::shared_ptr<WrappedServiceStub>& client) = 0;
    virtual void ClientDisconnected(const boost::shared_ptr<WrappedServiceSubscription>& subscription,
                                    const ServiceSubscriptionClientID& id,
                                    const boost::shared_ptr<WrappedServiceStub>& client) = 0;
    virtual void ClientConnectFailed(const boost::shared_ptr<RobotRaconteur::WrappedServiceSubscription>& subscription,
                                     const ServiceSubscriptionClientID& id, const std::vector<std::string>& url,
                                     HandlerErrorInfo& error) = 0;

    virtual ~WrappedServiceSubscriptionDirector() {}
};

class WrappedWireSubscription;
class WrappedPipeSubscription;

class WrappedServiceSubscription_TryDefaultClientRes
{
  public:
    bool res;
    RR_SHARED_PTR<WrappedServiceStub> client;

    WrappedServiceSubscription_TryDefaultClientRes() : res(false) {}
};

class WrappedServiceSubscription : public RR_ENABLE_SHARED_FROM_THIS<WrappedServiceSubscription>
{
  public:
    WrappedServiceSubscription(const RR_SHARED_PTR<ServiceSubscription>& subscription);

    std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<WrappedServiceStub> > GetConnectedClients();

    void Close();

    void ClaimClient(const RR_SHARED_PTR<WrappedServiceStub>& client);
    void ReleaseClient(const RR_SHARED_PTR<WrappedServiceStub>& client);

    uint32_t GetConnectRetryDelay();
    void SetConnectRetryDelay(uint32_t delay_milliseconds);

    RR_SHARED_PTR<WrappedWireSubscription> SubscribeWire(const std::string& membername, const std::string& servicepath);

    RR_SHARED_PTR<WrappedPipeSubscription> SubscribePipe(
        const std::string& membername, const std::string& servicepath,
        uint32_t max_recv_packets = std::numeric_limits<uint32_t>::max());

    RR_SHARED_PTR<WrappedServiceStub> GetDefaultClient();

    WrappedServiceSubscription_TryDefaultClientRes TryGetDefaultClient();

    RR_SHARED_PTR<WrappedServiceStub> GetDefaultClientWait(int32_t timeout = -1);

    WrappedServiceSubscription_TryDefaultClientRes TryGetDefaultClientWait(int32_t timeout = -1);

    void AsyncGetDefaultClient(int32_t timeout, AsyncStubReturnDirector* handler, int32_t id);

    void SetRRDirector(WrappedServiceSubscriptionDirector* director, int32_t id);

    boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> GetNode();

    std::vector<std::string> GetServiceURL();

    void UpdateServiceURL(
        const std::vector<std::string>& url, const std::string& username = "",
        const boost::intrusive_ptr<MessageElementData>& credentials = boost::intrusive_ptr<MessageElementData>(),
        const std::string& objecttype = "", bool close_connected = false);
    void UpdateServiceURL(
        const std::string& url, const std::string& username = "",
        const boost::intrusive_ptr<MessageElementData>& credentials = boost::intrusive_ptr<MessageElementData>(),
        const std::string& objecttype = "", bool close_connected = false);

  protected:
    RR_SHARED_PTR<ServiceSubscription> subscription;
    RR_SHARED_PTR<WrappedServiceSubscriptionDirector> RR_Director;
    boost::shared_mutex RR_Director_lock;
    boost::initialized<bool> events_connected;

    static void ClientConnected(RR_WEAK_PTR<WrappedServiceSubscription> this_,
                                const RR_SHARED_PTR<ServiceSubscription>& subscription,
                                const ServiceSubscriptionClientID& id, const RR_SHARED_PTR<RRObject>& client);
    static void ClientDisconnected(RR_WEAK_PTR<WrappedServiceSubscription> this_,
                                   const RR_SHARED_PTR<ServiceSubscription>& subscription,
                                   const ServiceSubscriptionClientID& id, const RR_SHARED_PTR<RRObject>& client);
    static void ClientConnectFailed(RR_WEAK_PTR<WrappedServiceSubscription> this_,
                                    const boost::shared_ptr<ServiceSubscription>& subscription,
                                    const ServiceSubscriptionClientID& id, const std::vector<std::string>& url,
                                    const RR_SHARED_PTR<RobotRaconteurException>& err);
    void ClientConnected1(const RR_SHARED_PTR<ServiceSubscription>& subscription, const ServiceSubscriptionClientID& id,
                          const RR_SHARED_PTR<RRObject>& client);
    void ClientDisconnected1(const RR_SHARED_PTR<ServiceSubscription>& subscription,
                             const ServiceSubscriptionClientID& id, const RR_SHARED_PTR<RRObject>& client);
    void ClientConnectFailed1(const boost::shared_ptr<ServiceSubscription>& subscription,
                              const ServiceSubscriptionClientID& id, const std::vector<std::string>& url,
                              const RR_SHARED_PTR<RobotRaconteurException>& err);
};

class WrappedWireSubscriptionDirector
{
  public:
    virtual void WireValueChanged(const boost::shared_ptr<WrappedWireSubscription>& wire_subscription,
                                  WrappedService_typed_packet& value, const TimeSpec& time) = 0;

    virtual ~WrappedWireSubscriptionDirector() {}
};

class WrappedWireSubscription_send_iterator;

class WrappedWireSubscription : public WireSubscriptionBase
{
  public:
    friend class WrappedWireSubscription_send_iterator;

    WrappedWireSubscription(const RR_SHARED_PTR<ServiceSubscription>& parent, const std::string& membername,
                            const std::string& servicepath);

    WrappedService_typed_packet GetInValue(TimeSpec* time = NULL);
    bool TryGetInValue(WrappedService_typed_packet& val, TimeSpec* time = NULL);

    void SetRRDirector(WrappedWireSubscriptionDirector* director, int32_t id);

  protected:
    RR_OVIRTUAL void fire_WireValueChanged(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& time,
                                           const RR_SHARED_PTR<WireConnectionBase>& connection) RR_OVERRIDE;
    RR_SHARED_PTR<WrappedWireSubscriptionDirector> RR_Director;
    boost::shared_mutex RR_Director_lock;
};

class WrappedWireSubscription_send_iterator
{
  protected:
    detail::WireSubscription_send_iterator iter;
    RR_SHARED_PTR<WrappedWireConnection> current_connection;

  public:
    WrappedWireSubscription_send_iterator(const RR_SHARED_PTR<WrappedWireSubscription>& sub);
    RR_SHARED_PTR<WrappedWireConnection> Next();
    void SetOutValue(const RR_INTRUSIVE_PTR<MessageElement>& value);
    RR_SHARED_PTR<TypeDefinition> GetType();
    RR_SHARED_PTR<WrappedServiceStub> GetStub();
    virtual ~WrappedWireSubscription_send_iterator();
};

class WrappedPipeSubscriptionDirector
{
  public:
    virtual void PipePacketReceived(const boost::shared_ptr<WrappedPipeSubscription>& pipe_subscription) = 0;

    virtual ~WrappedPipeSubscriptionDirector() {}
};

class WrappedPipeSubscription_send_iterator;

class WrappedPipeSubscription : public PipeSubscriptionBase
{
  public:
    friend class WrappedPipeSubscription_send_iterator;

    WrappedPipeSubscription(const RR_SHARED_PTR<ServiceSubscription>& parent, const std::string& membername,
                            const std::string& servicepath, int32_t max_recv_packets = -1,
                            int32_t max_send_backlog = 5);

    WrappedService_typed_packet ReceivePacket();
    bool TryReceivePacket(WrappedService_typed_packet& packet);
    bool TryReceivePacketWait(WrappedService_typed_packet& packet, int32_t timeout = RR_TIMEOUT_INFINITE,
                              bool peek = false);

    void SetRRDirector(WrappedPipeSubscriptionDirector* director, int32_t id);

  protected:
    RR_OVIRTUAL void fire_PipePacketReceived() RR_OVERRIDE;
    RR_SHARED_PTR<WrappedPipeSubscriptionDirector> RR_Director;
    boost::shared_mutex RR_Director_lock;
};

class WrappedPipeSubscription_send_iterator
{
  protected:
    detail::PipeSubscription_send_iterator iter;
    RR_SHARED_PTR<WrappedPipeEndpoint> current_connection;

  public:
    WrappedPipeSubscription_send_iterator(const RR_SHARED_PTR<WrappedPipeSubscription>& sub);
    RR_SHARED_PTR<WrappedPipeEndpoint> Next();
    void AsyncSendPacket(const RR_INTRUSIVE_PTR<MessageElement>& value);
    RR_SHARED_PTR<TypeDefinition> GetType();
    RR_SHARED_PTR<WrappedServiceStub> GetStub();
    virtual ~WrappedPipeSubscription_send_iterator();
};

std::vector<ServiceSubscriptionClientID> WrappedServiceSubscriptionClientsToVector(
    std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<WrappedServiceStub> >& clients);

std::vector<ServiceSubscriptionClientID> WrappedServiceInfo2SubscriptionServicesToVector(
    std::map<ServiceSubscriptionClientID, ServiceInfo2Wrapped>& infos);

RR_SHARED_PTR<WrappedServiceInfo2Subscription> WrappedSubscribeServiceInfo2(
    const RR_SHARED_PTR<RobotRaconteurNode>& node, const std::vector<std::string>& service_types,
    const RR_SHARED_PTR<WrappedServiceSubscriptionFilter>& filter = RR_SHARED_PTR<WrappedServiceSubscriptionFilter>());

RR_SHARED_PTR<WrappedServiceSubscription> WrappedSubscribeServiceByType(
    const RR_SHARED_PTR<RobotRaconteurNode>& node, const std::vector<std::string>& service_types,
    const RR_SHARED_PTR<WrappedServiceSubscriptionFilter>& filter = RR_SHARED_PTR<WrappedServiceSubscriptionFilter>());

RR_SHARED_PTR<WrappedServiceSubscription> WrappedSubscribeService(
    const RR_SHARED_PTR<RobotRaconteurNode>& node, const std::vector<std::string>& url,
    const std::string& username = "",
    const boost::intrusive_ptr<MessageElementData>& credentials = boost::intrusive_ptr<MessageElementData>(),
    const std::string& objecttype = "");

RR_SHARED_PTR<WrappedServiceSubscription> WrappedSubscribeService(
    const RR_SHARED_PTR<RobotRaconteurNode>& node, const std::string& url, const std::string& username = "",
    const boost::intrusive_ptr<MessageElementData>& credentials = boost::intrusive_ptr<MessageElementData>(),
    const std::string& objecttype = "");

class UserLogRecordHandlerDirector
{
  public:
    virtual void HandleLogRecord(const RRLogRecord& record) = 0;
    virtual ~UserLogRecordHandlerDirector() {}
};

class UserLogRecordHandlerBase : public LogRecordHandler
{
    RR_SHARED_PTR<UserLogRecordHandlerDirector> handler_director;

  public:
    void SetHandler(UserLogRecordHandlerDirector* director, int32_t id);

    RR_OVIRTUAL void HandleLogRecord(const RRLogRecord& record) RR_OVERRIDE;
};

#ifdef RR_PYTHON

class PythonThreadPool : public ThreadPool
{
  public:
    PythonThreadPool(const RR_SHARED_PTR<RobotRaconteurNode>& node) : ThreadPool(node) {}

  protected:
    virtual void thread_function()
    {
        DIRECTOR_CALL2(

            RR_Ensure_GIL py_gil;

            PyRun_SimpleString("import RobotRaconteur.RobotRaconteurPythonUtil as RRU\nRRU.settrace()\n");

            Py_BEGIN_ALLOW_THREADS ThreadPool::thread_function(); Py_END_ALLOW_THREADS)
    }
};

class PythonThreadPoolFactory : public ThreadPoolFactory
{
  public:
    virtual RR_SHARED_PTR<ThreadPool> NewThreadPool(const RR_SHARED_PTR<RobotRaconteurNode>& node)
    {
        return RR_MAKE_SHARED<PythonThreadPool>(node);
    }
};
#endif

} // namespace RobotRaconteur
