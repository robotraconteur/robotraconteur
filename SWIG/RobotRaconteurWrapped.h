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

#include <RobotRaconteur.h>

#ifdef RR_PYTHON
#include <Python.h>
#endif
#pragma once



namespace RobotRaconteur
{
#ifdef RR_PYTHON

	class RR_Release_GIL
	{
	public:
		RR_Release_GIL()
		{
			_save = PyEval_SaveThread();
		}

		~RR_Release_GIL()
		{
			PyEval_RestoreThread(_save);
		}

	protected:
		PyThreadState * _save;
	};

	class RR_Ensure_GIL
	{
	public:
		RR_Ensure_GIL()
		{
			gstate = PyGILState_Ensure();
		}

		~RR_Ensure_GIL()
		{
			PyGILState_Release(gstate);
		}
	private:
		PyGILState_STATE gstate;

	};

#define DIRECTOR_CALL(dirtype,command){ \
	boost::shared_lock<boost::shared_mutex> lock(RR_Director_lock); \
	RR_Ensure_GIL gil; \
	boost::shared_ptr<dirtype> RR_Director2(this->RR_Director); \
	\
	lock.unlock(); \
	if (!RR_Director2) throw InvalidOperationException("Director has been released");\
	{command;} \
}

#define DIRECTOR_CALL2(command) { \
RR_Ensure_GIL gil; \
{command ;}\
}

#define DIRECTOR_CALL3(dirtype,command){ \
	RR_Ensure_GIL gil; \
	boost::shared_lock<boost::shared_mutex> lock(RR_Director_lock); \
	boost::shared_ptr<dirtype> RR_Director2(this->RR_Director); \
	\
	lock.unlock(); \
	if (RR_Director2)\
	{command; } \
}



#define DIRECTOR_DELETE(var) { if(var) {delete var; var=NULL;}}

#else
#define DIRECTOR_CALL(dirtype,command) { \
boost::shared_lock<boost::shared_mutex> lock(RR_Director_lock);\
	boost::shared_ptr<dirtype> RR_Director2(this->RR_Director); \
	\
	lock.unlock(); \
	if (!RR_Director2) throw InvalidOperationException("Director has been released");\
	RRDirectorExceptionHelper::Reset(); \
	{command;} \
	if (RobotRaconteur::RRDirectorExceptionHelper::IsErrorPending()) RobotRaconteurExceptionUtil::ThrowMessageEntryException(RobotRaconteur::RRDirectorExceptionHelper::GetError()); }

#define DIRECTOR_CALL2(command) \
	RRDirectorExceptionHelper::Reset(); \
	{command;} \
	if (RobotRaconteur::RRDirectorExceptionHelper::IsErrorPending()) RobotRaconteurExceptionUtil::ThrowMessageEntryException(RobotRaconteur::RRDirectorExceptionHelper::GetError());

#define DIRECTOR_CALL3(dirtype,command) { \
boost::shared_lock<boost::shared_mutex> lock(RR_Director_lock);\
	boost::shared_ptr<dirtype> RR_Director2(this->RR_Director); \
	\
	lock.unlock(); \
	if (!RR_Director2) return;\
	RRDirectorExceptionHelper::Reset(); \
	{command;} \
	if (RobotRaconteur::RRDirectorExceptionHelper::IsErrorPending()) RobotRaconteurExceptionUtil::ThrowMessageEntryException(RobotRaconteur::RRDirectorExceptionHelper::GetError()); }

#define DIRECTOR_DELETE(var)
#endif
	
	

	

	class RRDirectorExceptionHelper
	{
	protected:
		static boost::thread_specific_ptr<RR_INTRUSIVE_PTR<MessageEntry> > last_err;

	public:
		static void Reset();
		static void SetError(RR_INTRUSIVE_PTR<MessageEntry> err);
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

			support=s;
		}

		virtual void DeleteObject_i(int32_t id)=0;

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

	template<typename T>
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
		WrappedServiceFactory(boost::shared_ptr<RobotRaconteur::ServiceDefinition> def);
		virtual ~WrappedServiceFactory() {}

		virtual std::string GetServiceName();
		virtual std::string DefString();
		virtual RR_SHARED_PTR<ServiceDefinition> ServiceDef();
		virtual RR_SHARED_PTR<RobotRaconteur::StructureStub> FindStructureStub(const std::string& s);
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementStructure> PackStructure(RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure> structin);
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> UnpackStructure(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementStructure> mstructin);
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodArray> PackPodArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray> structure);
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray> UnpackPodArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodArray> structure);
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodMultiDimArray> PackPodMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray> structure);
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray> UnpackPodMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodMultiDimArray> structure);
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedArray> PackNamedArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray> structure);
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray> UnpackNamedArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedArray> structure);
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedMultiDimArray> PackNamedMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray> structure);
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray> UnpackNamedMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedMultiDimArray> structure);
		virtual RR_SHARED_PTR<RobotRaconteur::ServiceStub> CreateStub(const std::string& objecttype, const std::string& path, RR_SHARED_PTR<RobotRaconteur::ClientContext> context);
		virtual RR_SHARED_PTR<RobotRaconteur::ServiceSkel> CreateSkel(const std::string& objecttype, const std::string& path, RR_SHARED_PTR<RobotRaconteur::RRObject> obj, RR_SHARED_PTR<RobotRaconteur::ServerContext> context);
		virtual void DownCastAndThrowException(RobotRaconteurException& exp)
		{
			throw exp;
		}

		virtual RR_SHARED_PTR<RobotRaconteurException> DownCastException(RR_SHARED_PTR<RobotRaconteurException> exp)
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
		virtual ~WrappedDynamicServiceFactory() {}
		virtual RR_SHARED_PTR<ServiceFactory> CreateServiceFactory(const std::string& def);
		virtual std::vector<RR_SHARED_PTR<ServiceFactory> > CreateServiceFactories(const std::vector<std::string>& def);
	};

	
	/*class AsyncHandlerDirector
	{
	public:
		virtual ~AsyncHandlerDirector() {}
		virtual void handler(void* m, uint32_t error_code, std::string errorname, std::string errormessage) {};

	};*/

	class AsyncRequestDirector
	{
	public:
		virtual ~AsyncRequestDirector() {}
		virtual void handler(RR_INTRUSIVE_PTR<MessageElement> ret, uint32_t error_code, const std::string& errorname, const std::string& errormessage) {};

	};

	class AsyncVoidReturnDirector
	{
	public:
		virtual ~AsyncVoidReturnDirector() {}
		virtual void handler(uint32_t error_code, const std::string& errorname, const std::string& errormessage) {};
	};

	class AsyncVoidNoErrReturnDirector
	{
	public:
		virtual ~AsyncVoidNoErrReturnDirector() {}
		virtual void handler() {};
	};

	class AsyncStringReturnDirector
	{
	public:
		virtual ~AsyncStringReturnDirector() {}
		virtual void handler(const std::string& ret, uint32_t error_code, const std::string& errorname, const std::string& errormessage) {};
	};

	class AsyncUInt32ReturnDirector
	{
	public:
		virtual ~AsyncUInt32ReturnDirector() {}
		virtual void handler(uint32_t ret, uint32_t error_code, const std::string& errorname, const std::string& errormessage) {};
	};

	class AsyncTimerEventReturnDirector
	{
	public:
		virtual ~AsyncTimerEventReturnDirector() {}
		virtual void handler(const TimerEvent& ret, uint32_t error_code, const std::string& errorname, const std::string& errormessage) {};
	};

	
	class WrappedServiceStubDirector
	{
	public:
		virtual ~WrappedServiceStubDirector() {}
		virtual void DispatchEvent(const std::string& EventName, const std::vector<RR_INTRUSIVE_PTR<MessageElement> > args) {}
		virtual RR_INTRUSIVE_PTR<MessageElement> CallbackCall(const std::string& CallbackName, const std::vector<RR_INTRUSIVE_PTR<MessageElement> > args) { return RR_INTRUSIVE_PTR<MessageElement>(); }

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
		virtual void handler(boost::shared_ptr<WrappedServiceStub> stub, uint32_t error_code, const std::string& errorname, const std::string& errormessage) {};
	};

	class WrappedServiceStub : public virtual RobotRaconteur::ServiceStub
	{
	public:
		WrappedServiceStub(const std::string& path, RR_SHARED_PTR<ServiceEntryDefinition> type, RR_SHARED_PTR<RobotRaconteur::ClientContext> c);
		virtual ~WrappedServiceStub();

		virtual RR_INTRUSIVE_PTR<MessageElement> PropertyGet(const std::string& PropertyName);
		virtual void PropertySet(const std::string& PropertyName, RR_INTRUSIVE_PTR<MessageElement> value);
		virtual RR_INTRUSIVE_PTR<MessageElement> FunctionCall(const std::string& FunctionName, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& args);
		virtual RR_SHARED_PTR<WrappedGeneratorClient> GeneratorFunctionCall(const std::string& FunctionName, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& args);

		virtual void async_PropertyGet(const std::string& PropertyName, int32_t timeout, AsyncRequestDirector* handler,int32_t id);
		virtual void async_PropertySet(const std::string& PropertyName, RR_INTRUSIVE_PTR<MessageElement> value, int32_t timeout, AsyncRequestDirector* handler,int32_t id);
		virtual void async_FunctionCall(const std::string& FunctionName, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& args, int32_t timeout, AsyncRequestDirector* handler, int32_t id);
		virtual void async_GeneratorFunctionCall(const std::string& FunctionName, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& args, int32_t timeout, AsyncGeneratorClientReturnDirector* handler, int32_t id);
		virtual void async_FindObjRef(const std::string& path, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id);
		virtual void async_FindObjRef(const std::string& path, const std::string& ind, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id);
		virtual void async_FindObjRefTyped(const std::string& path, const std::string& type, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id);
		virtual void async_FindObjRefTyped(const std::string& path, const std::string& ind, const std::string& type, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id);
		
		
	protected:
		virtual void async_PropertyGet_handler( RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncRequestDirector> handler);
		virtual void async_PropertySet_handler( RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncRequestDirector> handler);
		virtual void async_FunctionCall_handler( RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncRequestDirector> handler);
		virtual void async_GeneratorFunctionCall_handler(const std::string& FunctionName, RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncGeneratorClientReturnDirector> handler);
		virtual void async_FindObjRef_handler( RR_SHARED_PTR<RRObject> stub, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncStubReturnDirector> handler);
	public:

		virtual void DispatchEvent(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m);
		virtual void DispatchPipeMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m);
		virtual void DispatchWireMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m);
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallbackCall(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m);
		virtual RR_SHARED_PTR<RobotRaconteur::WrappedPipeClient> GetPipe(const std::string& membername);
		virtual RR_SHARED_PTR<RobotRaconteur::WrappedWireClient> GetWire(const std::string& membername);
		virtual RR_SHARED_PTR<PipeClientBase> RRGetPipeClient(const std::string& membername);
		virtual RR_SHARED_PTR<WireClientBase> RRGetWireClient(const std::string& membername);
		virtual RR_SHARED_PTR<RobotRaconteur::ArrayMemoryBase> GetArrayMemory(const std::string& membername);
		virtual RR_SHARED_PTR<WrappedPodArrayMemoryClient> GetPodArrayMemory(const std::string& membername);
		virtual RR_SHARED_PTR<WrappedNamedArrayMemoryClient> GetNamedArrayMemory(const std::string& membername);
		virtual RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemoryBase> GetMultiDimArrayMemory(const std::string& membername);
		virtual RR_SHARED_PTR<WrappedPodMultiDimArrayMemoryClient> GetPodMultiDimArrayMemory(const std::string& membername);
		virtual RR_SHARED_PTR<WrappedNamedMultiDimArrayMemoryClient> GetNamedMultiDimArrayMemory(const std::string& membername);
		virtual void RRClose();
		virtual void RRInitStub();
	public:
		RR_SHARED_PTR<ServiceEntryDefinition> RR_objecttype;
		
	public:
		virtual std::string RRType();
	protected:
		boost::shared_ptr<WrappedServiceStubDirector> RR_Director;
		boost::shared_mutex RR_Director_lock;

		int objectheapid;
	public:

		//WrappedServiceStubDirector* GetRRDirector();
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

		//int32_t objectheapid;

