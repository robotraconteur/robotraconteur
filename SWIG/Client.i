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

%shared_ptr(RobotRaconteur::WrappedServiceStub)
%shared_ptr(RobotRaconteur::RobotRaconteurNode)

%feature("director") RobotRaconteur::WrappedServiceStubDirector;
%feature("director") RobotRaconteur::AsyncStubReturnDirector;

//ServiceStub

namespace RobotRaconteur
{

class RobotRaconteurNode;

RR_DIRECTOR_SHARED_PTR_RETURN_RESET(RobotRaconteur::MessageElement)
class WrappedServiceStubDirector
{
public:
	virtual ~WrappedServiceStubDirector() {}
	virtual void DispatchEvent(const std::string& EventName, std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> > args);
	virtual boost::intrusive_ptr<RobotRaconteur::MessageElement> CallbackCall(const std::string& CallbackName, std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> > args);
};
RR_DIRECTOR_SHARED_PTR_RETURN_DEFAULT(RobotRaconteur::MessageElement)

class AsyncStubReturnDirector
{
public:
	virtual ~AsyncStubReturnDirector() {}
	virtual void handler(boost::shared_ptr<RobotRaconteur::WrappedServiceStub> stub, uint32_t error_code, const std::string& errorname, const std::string& errormessage);
};

%nodefaultctor WrappedServiceStub;
class WrappedServiceStub : public virtual RobotRaconteur::RRObject
{
public:

RR_RELEASE_GIL()

	virtual boost::intrusive_ptr<RobotRaconteur::MessageElement> PropertyGet(const std::string& PropertyName);
	virtual void PropertySet(const std::string& PropertyName, boost::intrusive_ptr<RobotRaconteur::MessageElement> value);
	virtual boost::intrusive_ptr<RobotRaconteur::MessageElement> FunctionCall(const std::string& FunctionName, const std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> >& args);
	virtual boost::shared_ptr<RobotRaconteur::WrappedGeneratorClient> GeneratorFunctionCall(const std::string& FunctionName, const std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> >& args);
	
RR_KEEP_GIL()
	
	virtual void async_PropertyGet(const std::string& PropertyName, int32_t timeout, AsyncRequestDirector* handler,int32_t id);
	virtual void async_PropertySet(const std::string& PropertyName, boost::intrusive_ptr<RobotRaconteur::MessageElement> value, int32_t timeout, AsyncRequestDirector* handler,int32_t id);
	virtual void async_FunctionCall(const std::string& FunctionName, const std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> >& args, int32_t timeout, AsyncRequestDirector* handler, int32_t id);
	virtual void async_GeneratorFunctionCall(const std::string& FunctionName, const std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> >& args, int32_t timeout, AsyncGeneratorClientReturnDirector* handler, int32_t id);
	
	virtual void async_FindObjRef(const std::string& path, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id);
	virtual void async_FindObjRef(const std::string& path, const std::string& ind, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id);
	virtual void async_FindObjRefTyped(const std::string& path, const std::string& type, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id);
	virtual void async_FindObjRefTyped(const std::string& path, const std::string& ind, const std::string& type, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id);
		
RR_RELEASE_GIL()	
	
	virtual boost::shared_ptr<RobotRaconteur::WrappedPipeClient> GetPipe(const std::string& membername);
	virtual boost::shared_ptr<RobotRaconteur::WrappedWireClient> GetWire(const std::string& membername);
	int GetObjectHeapID();
%extend
{
	boost::shared_ptr<WrappedServiceStub> FindObjRef(const std::string &n)
	{
		return rr_cast<WrappedServiceStub>($self->FindObjRef(n));
	}

	boost::shared_ptr<WrappedServiceStub> FindObjRef(const std::string &n, const std::string &i)
	{
		return rr_cast<WrappedServiceStub>($self->FindObjRef(n,i));
	}
	
	boost::shared_ptr<WrappedServiceStub> FindObjRefTyped(const std::string &n, const std::string& objecttype)
	{
		return rr_cast<WrappedServiceStub>($self->FindObjRefTyped(n,objecttype));
	}

	boost::shared_ptr<WrappedServiceStub> FindObjRefTyped(const std::string &n, const std::string &i, const std::string& objecttype)
	{
		return rr_cast<WrappedServiceStub>($self->FindObjRefTyped(n,i,objecttype));
	}


}

	virtual boost::shared_ptr<RobotRaconteur::ArrayMemoryBase> GetArrayMemory(const std::string& membername);
	virtual boost::shared_ptr<RobotRaconteur::MultiDimArrayMemoryBase> GetMultiDimArrayMemory(const std::string& membername);
	virtual boost::shared_ptr<RobotRaconteur::WrappedPodArrayMemoryClient> GetPodArrayMemory(const std::string& membername);
	virtual boost::shared_ptr<RobotRaconteur::WrappedPodMultiDimArrayMemoryClient> GetPodMultiDimArrayMemory(const std::string& membername);
	virtual boost::shared_ptr<RobotRaconteur::WrappedNamedArrayMemoryClient> GetNamedArrayMemory(const std::string& membername);
	virtual boost::shared_ptr<RobotRaconteur::WrappedNamedMultiDimArrayMemoryClient> GetNamedMultiDimArrayMemory(const std::string& membername);

	
	virtual void RRClose();
	
RR_KEEP_GIL()
	
	boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> RR_objecttype;
	virtual std::string RRType();
	//WrappedServiceStubDirector* RR_Director;


		
	void SetRRDirector(WrappedServiceStubDirector* director, int32_t id);
	
	boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> RRGetNode();
	
#ifdef SWIGPYTHON

RR_RELEASE_GIL()	
	PyObject* GetPyStub();	
	void SetPyStub(PyObject* stub);
RR_KEEP_GIL()
	
#endif
	
};

}