#ifdef RR_PYTHON
	protected:
		boost::mutex pystub_lock;
		PyObject* pystub;

	public:

		PyObject* GetPyStub()
		{
			boost::mutex::scoped_lock lock(pystub_lock);
            if (pystub!=NULL)
            {
                Py_XINCREF(pystub);
                return pystub;
            }
            else
            {
                PyObject* n=Py_None;
                Py_XINCREF(n);
                return n;
            }
		}

		void SetPyStub(PyObject* stub)
		{
			boost::mutex::scoped_lock lock(pystub_lock);
            if (pystub!=NULL)
            {
			Py_XDECREF(pystub);
            }
            if (stub==Py_None)
            {
                pystub=NULL;
            }
            else
            {
                Py_XINCREF(stub);
                pystub=stub;
            }
		}

#endif

	};

	class WrappedStubCallbackDirector
	{
	public:
		virtual ~WrappedStubCallbackDirector() {}
		virtual void Callback(ClientServiceListenerEventType) {}
	};

	class WrappedPipeBroadcaster;
	class WrappedPipeEndpoint;
	class WrappedPipeEndpointDirector
	{
	public:
		virtual ~WrappedPipeEndpointDirector() {}
		virtual void PipeEndpointClosedCallback() {};
		virtual void PacketReceivedEvent() {};
		virtual void PacketAckReceivedEvent(uint32_t packetnum) {};

	};

	class WrappedPipeEndpoint : public PipeEndpointBase
	{

	public:
		
		friend class WrappedPipeBroadcaster;

		virtual ~WrappedPipeEndpoint();

		virtual uint32_t SendPacket(RR_INTRUSIVE_PTR<MessageElement> packet);
		virtual void AsyncSendPacket(RR_INTRUSIVE_PTR<MessageElement> packet, AsyncUInt32ReturnDirector* handler, int32_t id);
		virtual RR_INTRUSIVE_PTR<MessageElement> ReceivePacket();
		virtual RR_INTRUSIVE_PTR<MessageElement> PeekNextPacket();
		virtual RR_INTRUSIVE_PTR<MessageElement> ReceivePacketWait(int32_t timeout = RR_TIMEOUT_INFINITE);
		virtual RR_INTRUSIVE_PTR<MessageElement> PeekNextPacketWait(int32_t timeout = RR_TIMEOUT_INFINITE);
		virtual bool TryReceivePacketWait(RR_INTRUSIVE_PTR<MessageElement>& packet, int32_t timeout = RR_TIMEOUT_INFINITE, bool peek = false);
		
		WrappedPipeEndpoint(RR_SHARED_PTR<PipeBase> parent, int32_t index, uint32_t endpoint, RR_SHARED_PTR<TypeDefinition> Type, bool unreliable, MemberDefinition_Direction direction, bool message3);
		RR_SHARED_PTR<TypeDefinition> Type;

	protected:
		virtual void fire_PipeEndpointClosedCallback();
		virtual void fire_PacketReceivedEvent();
		virtual void fire_PacketAckReceivedEvent(uint32_t packetnum);
	

	
		boost::shared_ptr<WrappedPipeEndpointDirector> RR_Director;
		boost::shared_mutex RR_Director_lock;


		static void send_handler(uint32_t packetnumber, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<uint32_t>,RR_SHARED_PTR<RobotRaconteurException>)> handler)
		{
			handler(RR_MAKE_SHARED<uint32_t>(packetnumber),err);
		}

		void AsyncSendPacket_handler(uint32_t id, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncUInt32ReturnDirector> handler);


	public:

		//WrappedPipeEndpointDirector* GetRRDirector();
		void SetRRDirector(WrappedPipeEndpointDirector* director, int32_t id);
		RR_SHARED_PTR<WrappedServiceStub> GetStub();

		//int32_t objectheapid;

		virtual void Close();
        using PipeEndpointBase::AsyncClose;
		void AsyncClose(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id);
	protected:
		void AsyncClose_handler(RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncVoidReturnDirector> handler);
		
		void SetPipeBroadcaster(RR_SHARED_PTR<WrappedPipeBroadcaster> broadcaster, RR_SHARED_PTR<detail::PipeBroadcasterBase_connected_endpoint> cep);

		RR_WEAK_PTR<WrappedPipeBroadcaster> broadcaster;
		RR_WEAK_PTR<detail::PipeBroadcasterBase_connected_endpoint> broadcaster_cep;

	};

	class AsyncPipeEndpointReturnDirector
	{
	public:
		virtual ~AsyncPipeEndpointReturnDirector() {}
		virtual void handler(boost::shared_ptr<WrappedPipeEndpoint> ep, uint32_t error_code, const std::string& errorname, const std::string& errormessage) {};
	};

	class WrappedPipeClient : public virtual PipeClientBase
	{
	public:

		virtual ~WrappedPipeClient() {}
		
		//virtual boost::function<void(RR_SHARED_PTR<WrappedPipeEndpoint>)> GetPipeConnectCallback();
		//virtual void SetPipeConnectCallback(boost::function<void(RR_SHARED_PTR<WrappedPipeEndpoint>)> function);
		virtual RR_SHARED_PTR<WrappedPipeEndpoint> Connect(int32_t index);
		void AsyncConnect(int32_t index, int32_t timeout, AsyncPipeEndpointReturnDirector* handler, int32_t id);
		WrappedPipeClient(const std::string& name, RR_SHARED_PTR<ServiceStub> stub, RR_SHARED_PTR<TypeDefinition> Type, bool unreliable, MemberDefinition_Direction direction);

		
		RR_SHARED_PTR<TypeDefinition> Type;

	protected:
		virtual RR_SHARED_PTR<PipeEndpointBase> CreateNewPipeEndpoint(int32_t index, bool unreliable, MemberDefinition_Direction direction, bool message3);
		void AsyncConnect_handler(RR_SHARED_PTR<PipeEndpointBase> ep, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncPipeEndpointReturnDirector> handler);
	};

	class WrappedPipeServerConnectDirector
	{
	public:
		virtual ~WrappedPipeServerConnectDirector() {}

		virtual void PipeConnectCallback(boost::shared_ptr<WrappedPipeEndpoint> e) {};

		virtual void PipeConnectCallbackFire(RR_SHARED_PTR<PipeEndpointBase> e);

	};

	class WrappedPipeServer : public virtual PipeServerBase
	{
	public:

		WrappedPipeServer(const std::string& name, RR_SHARED_PTR<ServiceSkel> skel, RR_SHARED_PTR<TypeDefinition> Type, bool unreliable, MemberDefinition_Direction direction);
		virtual ~WrappedPipeServer();

		RR_SHARED_PTR<TypeDefinition> Type;
	protected:

		boost::function<void(RR_SHARED_PTR<PipeEndpointBase >)> callback;
				
		boost::mutex callback_lock;
	public:
		//WrappedPipeServerDirector* GetRRDirector();
		void SetWrappedPipeConnectCallback(WrappedPipeServerConnectDirector* director, int32_t id);

		//int32_t objectheapid;

		virtual boost::function<void(RR_SHARED_PTR<PipeEndpointBase >)> GetPipeConnectCallback();
		
		virtual void SetPipeConnectCallback(boost::function<void(RR_SHARED_PTR<PipeEndpointBase >)> function);
		
		virtual void Shutdown();

	protected:
		virtual RR_SHARED_PTR<PipeEndpointBase> CreateNewPipeEndpoint(int32_t index, uint32_t endpoint, bool unreliable, MemberDefinition_Direction direction, bool message3);

		virtual void fire_PipeConnectCallback(RR_SHARED_PTR<PipeEndpointBase> e);

	};

	class WrappedPipeBroadcasterPredicateDirector
	{
	public:

		virtual bool Predicate(uint32_t client_endpoint, int32_t index) = 0;
		virtual bool CallPredicate(uint32_t client_endpoint, int32_t index);
		virtual ~WrappedPipeBroadcasterPredicateDirector() {}
	};

	class WrappedPipeBroadcaster : public PipeBroadcasterBase
	{
	public:

		friend class WrappedPipeEndpoint;

		void Init(RR_SHARED_PTR<WrappedPipeServer> pipe, int32_t maximum_backlog = -1);

		void SendPacket(RR_INTRUSIVE_PTR<MessageElement> packet);

		void AsyncSendPacket(RR_INTRUSIVE_PTR<MessageElement> packet, AsyncVoidNoErrReturnDirector* handler, int32_t id);

		void AsyncSendPacket_handler(RR_SHARED_PTR<AsyncVoidNoErrReturnDirector> handler);

		void SetPredicateDirector(WrappedPipeBroadcasterPredicateDirector* f, int32_t id);

	protected:

		virtual void AttachPipeServerEvents(RR_SHARED_PTR<PipeServerBase> p);

		virtual void AttachPipeEndpointEvents(RR_SHARED_PTR<PipeEndpointBase> ep, RR_SHARED_PTR<detail::PipeBroadcasterBase_connected_endpoint> cep);

	};

	class WrappedWireConnection;
	class WrappedWireBroadcaster;
	class WrappedWireUnicastReceiver;
	class WrappedWireConnectionDirector
	{
	public:
		virtual ~WrappedWireConnectionDirector() {}
		virtual void WireValueChanged(RR_INTRUSIVE_PTR<MessageElement> value, const TimeSpec& time) {}
		virtual void WireConnectionClosedCallback() {}
	};

	class WrappedWireConnection : public virtual WireConnectionBase
	{
	public:

		friend class WrappedWireBroadcaster;
		friend class WrappedWireUnicastReceiver;

		virtual ~WrappedWireConnection();
		virtual RR_INTRUSIVE_PTR<MessageElement> GetInValue();
		virtual RR_INTRUSIVE_PTR<MessageElement> GetOutValue();
		virtual void SetOutValue(RR_INTRUSIVE_PTR<MessageElement> value);

		bool TryGetInValue(RR_INTRUSIVE_PTR<MessageElement>& value, TimeSpec& ts);
		bool TryGetOutValue(RR_INTRUSIVE_PTR<MessageElement>& value, TimeSpec& ts);

		WrappedWireConnection(RR_SHARED_PTR<WireBase> parent, uint32_t endpoint, RR_SHARED_PTR<TypeDefinition> Type, MemberDefinition_Direction direction, bool message3) ;

		virtual void fire_WireValueChanged(RR_INTRUSIVE_PTR<RRValue> value, TimeSpec time);
		virtual void fire_WireClosedCallback();
						
		RR_SHARED_PTR<TypeDefinition> Type;
		
	protected:

		boost::shared_ptr<WrappedWireConnectionDirector> RR_Director;
		boost::shared_mutex RR_Director_lock;

	public:
		//WrappedWireConnectionDirector* GetRRDirector();
		void SetRRDirector(WrappedWireConnectionDirector* director, int32_t id);
		RR_SHARED_PTR<WrappedServiceStub> GetStub();

		//int32_t objectheapid;
		virtual void Close();
        using WireConnectionBase::AsyncClose;
		void AsyncClose(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id);
	protected:
		void AsyncClose_handler(RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncVoidReturnDirector> handler);
		
		void SetWireBroadcaster(RR_SHARED_PTR<WrappedWireBroadcaster> broadcaster, RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> cep);
		void SetWireUnicastReceiver(RR_SHARED_PTR<WrappedWireUnicastReceiver> receiver);

		RR_WEAK_PTR<WrappedWireBroadcaster> broadcaster;
		RR_WEAK_PTR<detail::WireBroadcaster_connected_connection> broadcaster_cep;

		RR_WEAK_PTR<WrappedWireUnicastReceiver> unicast_receiver;

	};

	class AsyncWireConnectionReturnDirector
	{
	public:
		virtual ~AsyncWireConnectionReturnDirector() {}
		virtual void handler(boost::shared_ptr<WrappedWireConnection> ep, uint32_t error_code, const std::string& errorname, const std::string& errormessage) {};
	};

	class AsyncWirePeekReturnDirector
	{
	public:
		virtual ~AsyncWirePeekReturnDirector() {}
		virtual void handler(RR_INTRUSIVE_PTR<MessageElement> value, const TimeSpec& ts, uint32_t error_code, const std::string& errorname, const std::string& errormessage) {};
	};

	class WrappedWireClient : public virtual WireClientBase
	{
	public:
		virtual ~WrappedWireClient() {}

		virtual boost::function<void(RR_SHARED_PTR<WrappedWireConnection>)> GetWireConnectCallback();
		virtual void SetWireConnectCallback(boost::function<void(RR_SHARED_PTR<WrappedWireConnection>)> function);
		virtual RR_SHARED_PTR<WrappedWireConnection> Connect();
		void AsyncConnect(int32_t timeout, AsyncWireConnectionReturnDirector* handler, int32_t id);
		WrappedWireClient(const std::string& name, RR_SHARED_PTR<ServiceStub> stub, RR_SHARED_PTR<TypeDefinition> Type, MemberDefinition_Direction direction);

		RR_INTRUSIVE_PTR<MessageElement> PeekInValue(TimeSpec& ts);
		RR_INTRUSIVE_PTR<MessageElement> PeekOutValue(TimeSpec& ts);
		void PokeOutValue(const RR_INTRUSIVE_PTR<MessageElement>& value);
		
		void AsyncPeekInValue(int32_t timeout, AsyncWirePeekReturnDirector* handler, int32_t id);
		void AsyncPeekOutValue(int32_t timeout, AsyncWirePeekReturnDirector* handler, int32_t id);
		void AsyncPokeOutValue(const RR_INTRUSIVE_PTR<MessageElement>& value, int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id);

		void AsyncPeekValue_handler(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncWirePeekReturnDirector> handler);
		void AsyncPokeValue_handler(RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncVoidReturnDirector> handler);

		RR_SHARED_PTR<TypeDefinition> Type;

	protected:
		virtual RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(MemberDefinition_Direction direction, bool message3);
		void AsyncConnect_handler(RR_SHARED_PTR<WireConnectionBase> ep, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncWireConnectionReturnDirector> handler);
		
	};

	class WrappedWireServerConnectDirector
	{
	public:
		virtual ~WrappedWireServerConnectDirector() {}
		virtual void WireConnectCallback(boost::shared_ptr<WrappedWireConnection> c) {};
		virtual void WireConnectCallbackFire(RR_SHARED_PTR<WireConnectionBase> e);
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
		WrappedWireServer(const std::string& name, RR_SHARED_PTR<ServiceSkel> skel, RR_SHARED_PTR<TypeDefinition> Type, MemberDefinition_Direction direction);
		virtual ~WrappedWireServer();
		RR_SHARED_PTR<TypeDefinition> Type;
		
	protected:
		virtual RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(uint32_t endpoint, MemberDefinition_Direction direction, bool message);
		virtual void fire_WireConnectCallback(RR_SHARED_PTR<WireConnectionBase> e);
		
		boost::function<void(RR_SHARED_PTR<WireConnectionBase >)> callback;

		boost::mutex callback_lock;

		RR_WEAK_PTR<IWrappedWirePeekPokeCallbacks> wrapped_peek_poke_callbacks;

	public:
		//WrappedWireServerDirector* GetRRDirector();
		void SetWrappedWireConnectCallback(WrappedWireServerConnectDirector* director, int32_t id);

		//int32_t objectheapid;
				
		virtual void SetWireConnectCallback(boost::function<void(RR_SHARED_PTR<WireConnectionBase >)> function);

		//For WireBroadcaster and WireUnicastReceiver
		virtual void SetWireWrappedPeekPokeCallbacks(RR_SHARED_PTR<IWrappedWirePeekPokeCallbacks> peek_poke_callbacks);

		virtual void Shutdown();

		virtual void SetPeekInValueCallback(WrappedWireServerPeekValueDirector* director, int32_t id);
		virtual void SetPeekOutValueCallback(WrappedWireServerPeekValueDirector* director, int32_t id);
		virtual void SetPokeOutValueCallback(WrappedWireServerPokeValueDirector* director, int32_t id);
					
	protected:

		virtual RR_INTRUSIVE_PTR<RRValue> do_PeekInValue(const uint32_t& ep);
		virtual RR_INTRUSIVE_PTR<RRValue> do_PeekOutValue(const uint32_t& ep);
		virtual void do_PokeOutValue(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts, const uint32_t& ep);

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
	};

	class WrappedWireBroadcasterPredicateDirector
	{
	public:

		virtual bool Predicate(uint32_t client_endpoint) = 0;
		virtual bool CallPredicate(uint32_t client_endpoint);
		virtual ~WrappedWireBroadcasterPredicateDirector() {}
	};

	class WrappedWireBroadcaster : public WireBroadcasterBase, public IWrappedWirePeekPokeCallbacks
	{
	public:
		
		friend class WrappedWireConnection;

		void Init(RR_SHARED_PTR<WrappedWireServer> wire);

		void SetOutValue(RR_INTRUSIVE_PTR<MessageElement> value);

		void SetPredicateDirector(WrappedWireBroadcasterPredicateDirector* f, int32_t id);

	protected:

		virtual void AttachWireServerEvents(RR_SHARED_PTR<WireServerBase> w);

		virtual void AttachWireConnectionEvents(RR_SHARED_PTR<WireConnectionBase> w, RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> cep);

	public:

		virtual RR_INTRUSIVE_PTR<RRValue> do_PeekInValue(const uint32_t& ep);
		virtual RR_INTRUSIVE_PTR<RRValue> do_PeekOutValue(const uint32_t& ep);
		virtual void do_PokeOutValue(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts, const uint32_t& ep);

	};

	
	namespace detail
	{	
		template<>
		class Wire_traits<WrappedWireServer>
		{
		public:
			typedef WrappedWireConnection wireconnection_type;
			typedef WrappedWireClient wireclient_type;
			typedef WrappedWireServer wireserver_type;
		};

	}

	class WrappedWireUnicastReceiver : public WireUnicastReceiverBase<WrappedWireServer, RR_INTRUSIVE_PTR<MessageElement> >,  public IWrappedWirePeekPokeCallbacks
	{
	public:

		friend class WrappedWireConnection;

		virtual void Init(RR_SHARED_PTR<WrappedWireServer> server);

		void WrappedConnectionConnected(RR_SHARED_PTR<WireConnectionBase> connection);

		virtual RR_INTRUSIVE_PTR<RRValue> do_PeekInValue(const uint32_t& ep);
		virtual RR_INTRUSIVE_PTR<RRValue> do_PeekOutValue(const uint32_t& ep);
		virtual void do_PokeOutValue(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts, const uint32_t& ep);

		void AddInValueChangedListener(WrappedWireServerPokeValueDirector* director, int32_t id);
		static void ValueChanged(const RR_INTRUSIVE_PTR<MessageElement>& m, const TimeSpec& ts, const uint32_t& ep, RR_SHARED_PTR<WrappedWireServerPokeValueDirector> spdirector);
	};

	class AsyncGeneratorClientReturnDirector
	{
	public:
		virtual ~AsyncGeneratorClientReturnDirector() {}
		virtual void handler(boost::shared_ptr<WrappedGeneratorClient> ret, uint32_t error_code, const std::string& errorname, const std::string& errormessage) {};
	};

	class WrappedGeneratorClient : public GeneratorClientBase
	{
	public:
		WrappedGeneratorClient(const std::string& name, int32_t id, RR_SHARED_PTR<ServiceStub> stub);

		virtual RR_INTRUSIVE_PTR<MessageElement> Next(RR_INTRUSIVE_PTR<MessageElement> v);
		virtual void AsyncNext(RR_INTRUSIVE_PTR<MessageElement> v, int32_t timeout, AsyncRequestDirector* handler, int32_t id);

		virtual void AsyncAbort(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id);
		virtual void AsyncClose(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id);

		virtual std::vector<RR_INTRUSIVE_PTR<MessageElement> > NextAll();
		
	protected:

		static void AsyncNext_handler(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> m, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncRequestDirector> handler);		
		static void AsyncAbort_handler(RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncVoidReturnDirector> handler);
	};

	class WrappedGeneratorServerDirector
	{
	public:
		WrappedGeneratorServerDirector()
		{
			objectheapid = 0;
		}
		virtual ~WrappedGeneratorServerDirector() {}

		virtual RR_INTRUSIVE_PTR<MessageElement> Next(RR_INTRUSIVE_PTR<MessageElement> m) { return RR_INTRUSIVE_PTR<MessageElement>(); }

		virtual void Abort() {}
		virtual void Close() {}

		int32_t objectheapid;
	};

	class WrappedGeneratorServer : public GeneratorServerBase
	{
	public:
		WrappedGeneratorServer(const std::string& name, int32_t index, RR_SHARED_PTR<ServiceSkel> skel, RR_SHARED_PTR<ServerEndpoint> ep, WrappedGeneratorServerDirector* director);

		virtual void CallNext(RR_INTRUSIVE_PTR<MessageEntry> m);

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
		static RR_INTRUSIVE_PTR<RRBaseArray> Read(RR_SHARED_PTR<ArrayMemoryBase> mem, uint64_t memorypos, uint64_t count);

		static void Write(RR_SHARED_PTR<ArrayMemoryBase> mem, uint64_t memorypos, RR_INTRUSIVE_PTR<RRBaseArray> buffer, uint64_t bufferpos, uint64_t count);

		static MemberDefinition_Direction Direction(RR_SHARED_PTR<ArrayMemoryBase> mem);
	};

	class WrappedMultiDimArrayMemoryClientUtil
	{
	public:
		static RR_SHARED_PTR<RRMultiDimArrayUntyped> Read(RR_SHARED_PTR<MultiDimArrayMemoryBase> mem, std::vector<uint64_t> memorypos, std::vector<uint64_t> count);

		static void Write(RR_SHARED_PTR<MultiDimArrayMemoryBase> mem, std::vector<uint64_t> memorypos, RR_SHARED_PTR<RRMultiDimArrayUntyped> buffer, std::vector<uint64_t> bufferpos, std::vector<uint64_t> count);

		static MemberDefinition_Direction Direction(RR_SHARED_PTR<MultiDimArrayMemoryBase> mem);
	};

	class WrappedPodArrayMemoryClientBuffer
	{
	public:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodArray> res, uint64_t bufferpos, uint64_t count) = 0;
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodArray> PackWriteRequest(uint64_t bufferpos, uint64_t count) = 0;
		virtual uint64_t GetBufferLength() = 0;
		virtual ~WrappedPodArrayMemoryClientBuffer() {}
	};

	class WrappedPodArrayMemoryClient : public virtual ArrayMemoryClientBase
	{
	public:
		WrappedPodArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction);
		virtual void Read(uint64_t memorypos, WrappedPodArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);
		virtual void Write(uint64_t memorypos, WrappedPodArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);
	protected:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, uint64_t bufferpos, uint64_t count);
		virtual RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(void* buffer, uint64_t bufferpos, uint64_t count);
		virtual size_t GetBufferLength(void* buffer);		
	};

	class WrappedPodMultiDimArrayMemoryClientBuffer
	{
	public:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodMultiDimArray> res, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodMultiDimArray> PackWriteRequest(const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
		virtual ~WrappedPodMultiDimArrayMemoryClientBuffer() {}
	};

	class WrappedPodMultiDimArrayMemoryClient : public virtual MultiDimArrayMemoryClientBase
	{		
	public:
		WrappedPodMultiDimArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction);
		virtual void Read(const std::vector<uint64_t>& memorypos, WrappedPodMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
		virtual void Write(const std::vector<uint64_t>& memorypos, WrappedPodMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);		

	protected:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount);
		virtual RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount);
	};

	//namedarray memory
	class WrappedNamedArrayMemoryClientBuffer
	{
	public:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedArray> res, uint64_t bufferpos, uint64_t count) = 0;
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedArray> PackWriteRequest(uint64_t bufferpos, uint64_t count) = 0;
		virtual uint64_t GetBufferLength() = 0;
		virtual ~WrappedNamedArrayMemoryClientBuffer() {}
	};

	class WrappedNamedArrayMemoryClient : public virtual ArrayMemoryClientBase
	{
	public:
		WrappedNamedArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction);
		virtual void Read(uint64_t memorypos, WrappedNamedArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);
		virtual void Write(uint64_t memorypos, WrappedNamedArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);
	protected:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, uint64_t bufferpos, uint64_t count);
		virtual RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(void* buffer, uint64_t bufferpos, uint64_t count);
		virtual size_t GetBufferLength(void* buffer);
	};

	class WrappedNamedMultiDimArrayMemoryClientBuffer
	{
	public:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedMultiDimArray> res, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
		virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedMultiDimArray> PackWriteRequest(const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
		virtual ~WrappedNamedMultiDimArrayMemoryClientBuffer() {}
	};

	class WrappedNamedMultiDimArrayMemoryClient : public virtual MultiDimArrayMemoryClientBase
	{
	public:
		WrappedNamedMultiDimArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction);
		virtual void Read(const std::vector<uint64_t>& memorypos, WrappedNamedMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
		virtual void Write(const std::vector<uint64_t>& memorypos, WrappedNamedMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);

	protected:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount);
		virtual RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount);
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

	std::vector<RobotRaconteur::ServiceInfo2Wrapped> WrappedFindServiceByType(RR_SHARED_PTR<RobotRaconteurNode> node, const std::string &servicetype, const std::vector<std::string>& transportschemes);

	std::vector<RobotRaconteur::NodeInfo2> WrappedFindNodeByID(RR_SHARED_PTR<RobotRaconteurNode> node, const NodeID& id, const std::vector<std::string>& transportschemes);
	
	std::vector<RobotRaconteur::NodeInfo2> WrappedFindNodeByName(RR_SHARED_PTR<RobotRaconteurNode> node, const std::string& name, const std::vector<std::string>& transportschemes);
	
	class AsyncServiceInfo2VectorReturnDirector
	{
	public:
		virtual ~AsyncServiceInfo2VectorReturnDirector() {}
		virtual void handler(const std::vector<ServiceInfo2Wrapped>& ret) {}
	};

	class AsyncNodeInfo2VectorReturnDirector
	{
	public:
		virtual ~AsyncNodeInfo2VectorReturnDirector() {}
		virtual void handler(const std::vector<NodeInfo2>& ret) {}
	};

	void AsyncServiceInfo2VectorReturn_handler(RR_SHARED_PTR<std::vector<ServiceInfo2> > ret, RR_SHARED_PTR<AsyncServiceInfo2VectorReturnDirector> handler);
	void AsyncNodeInfo2VectorReturn_handler(RR_SHARED_PTR<std::vector<NodeInfo2> > ret, RR_SHARED_PTR<AsyncNodeInfo2VectorReturnDirector> handler);

	void AsyncWrappedFindServiceByType(RR_SHARED_PTR<RobotRaconteurNode> node, const std::string &servicetype, const std::vector<std::string>& transportschemes, int32_t timeout, AsyncServiceInfo2VectorReturnDirector* handler, int32_t id1);

	void AsyncWrappedFindNodeByID(RR_SHARED_PTR<RobotRaconteurNode> node, const NodeID& id, const std::vector<std::string>& transportschemes, int32_t timeout, AsyncNodeInfo2VectorReturnDirector* handler, int32_t id1);
	
	void AsyncWrappedFindNodeByName(RR_SHARED_PTR<RobotRaconteurNode> node, const std::string& name, const std::vector<std::string>& transportschemes, int32_t timeout, AsyncNodeInfo2VectorReturnDirector* handler, int32_t id);
	
	void WrappedUpdateDetectedNodes(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<std::string>& schemes);

	void AsyncWrappedUpdateDetectedNodes(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<std::string>& schemes, int32_t timeout, AsyncVoidNoErrReturnDirector* handler, int32_t id1);

	std::vector<std::string> WrappedGetDetectedNodes(RR_SHARED_PTR<RobotRaconteurNode> node);

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
		virtual void Init(boost::shared_ptr<WrappedServiceSkel> skel) {};
		virtual RR_INTRUSIVE_PTR<MessageElement> CallGetProperty(const std::string& name) {return RR_INTRUSIVE_PTR<MessageElement>();};
		virtual void CallSetProperty(const std::string& name, RR_INTRUSIVE_PTR<MessageElement> m) {};
		virtual RR_INTRUSIVE_PTR<MessageElement> CallFunction(const std::string& name, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& m) {return RR_INTRUSIVE_PTR<MessageElement>();};
		virtual boost::shared_ptr<WrappedRRObject> GetSubObj(const std::string& name, const std::string& index) {return boost::shared_ptr<WrappedRRObject>();};
		virtual WrappedArrayMemoryDirector* GetArrayMemory(const std::string& name) {return 0;};
		virtual WrappedMultiDimArrayMemoryDirector* GetMultiDimArrayMemory(const std::string& name) {return 0;};
		virtual WrappedPodArrayMemoryDirector* GetPodArrayMemory(const std::string& name) { return 0; };
		virtual WrappedPodMultiDimArrayMemoryDirector* GetPodMultiDimArrayMemory(const std::string& name) { return 0; };
		virtual WrappedNamedArrayMemoryDirector* GetNamedArrayMemory(const std::string& name) { return 0; };
		virtual WrappedNamedMultiDimArrayMemoryDirector* GetNamedMultiDimArrayMemory(const std::string& name) { return 0; };


		virtual void MonitorEnter(int32_t timeout) {};
		virtual void MonitorExit() {};
		virtual void ReleaseCastObject() {};

	};

	class WrappedServiceSkel : public virtual ServiceSkel
	{
	public:
		/*WrappedServiceSkel()
		{
			RR_Director=NULL;
		}*/

		virtual ~WrappedServiceSkel() {}
		boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> Type;

		boost::shared_ptr<WrappedRRObject> castobj;
		virtual void Init(const std::string &s, RR_SHARED_PTR<RRObject> o, RR_SHARED_PTR<ServerContext> c);

		virtual RR_INTRUSIVE_PTR<MessageEntry> CallGetProperty(RR_INTRUSIVE_PTR<MessageEntry> m);
		virtual RR_INTRUSIVE_PTR<MessageEntry> CallSetProperty(RR_INTRUSIVE_PTR<MessageEntry> m);
		virtual RR_INTRUSIVE_PTR<MessageEntry> CallFunction(RR_INTRUSIVE_PTR<MessageEntry> m);
		virtual RR_SHARED_PTR<RRObject> GetSubObj(const std::string &name, const std::string &ind);
		virtual void ReleaseCastObject();

		virtual std::string GetObjectType();
	protected:
		boost::shared_ptr<WrappedServiceSkelDirector> RR_Director;
		boost::shared_mutex RR_Director_lock;
	public:

		//WrappedServiceSkelDirector* GetRRDirector();
		//void SetRRDirector(WrappedServiceSkelDirector* director, int32_t id);

		virtual RR_SHARED_PTR<RobotRaconteur::WrappedPipeServer> GetPipe(const std::string& membername);
		virtual RR_SHARED_PTR<RobotRaconteur::WrappedWireServer> GetWire(const std::string& membername);

		std::map<std::string, RR_SHARED_PTR<WrappedPipeServer> > pipes;
		std::map<std::string, RR_SHARED_PTR<WrappedWireServer> > wires;

		virtual void DispatchPipeMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, uint32_t endpoint);
		virtual void DispatchWireMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, uint32_t endpoint);

		virtual RR_INTRUSIVE_PTR<MessageEntry> CallPipeFunction(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

		virtual RR_INTRUSIVE_PTR<MessageEntry> CallWireFunction(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

		virtual void WrappedDispatchEvent(const std::string& name, const std::vector<RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> >& m);

		virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> WrappedCallbackCall(const std::string& name, uint32_t endpoint, const std::vector<RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> >& m);

		virtual RR_INTRUSIVE_PTR<MessageEntry> CallMemoryFunction(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e);

		std::map<std::string,boost::shared_ptr<void> > memories;
		std::map<std::string, boost::shared_ptr<WrappedPodArrayMemoryServiceSkel> > pod_memories;
		std::map<std::string, boost::shared_ptr<WrappedPodMultiDimArrayMemoryServiceSkel> > pod_multidimmemories;
		std::map<std::string, boost::shared_ptr<WrappedNamedArrayMemoryServiceSkel> > namedarray_memories;
		std::map<std::string, boost::shared_ptr<WrappedNamedMultiDimArrayMemoryServiceSkel> > namedarray_multidimmemories;
		std::map<std::string,boost::shared_ptr<MemoryDefinition> > memorytypes;

		virtual int32_t RegisterGeneratorServer(const std::string& function_name, WrappedGeneratorServerDirector* gen);

	};

	class WrappedRRObject : public RRObject, public IRobotRaconteurMonitorObject
	{
	public:
		WrappedRRObject(const std::string& type, WrappedServiceSkelDirector* RR_Director, int32_t id);
		virtual std::string RRType();
		std::string Type;
	protected:

		boost::shared_ptr<WrappedServiceSkelDirector> RR_Director;
		boost::shared_mutex RR_Director_lock;
		
	public:

		boost::shared_ptr<WrappedServiceSkelDirector> GetRRDirector();
		//void SetRRDirector(WrappedServiceSkelDirector* director, int32_t id);

		virtual void RobotRaconteurMonitorEnter()
		{
			DIRECTOR_CALL(WrappedServiceSkelDirector,RR_Director2->MonitorEnter(-1));
		}


		virtual void RobotRaconteurMonitorEnter(int32_t timeout)
		{
			


			DIRECTOR_CALL(WrappedServiceSkelDirector,RR_Director2->MonitorEnter(timeout));
		}

		virtual void RobotRaconteurMonitorExit()
		{
			DIRECTOR_CALL(WrappedServiceSkelDirector,RR_Director2->MonitorExit());
		}


		virtual ~WrappedRRObject();
	};


	class WrappedArrayMemoryDirector
	{
	public:
		WrappedArrayMemoryDirector()
		{
			objectheapid=0;
		}

		virtual ~WrappedArrayMemoryDirector() {}
		virtual uint64_t Length() {return 0;}
		virtual void Read(uint64_t memorypos, boost::intrusive_ptr<RRBaseArray> buffer, uint64_t bufferpos, uint64_t count) {}
		virtual void Write(uint64_t memorypos, boost::intrusive_ptr<RRBaseArray> buffer, uint64_t bufferpos, uint64_t count) {}
		
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
			//this->RR_Director=0;
			if (!RR_Director) throw InvalidArgumentException("RR_Director cannot be null");
			this->RR_Director.reset(RR_Director, boost::bind(&ReleaseDirector<WrappedArrayMemoryDirector>, _1, RR_Director->objectheapid));
		}

		virtual ~WrappedArrayMemory()
		{
			
		}

		virtual uint64_t Length()
		{
			if (!RR_Director) throw InvalidOperationException("Director not set");
			uint64_t len;
			DIRECTOR_CALL(WrappedArrayMemoryDirector,len=RR_Director2->Length());
			return len;
		}
		virtual void Read(uint64_t memorypos, boost::intrusive_ptr<RRArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			if (!RR_Director) throw InvalidOperationException("Director not set");
			boost::intrusive_ptr<RRBaseArray> buffer2=rr_cast<RRBaseArray>(buffer);
			DIRECTOR_CALL(WrappedArrayMemoryDirector,RR_Director2->Read(memorypos,buffer2,bufferpos,count));

		}
		virtual void Write(uint64_t memorypos, boost::intrusive_ptr<RRArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			if (!RR_Director) throw InvalidOperationException("Director not set");
			boost::intrusive_ptr<RRBaseArray> buffer2=rr_cast<RRBaseArray>(buffer);
			DIRECTOR_CALL(WrappedArrayMemoryDirector,RR_Director2->Write(memorypos,buffer2,bufferpos,count));
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

		virtual ~WrappedMultiDimArrayMemoryDirector() {}
		virtual std::vector<uint64_t> Dimensions() {return std::vector<uint64_t>();}
		virtual uint64_t DimCount() {return 0;}
		virtual bool Complex() {return false;}
		virtual void Read(WrappedMultiDimArrayMemoryParams* p) {}
		virtual void Write(WrappedMultiDimArrayMemoryParams* p) {}

		int32_t objectheapid;
		
	};

	template <class T>
	class WrappedMultiDimArrayMemory : public virtual MultiDimArrayMemory<T>
	{
	public:
		boost::shared_ptr<WrappedMultiDimArrayMemoryDirector> RR_Director;
		boost::shared_mutex RR_Director_lock;
		WrappedMultiDimArrayMemory(WrappedMultiDimArrayMemoryDirector* RR_Director)
		{
			//this->RR_Director=0;
			if (!RR_Director) throw InvalidArgumentException("RR_Director cannot be null");
			this->RR_Director.reset(RR_Director, boost::bind(&ReleaseDirector<WrappedMultiDimArrayMemoryDirector>, _1, RR_Director->objectheapid));
		}

		virtual ~WrappedMultiDimArrayMemory()
		{
			

			
		}

		virtual std::vector<uint64_t> Dimensions()
		{
			if (!RR_Director) throw InvalidOperationException("Director not set");
			std::vector<uint64_t> dims;
			DIRECTOR_CALL(WrappedMultiDimArrayMemoryDirector,dims=RR_Director2->Dimensions());
			return dims;
		}

		virtual uint64_t DimCount()
		{
			if (!RR_Director) throw InvalidOperationException("Director not set");
			uint64_t dim;
			DIRECTOR_CALL(WrappedMultiDimArrayMemoryDirector,dim=RR_Director2->DimCount());
			return dim;
		}

		virtual bool Complex()
		{
			if (!RR_Director) throw InvalidOperationException("Director not set");
			bool c;
			DIRECTOR_CALL(WrappedMultiDimArrayMemoryDirector,c=RR_Director2->Complex());
			return c;
		}

		virtual void Read(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			if (!RR_Director) throw InvalidOperationException("Director not set");
			WrappedMultiDimArrayMemoryParams p;
			p.memorypos=memorypos;
			boost::shared_ptr<RRMultiDimArrayUntyped> buffer2=boost::make_shared<RRMultiDimArrayUntyped>();
			buffer2->Dims=buffer->Dims;
			buffer2->Array=buffer->Array;			

			p.buffer=buffer2;
			p.bufferpos=bufferpos;
			p.count=count;

			DIRECTOR_CALL(WrappedMultiDimArrayMemoryDirector,RR_Director2->Read(&p))
		}

		virtual void Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			if (!RR_Director) throw InvalidOperationException("Director not set");
			WrappedMultiDimArrayMemoryParams p;
			p.memorypos=memorypos;
			boost::shared_ptr<RRMultiDimArrayUntyped> buffer2=boost::make_shared<RRMultiDimArrayUntyped>();
			buffer2->Dims=buffer->Dims;
			buffer2->Array=buffer->Array;
			
			p.buffer=buffer2;
			p.bufferpos=bufferpos;
			p.count=count;

			DIRECTOR_CALL(WrappedMultiDimArrayMemoryDirector,RR_Director2->Write(&p))
		}
	};

	class WrappedPodArrayMemoryDirector
	{
	public:

		WrappedPodArrayMemoryDirector()
		{
			objectheapid = 0;
		}

		virtual ~WrappedPodArrayMemoryDirector() {}
		virtual uint64_t Length() { return 0; }
		virtual RR_INTRUSIVE_PTR<MessageElementPodArray> Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count) { return RR_INTRUSIVE_PTR<MessageElementPodArray>(); }
		virtual void Write(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementPodArray> buffer, uint64_t bufferpos, uint64_t count) {}
		int32_t objectheapid;
	};

	class WrappedPodArrayMemory : public ArrayMemoryBase
	{
	public:

		WrappedPodArrayMemory(WrappedPodArrayMemoryDirector* RR_Director);

		virtual uint64_t Length();
		virtual DataTypes ElementTypeID();

		virtual RR_INTRUSIVE_PTR<MessageElementPodArray> Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count);
		virtual void Write(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementPodArray> buffer, uint64_t bufferpos, uint64_t count);

		boost::shared_ptr<WrappedPodArrayMemoryDirector> RR_Director;
		boost::shared_mutex RR_Director_lock;
	};

	class WrappedPodArrayMemoryServiceSkel : public ArrayMemoryServiceSkelBase
	{
	public:
		WrappedPodArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction);
		virtual RR_INTRUSIVE_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem);
		virtual void DoWrite(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem);
	};

	class WrappedPodMultiDimArrayMemoryDirector
	{
	public:
		
		virtual ~WrappedPodMultiDimArrayMemoryDirector() {}
		virtual std::vector<uint64_t> Dimensions() { return std::vector<uint64_t>(); }
		virtual uint64_t DimCount() { return 0; }		
		virtual RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> Read(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) { return RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray>();  }
		virtual void Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) {}

		int32_t objectheapid;
	};

	class WrappedPodMultiDimArrayMemory : public MultiDimArrayMemoryBase
	{
	public:

		WrappedPodMultiDimArrayMemory(WrappedPodMultiDimArrayMemoryDirector* RR_Director);

		virtual std::vector<uint64_t> Dimensions();
		virtual uint64_t DimCount();
		virtual bool Complex();
		virtual DataTypes ElementTypeID();

		virtual RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> Read(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
		virtual void Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);

		boost::shared_ptr<WrappedPodMultiDimArrayMemoryDirector> RR_Director;
		boost::shared_mutex RR_Director_lock;
	};

	class WrappedPodMultiDimArrayMemoryServiceSkel : public MultiDimArrayMemoryServiceSkelBase
	{
	public:
		WrappedPodMultiDimArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction);
		virtual RR_INTRUSIVE_PTR<MessageElementData> DoRead(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elem_count, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem);
		virtual void DoWrite(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elem_count, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem);
	};

	class WrappedNamedArrayMemory : public ArrayMemoryBase
	{
	public:

		WrappedNamedArrayMemory(WrappedNamedArrayMemoryDirector* RR_Director);

		virtual uint64_t Length();
		virtual DataTypes ElementTypeID();

		virtual RR_INTRUSIVE_PTR<MessageElementNamedArray> Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count);
		virtual void Write(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementNamedArray> buffer, uint64_t bufferpos, uint64_t count);

		boost::shared_ptr<WrappedNamedArrayMemoryDirector> RR_Director;
		boost::shared_mutex RR_Director_lock;
	};

	class WrappedNamedArrayMemoryDirector
	{
	public:

		WrappedNamedArrayMemoryDirector()
		{
			objectheapid = 0;
		}

		virtual ~WrappedNamedArrayMemoryDirector() {}
		virtual uint64_t Length() { return 0; }
		virtual RR_INTRUSIVE_PTR<MessageElementNamedArray> Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count) { return RR_INTRUSIVE_PTR<MessageElementNamedArray>(); }
		virtual void Write(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementNamedArray> buffer, uint64_t bufferpos, uint64_t count) {}
		int32_t objectheapid;
	};

	class WrappedNamedArrayMemoryServiceSkel : public ArrayMemoryServiceSkelBase
	{
	public:
		WrappedNamedArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction);
		virtual RR_INTRUSIVE_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem);
		virtual void DoWrite(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem);
	};

	class WrappedNamedMultiDimArrayMemoryDirector
	{
	public:

		virtual ~WrappedNamedMultiDimArrayMemoryDirector() {}
		virtual std::vector<uint64_t> Dimensions() { return std::vector<uint64_t>(); }
		virtual uint64_t DimCount() { return 0; }
		virtual RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> Read(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) { return RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray>(); }
		virtual void Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) {}

		int32_t objectheapid;
	};

	class WrappedNamedMultiDimArrayMemory : public MultiDimArrayMemoryBase
	{
	public:

		WrappedNamedMultiDimArrayMemory(WrappedNamedMultiDimArrayMemoryDirector* RR_Director);

		virtual std::vector<uint64_t> Dimensions();
		virtual uint64_t DimCount();
		virtual bool Complex();
		virtual DataTypes ElementTypeID();

		virtual RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> Read(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
		virtual void Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);

		boost::shared_ptr<WrappedNamedMultiDimArrayMemoryDirector> RR_Director;
		boost::shared_mutex RR_Director_lock;
	};

	class WrappedNamedMultiDimArrayMemoryServiceSkel : public MultiDimArrayMemoryServiceSkelBase
	{
	public:
		WrappedNamedMultiDimArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction);
		virtual RR_INTRUSIVE_PTR<MessageElementData> DoRead(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elem_count, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem);
		virtual void DoWrite(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elem_count, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem);
	};

	void AsyncStubReturn_handler(RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncStubReturnDirector> handler);
	
	void AsyncVoidNoErrReturn_handler(RR_SHARED_PTR<AsyncVoidNoErrReturnDirector> handler);
	

	void AsyncStringReturn_handler(RR_SHARED_PTR<std::string> str, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncStringReturnDirector> handler);
	
	void WrappedExceptionHandler(const std::exception* err, RR_SHARED_PTR<AsyncVoidReturnDirector> handler);

	void TimerHandlerFunc(const TimerEvent& ev, RR_SHARED_PTR<AsyncTimerEventReturnDirector> d);
	
	class WrappedUserAuthenticatorDirector
	{
	public:
		virtual boost::shared_ptr<AuthenticatedUser> AuthenticateUser(const std::string &username, RR_INTRUSIVE_PTR<MessageElement> credentials, boost::shared_ptr<ServerContext> context) { return boost::shared_ptr<AuthenticatedUser>(); }

		virtual ~WrappedUserAuthenticatorDirector() {}
	};

	class WrappedUserAuthenticator : public UserAuthenticator
	{

		boost::shared_ptr<WrappedUserAuthenticatorDirector> RR_Director;
	public:
		void SetRRDirector(WrappedUserAuthenticatorDirector* director, int id);

		virtual RR_SHARED_PTR<AuthenticatedUser> AuthenticateUser(const std::string &username, const std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > &credentials, RR_SHARED_PTR<ServerContext> context);
		
	};

	//Subscriptions

	class WrappedServiceSubscriptionFilterPredicateDirector
	{
	public:
		virtual bool Predicate(const ServiceInfo2Wrapped& info) = 0;

		virtual bool CallPredicate(const ServiceInfo2& info);

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
	};

	class WrappedServiceInfo2Subscription;

	class WrappedServiceInfo2SubscriptionDirector
	{
	public:
		virtual void ServiceDetected(boost::shared_ptr<WrappedServiceInfo2Subscription> subscription, const ServiceSubscriptionClientID& id, const ServiceInfo2Wrapped& service) {}
		virtual void ServiceLost(boost::shared_ptr<WrappedServiceInfo2Subscription> subscription, const ServiceSubscriptionClientID& id, const ServiceInfo2Wrapped& service) {}

		virtual ~WrappedServiceInfo2SubscriptionDirector() {}
	};

	class WrappedServiceInfo2Subscription : public RR_ENABLE_SHARED_FROM_THIS<WrappedServiceInfo2Subscription>
	{
	public:
		WrappedServiceInfo2Subscription(RR_SHARED_PTR<ServiceInfo2Subscription> subscription);

		std::map<ServiceSubscriptionClientID, ServiceInfo2Wrapped > GetDetectedServiceInfo2();

		void Close();

		void SetRRDirector(WrappedServiceInfo2SubscriptionDirector* director, int32_t id);

	protected:

		RR_SHARED_PTR<ServiceInfo2Subscription> subscription;
		RR_SHARED_PTR<WrappedServiceInfo2SubscriptionDirector> RR_Director;
		boost::shared_mutex RR_Director_lock;
		boost::initialized<bool> events_connected;

		static void ServiceDetected(RR_WEAK_PTR<WrappedServiceInfo2Subscription> this_, RR_SHARED_PTR<ServiceInfo2Subscription> subscription, const ServiceSubscriptionClientID& id, const ServiceInfo2& info);
		static void ServiceLost(RR_WEAK_PTR<WrappedServiceInfo2Subscription> this_, RR_SHARED_PTR<ServiceInfo2Subscription> subscription, const ServiceSubscriptionClientID& id, const ServiceInfo2& info);
		void ServiceDetected1(RR_SHARED_PTR<ServiceInfo2Subscription>& subscription, const ServiceSubscriptionClientID& id, const ServiceInfo2& info);
		void ServiceLost1(RR_SHARED_PTR<ServiceInfo2Subscription>& subscription, const ServiceSubscriptionClientID& id, const ServiceInfo2& info);
	};


	class WrappedService_typed_packet
	{
	public:
		RR_INTRUSIVE_PTR<MessageElement> packet;
		RR_SHARED_PTR<TypeDefinition> type;
		RR_SHARED_PTR<WrappedServiceStub> stub;		
	};

	class WrappedServiceSubscription;

	class WrappedServiceSubscriptionDirector
	{
	public:
		virtual void ClientConnected(boost::shared_ptr<WrappedServiceSubscription> subscription, const ServiceSubscriptionClientID& id, boost::shared_ptr<WrappedServiceStub> client) = 0;
		virtual void ClientDisconnected(boost::shared_ptr<WrappedServiceSubscription> subscription, const ServiceSubscriptionClientID& id, boost::shared_ptr<WrappedServiceStub> client) = 0;
		
		virtual ~WrappedServiceSubscriptionDirector() {}
	};

	class WrappedWireSubscription;
	class WrappedPipeSubscription;

	class WrappedServiceSubscription : public RR_ENABLE_SHARED_FROM_THIS<WrappedServiceSubscription>
	{
	public:

		WrappedServiceSubscription(RR_SHARED_PTR<ServiceSubscription> subscription);

		std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<WrappedServiceStub> > GetConnectedClients();

		void Close();
		
		void ClaimClient(RR_SHARED_PTR<WrappedServiceStub> client);
		void ReleaseClient(RR_SHARED_PTR<WrappedServiceStub> client);

		uint32_t GetConnectRetryDelay();
		void SetConnectRetryDelay(uint32_t delay_milliseconds);

		RR_SHARED_PTR<WrappedWireSubscription> SubscribeWire(const std::string& membername);

		RR_SHARED_PTR<WrappedPipeSubscription> SubscribePipe(const std::string& membername, uint32_t max_recv_packets = std::numeric_limits<uint32_t>::max());

		void SetRRDirector(WrappedServiceSubscriptionDirector* director, int32_t id);
		
	protected:

		RR_SHARED_PTR<ServiceSubscription> subscription;
		RR_SHARED_PTR<WrappedServiceSubscriptionDirector> RR_Director;
		boost::shared_mutex RR_Director_lock;
		boost::initialized<bool> events_connected;

		static void ClientConnected(RR_WEAK_PTR<WrappedServiceSubscription> this_, RR_SHARED_PTR<ServiceSubscription> subscription, const ServiceSubscriptionClientID& id, RR_SHARED_PTR<RRObject> client);
		static void ClientDisconnected(RR_WEAK_PTR<WrappedServiceSubscription> this_, RR_SHARED_PTR<ServiceSubscription> subscription, const ServiceSubscriptionClientID& id, RR_SHARED_PTR<RRObject> client);
		void ClientConnected1(RR_SHARED_PTR<ServiceSubscription>& subscription, const ServiceSubscriptionClientID& id, RR_SHARED_PTR<RRObject>& client);
		void ClientDisconnected1(RR_SHARED_PTR<ServiceSubscription>& subscription, const ServiceSubscriptionClientID& id, RR_SHARED_PTR<RRObject>& client);

	};

	class WrappedWireSubscriptionDirector
	{
	public:
		virtual void WireValueChanged(boost::shared_ptr<WrappedWireSubscription> wire_subscription, WrappedService_typed_packet& value, const TimeSpec& time) = 0;
		
		virtual ~WrappedWireSubscriptionDirector() {}
	};

	class WrappedWireSubscription_send_iterator;

	class WrappedWireSubscription : public WireSubscriptionBase
	{
	public:

		friend class WrappedWireSubscription_send_iterator;

		WrappedWireSubscription(RR_SHARED_PTR<ServiceSubscription> parent, const std::string& membername);

		WrappedService_typed_packet GetInValue(TimeSpec* time = NULL);
		bool TryGetInValue(WrappedService_typed_packet& val, TimeSpec* time = NULL);
		
		void SetRRDirector(WrappedWireSubscriptionDirector* director, int32_t id);
		
	protected:

		virtual void fire_WireValueChanged(RR_INTRUSIVE_PTR<RRValue> value, const TimeSpec& time, RR_SHARED_PTR<WireConnectionBase> connection);
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
		virtual void PipePacketReceived(boost::shared_ptr<WrappedPipeSubscription> pipe_subscription) = 0;

		virtual ~WrappedPipeSubscriptionDirector() {}
	};

	class WrappedPipeSubscription_send_iterator;

	class WrappedPipeSubscription : public PipeSubscriptionBase
	{
	public:

		friend class WrappedPipeSubscription_send_iterator;

		WrappedPipeSubscription(RR_SHARED_PTR<ServiceSubscription> parent, const std::string& membername, int32_t max_recv_packets = -1, int32_t max_send_backlog = 5);

		WrappedService_typed_packet ReceivePacket();
		bool TryReceivePacket(WrappedService_typed_packet& packet);
		bool TryReceivePacketWait(WrappedService_typed_packet& packet, int32_t timeout = RR_TIMEOUT_INFINITE, bool peek = false);
		
		void SetRRDirector(WrappedPipeSubscriptionDirector* director, int32_t id);

	protected:

		virtual void fire_PipePacketReceived();
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

	std::vector<ServiceSubscriptionClientID> WrappedServiceSubscriptionClientsToVector(std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<WrappedServiceStub> >& clients);

	std::vector<ServiceSubscriptionClientID> WrappedServiceInfo2SubscriptionServicesToVector(std::map<ServiceSubscriptionClientID, ServiceInfo2Wrapped>& infos);
	
	RR_SHARED_PTR<WrappedServiceInfo2Subscription> WrappedSubscribeServiceInfo2(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<std::string>& service_types, RR_SHARED_PTR<WrappedServiceSubscriptionFilter> filter = RR_SHARED_PTR<WrappedServiceSubscriptionFilter>());

	RR_SHARED_PTR<WrappedServiceSubscription> WrappedSubscribeService(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<std::string>& service_types, RR_SHARED_PTR<WrappedServiceSubscriptionFilter> filter = RR_SHARED_PTR<WrappedServiceSubscriptionFilter>());
	
#ifdef RR_PYTHON

	class PythonThreadPool : public ThreadPool
	{
	public:
		PythonThreadPool(RR_SHARED_PTR<RobotRaconteurNode> node) : ThreadPool(node)
		{

		}
	protected:
		virtual void thread_function()
		{
			DIRECTOR_CALL2(
				PyRun_SimpleString("import RobotRaconteur.RobotRaconteurPythonUtil as RRU\nRRU.settrace()\n");
				
				
				Py_BEGIN_ALLOW_THREADS
				ThreadPool::thread_function();
				Py_END_ALLOW_THREADS
				)
			
			
		}

	};


	class PythonThreadPoolFactory : public ThreadPoolFactory
	{
	public: 
		virtual RR_SHARED_PTR<ThreadPool> NewThreadPool(RR_SHARED_PTR<RobotRaconteurNode> node)
		{
			return RR_MAKE_SHARED<PythonThreadPool>(node);
		}
	};
#endif

}
