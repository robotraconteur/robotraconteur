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

#include "RobotRaconteurWrapped.h"
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/range/adaptors.hpp>
//#include <Python.h>


namespace RobotRaconteur
{


	boost::mutex RRNativeObjectHeapSupport::support_lock;
	RRNativeObjectHeapSupport* RRNativeObjectHeapSupport::support=NULL;

#ifdef RR_PYTHON
	bool RRNativeDirectorSupport::running=false;
	boost::shared_mutex RRNativeDirectorSupport::running_lock;
#endif
	//Wrapped Service Factory

	WrappedServiceFactory::WrappedServiceFactory(const std::string& defstring)
	{
		this->defstring=defstring;
		servicedef=RR_MAKE_SHARED<ServiceDefinition>();
		servicedef->FromString(defstring);
	}

	WrappedServiceFactory::WrappedServiceFactory(boost::shared_ptr<RobotRaconteur::ServiceDefinition> def)
	{
		this->defstring=def->ToString();
		servicedef=def;
		
	}

	std::string WrappedServiceFactory::GetServiceName()
	{
		return servicedef->Name;
	}

	std::string WrappedServiceFactory::DefString()
	{
		return defstring;
	}
	
	RR_SHARED_PTR<ServiceDefinition> WrappedServiceFactory::ServiceDef()
	{
		return servicedef;
	}

	RR_SHARED_PTR<RobotRaconteur::StructureStub> WrappedServiceFactory::FindStructureStub(const std::string& s)
	{
		throw ServiceException("Invalid for wrapped service type");
	}

	RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementStructure> WrappedServiceFactory::PackStructure(RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure> structin)
	{
		throw ServiceException("Invalid for wrapped service type");
	}

	RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> WrappedServiceFactory::UnpackStructure(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementStructure> mstructin)
	{
		throw ServiceException("Invalid for wrapped service type");
	}

	RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodArray> WrappedServiceFactory::PackPodArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray> structure)
	{
		throw ServiceException("Invalid for wrapped service type");
	}
	RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray> WrappedServiceFactory::UnpackPodArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodArray> structure)
	{
		throw ServiceException("Invalid for wrapped service type");
	}
	RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodMultiDimArray> WrappedServiceFactory::PackPodMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray> structure)
	{
		throw ServiceException("Invalid for wrapped service type");
	}
	RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray> WrappedServiceFactory::UnpackPodMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodMultiDimArray> structure)
	{
		throw ServiceException("Invalid for wrapped service type");
	}
	
	RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedArray> WrappedServiceFactory::PackNamedArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray> structure)
	{
		throw ServiceException("Invalid for wrapped service type");
	}
	RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray> WrappedServiceFactory::UnpackNamedArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedArray> structure)
	{
		throw ServiceException("Invalid for wrapped service type");
	}
	RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedMultiDimArray> WrappedServiceFactory::PackNamedMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray> structure)
	{
		throw ServiceException("Invalid for wrapped service type");
	}
	RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray> WrappedServiceFactory::UnpackNamedMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedMultiDimArray> structure)
	{
		throw ServiceException("Invalid for wrapped service type");
	}
	
	RR_SHARED_PTR<RobotRaconteur::ServiceStub> WrappedServiceFactory::CreateStub(const std::string& type, const std::string& path, RR_SHARED_PTR<RobotRaconteur::ClientContext> context)
	{
		boost::tuple<std::string,std::string> res=SplitQualifiedName(type);
		
		std::string servicetype=res.get<0>();
		std::string objecttype=res.get<1>();
		if (servicetype != GetServiceName()) return RobotRaconteur::RobotRaconteurNode::s()->GetServiceType(servicetype)->CreateStub(type,path,context);
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::iterator ee=servicedef->Objects.begin(); ee!=servicedef->Objects.end(); ++ee)
		{
			if ((*ee)->Name==objecttype)
			{
				RR_SHARED_PTR<WrappedServiceStub> out=RR_MAKE_SHARED<WrappedServiceStub>(path,*ee,context);
				out->RRInitStub();
				return out;
			}
		}
		throw RobotRaconteur::ServiceException("Invalid service stub type.");
	}

	RR_SHARED_PTR<RobotRaconteur::ServiceSkel> WrappedServiceFactory::CreateSkel(const std::string& type, const std::string& path, RR_SHARED_PTR<RobotRaconteur::RRObject> obj, RR_SHARED_PTR<RobotRaconteur::ServerContext> context)
	{
		boost::tuple<std::string,std::string> res=SplitQualifiedName(type);
		
		std::string servicetype=res.get<0>();
		std::string objecttype=res.get<1>();
		if (servicetype != GetServiceName()) return RobotRaconteur::RobotRaconteurNode::s()->GetServiceType(servicetype)->CreateSkel(type,path,obj,context);
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::iterator ee=servicedef->Objects.begin(); ee!=servicedef->Objects.end(); ++ee)
		{
			if ((*ee)->Name==objecttype)
			{
				RR_SHARED_PTR<WrappedServiceSkel> out=RR_MAKE_SHARED<WrappedServiceSkel>();
				out->Init(path,obj,context);
				return out;
			}
		}
		throw RobotRaconteur::ServiceException("Invalid service skel type.");

	}

	//Wrapped Dynamic Service Factory
	RR_SHARED_PTR<ServiceFactory> WrappedDynamicServiceFactory::CreateServiceFactory(const std::string& def)
	{
		return RR_MAKE_SHARED<WrappedServiceFactory>(def);
	}
	std::vector<RR_SHARED_PTR<ServiceFactory> > WrappedDynamicServiceFactory::CreateServiceFactories(const std::vector<std::string>& def)
	{
		std::vector<RR_SHARED_PTR<ServiceFactory> > out;
		for (std::vector<std::string>::const_iterator ee=def.begin(); ee!=def.end(); ++ee)
		{
			out.push_back(RR_MAKE_SHARED<WrappedServiceFactory>(*ee));
		}
		return out;
	}


	//Wrapped Service Stub
	WrappedServiceStub::WrappedServiceStub(const std::string& path, RR_SHARED_PTR<ServiceEntryDefinition> type, RR_SHARED_PTR<RobotRaconteur::ClientContext> c)
		: RobotRaconteur::ServiceStub(path,c)
	{
		RR_objecttype=type;
		//this->RR_Director=0;
		this->objectheapid=0;
#ifdef RR_PYTHON
		pystub=NULL;
		//DIRECTOR_CALL2(Py_XINCREF(pystub));
#endif 
	}

	void WrappedServiceStub::RRInitStub()
	{
		for (std::vector<RR_SHARED_PTR<MemberDefinition> >::iterator e=RR_objecttype->Members.begin(); e!=RR_objecttype->Members.end(); ++e)
		{
			RR_SHARED_PTR<PipeDefinition> p=boost::dynamic_pointer_cast<PipeDefinition>(*e);
			if (p)
			{
				bool unreliable=p->IsUnreliable();
				MemberDefinition_Direction direction = p->Direction();
				
				RR_SHARED_PTR<WrappedPipeClient> c=RR_MAKE_SHARED<WrappedPipeClient>(p->Name,shared_from_this(),p->Type, unreliable, direction);
				pipes.insert(std::make_pair(p->Name,c));				

			}

			RR_SHARED_PTR<WireDefinition> w=boost::dynamic_pointer_cast<WireDefinition>(*e);
			if (w)
			{
				MemberDefinition_Direction direction = w->Direction();
				RR_SHARED_PTR<WrappedWireClient> c=RR_MAKE_SHARED<WrappedWireClient>(w->Name,shared_from_this(),w->Type,direction);
				wires.insert(std::make_pair(w->Name,c));

			}

			RR_SHARED_PTR<MemoryDefinition> m=boost::dynamic_pointer_cast<MemoryDefinition>(*e);
			if (m)
			{
				if (IsTypeNumeric(m->Type->Type))
				{
					MemberDefinition_Direction direction = m->Direction();
					if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
					{
						RR_SHARED_PTR<ArrayMemoryBase> o;
						switch (m->Type->Type)
						{
						case DataTypes_double_t:
							o = RR_MAKE_SHARED<ArrayMemoryClient<double> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_single_t:
							o = RR_MAKE_SHARED<ArrayMemoryClient<float> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int8_t:
							o = RR_MAKE_SHARED<ArrayMemoryClient<int8_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint8_t:
							o = RR_MAKE_SHARED<ArrayMemoryClient<uint8_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int16_t:
							o = RR_MAKE_SHARED<ArrayMemoryClient<int16_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint16_t:
							o = RR_MAKE_SHARED<ArrayMemoryClient<uint16_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int32_t:
							o = RR_MAKE_SHARED<ArrayMemoryClient<int32_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint32_t:
							o = RR_MAKE_SHARED<ArrayMemoryClient<uint32_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int64_t:
							o = RR_MAKE_SHARED<ArrayMemoryClient<int64_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint64_t:
							o = RR_MAKE_SHARED<ArrayMemoryClient<uint64_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_cdouble_t:
							o = RR_MAKE_SHARED<ArrayMemoryClient<cdouble> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_csingle_t:
							o = RR_MAKE_SHARED<ArrayMemoryClient<cfloat> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_bool_t:
							o = RR_MAKE_SHARED<ArrayMemoryClient<rr_bool> >(m->Name, shared_from_this(), direction);
							break;
						default:
							throw InvalidArgumentException("Invalid memory data type");

						}
						arraymemories.insert(std::make_pair(m->Name, o));

					}
					else
					{
						RR_SHARED_PTR<MultiDimArrayMemoryBase> o;
						switch (m->Type->Type)
						{
						case DataTypes_double_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<double> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_single_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<float> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int8_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<int8_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint8_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<uint8_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int16_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<int16_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint16_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<uint16_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int32_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<int32_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint32_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<uint32_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int64_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<int64_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint64_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<uint64_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_cdouble_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<cdouble> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_csingle_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<cfloat> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_bool_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<rr_bool> >(m->Name, shared_from_this(), direction);
							break;
						default:
							throw InvalidArgumentException("Invalid memory data type");

						}
						multidimarraymemories.insert(std::make_pair(m->Name, o));
					}
				}
				else
				{
					MemberDefinition_Direction direction = m->Direction();
					std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs;
					
					RR_SHARED_PTR<ServiceEntryDefinition> service_entry = rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType());
					if (service_entry->RRDataType() == DataTypes_pod_t)
					{
						size_t element_size = EstimatePodPackedElementSize(service_entry, other_defs, RRGetNode());
						if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
						{
							RR_SHARED_PTR<WrappedPodArrayMemoryClient> o = RR_MAKE_SHARED<WrappedPodArrayMemoryClient>(m->Name, shared_from_this(), element_size, direction);
							pod_arraymemories.insert(std::make_pair(m->Name, o));
						}
						else
						{
							RR_SHARED_PTR<WrappedPodMultiDimArrayMemoryClient> o = RR_MAKE_SHARED<WrappedPodMultiDimArrayMemoryClient>(m->Name, shared_from_this(), element_size, direction);
							pod_multidimarraymemories.insert(std::make_pair(m->Name, o));
						}
					}
					else
					{
						boost::tuple<DataTypes, size_t> t4 = GetNamedArrayElementTypeAndCount(service_entry);
						size_t element_size = t4.get<1>();
						if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
						{
							RR_SHARED_PTR<WrappedNamedArrayMemoryClient> o = RR_MAKE_SHARED<WrappedNamedArrayMemoryClient>(m->Name, shared_from_this(), element_size, direction);
							namedarray_arraymemories.insert(std::make_pair(m->Name, o));
						}
						else
						{
							RR_SHARED_PTR<WrappedNamedMultiDimArrayMemoryClient> o = RR_MAKE_SHARED<WrappedNamedMultiDimArrayMemoryClient>(m->Name, shared_from_this(), element_size, direction);
							namedarray_multidimarraymemories.insert(std::make_pair(m->Name, o));
						}
					}
				}
			}


		}

	}

	RR_INTRUSIVE_PTR<MessageElement> WrappedServiceStub::PropertyGet(const std::string& PropertyName)
	{
		RR_INTRUSIVE_PTR<MessageEntry> req=CreateMessageEntry(MessageEntryType_PropertyGetReq,PropertyName);
		RR_INTRUSIVE_PTR<MessageEntry> res=ProcessRequest(req);
		return res->FindElement("value");
	}

	void WrappedServiceStub::PropertySet(const std::string& PropertyName, RR_INTRUSIVE_PTR<MessageElement> value)
	{
		RR_INTRUSIVE_PTR<MessageEntry> req=CreateMessageEntry(MessageEntryType_PropertySetReq, PropertyName);
		value->ElementName="value";
		req->AddElement(value);
		RR_INTRUSIVE_PTR<MessageEntry> res=ProcessRequest(req);		
	}

	RR_INTRUSIVE_PTR<MessageElement> WrappedServiceStub::FunctionCall(const std::string& FunctionName, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& args)
	{
		RR_INTRUSIVE_PTR<MessageEntry> req=CreateMessageEntry(MessageEntryType_FunctionCallReq, FunctionName);
		req->elements=args;
		RR_INTRUSIVE_PTR<MessageEntry> res=ProcessRequest(req);
		return res->FindElement("return");
	}

	RR_SHARED_PTR<WrappedGeneratorClient> WrappedServiceStub::GeneratorFunctionCall(const std::string& FunctionName, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& args)
	{
		RR_INTRUSIVE_PTR<MessageEntry> req = CreateMessageEntry(MessageEntryType_FunctionCallReq, FunctionName);
		req->elements = args;
		RR_INTRUSIVE_PTR<MessageEntry> res = ProcessRequest(req);
		return RR_MAKE_SHARED<WrappedGeneratorClient>(FunctionName, RRArrayToScalar(res->FindElement("index")->CastData<RRArray<int32_t> >()),shared_from_this());
	}

	void WrappedServiceStub::async_PropertyGet(const std::string& PropertyName, int32_t timeout, AsyncRequestDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncRequestDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncRequestDirector>, _1, id));
		RR_INTRUSIVE_PTR<MessageEntry> req=CreateMessageEntry(MessageEntryType_PropertyGetReq,PropertyName);
		AsyncProcessRequest(req,boost::bind(&WrappedServiceStub::async_PropertyGet_handler,rr_cast<WrappedServiceStub>(shared_from_this()),_1,_2,sphandler),timeout);

	}
	void WrappedServiceStub::async_PropertySet(const std::string& PropertyName,  RR_INTRUSIVE_PTR<MessageElement> value, int32_t timeout, AsyncRequestDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncRequestDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncRequestDirector>, _1, id));
		RR_INTRUSIVE_PTR<MessageEntry> req=CreateMessageEntry(MessageEntryType_PropertySetReq, PropertyName);
		value->ElementName="value";
		req->AddElement(value);
		AsyncProcessRequest(req,boost::bind(&WrappedServiceStub::async_PropertySet_handler,rr_cast<WrappedServiceStub>(shared_from_this()),_1,_2,sphandler),timeout);
	}
	void WrappedServiceStub::async_FunctionCall(const std::string& FunctionName, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& args, int32_t timeout, AsyncRequestDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncRequestDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncRequestDirector>, _1, id));
		RR_INTRUSIVE_PTR<MessageEntry> req=CreateMessageEntry(MessageEntryType_FunctionCallReq, FunctionName);
		req->elements=args;
		AsyncProcessRequest(req,boost::bind(&WrappedServiceStub::async_FunctionCall_handler,rr_cast<WrappedServiceStub>(shared_from_this()),_1,_2,sphandler),timeout);
	}
	
	void WrappedServiceStub::async_GeneratorFunctionCall(const std::string& FunctionName, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& args, int32_t timeout, AsyncGeneratorClientReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncGeneratorClientReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncGeneratorClientReturnDirector>, _1, id));
		RR_INTRUSIVE_PTR<MessageEntry> req = CreateMessageEntry(MessageEntryType_FunctionCallReq, FunctionName);
		req->elements = args;
		AsyncProcessRequest(req, boost::bind(&WrappedServiceStub::async_GeneratorFunctionCall_handler, rr_cast<WrappedServiceStub>(shared_from_this()), FunctionName, _1, _2, sphandler), timeout);
	}

	void WrappedServiceStub::async_PropertyGet_handler( RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncRequestDirector> handler)
	{
		
		if (err)
		{

		DIRECTOR_CALL2(handler->handler(RR_INTRUSIVE_PTR<MessageElement>(),err->ErrorCode,err->Error,err->Message));
		
		return;
		}
		if (m->Error != RobotRaconteur::MessageErrorType_None)
		{
			DIRECTOR_CALL2(handler->handler(RR_INTRUSIVE_PTR<MessageElement>(),m->Error,m->FindElement("errorname")->CastDataToString(),m->FindElement("errorstring")->CastDataToString()));
			return;
		}
		RR_INTRUSIVE_PTR<MessageElement> ret=m->FindElement("value");
		DIRECTOR_CALL2(handler->handler(ret,0,"",""));
	}

	void WrappedServiceStub::async_PropertySet_handler(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncRequestDirector> handler)
	{
		
		if (err)
		{

		DIRECTOR_CALL2(handler->handler(RR_INTRUSIVE_PTR<MessageElement>(),err->ErrorCode,err->Error,err->Message));
		
		return;
		}
		if (m->Error != RobotRaconteur::MessageErrorType_None)
		{
			DIRECTOR_CALL2(handler->handler(RR_INTRUSIVE_PTR<MessageElement>(),m->Error,m->FindElement("errorname")->CastDataToString(),m->FindElement("errorstring")->CastDataToString()));
			return;
		}

		DIRECTOR_CALL2(handler->handler(RR_INTRUSIVE_PTR<MessageElement>(),0,"",""));
	}

	void WrappedServiceStub::async_FunctionCall_handler(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncRequestDirector> handler)
	{
		
		if (err)
		{

		DIRECTOR_CALL2(handler->handler(RR_INTRUSIVE_PTR<MessageElement>(),err->ErrorCode,err->Error,err->Message));
		
		return;
		}
		if (m->Error != RobotRaconteur::MessageErrorType_None)
		{
			DIRECTOR_CALL2(handler->handler(RR_INTRUSIVE_PTR<MessageElement>(),m->Error,m->FindElement("errorname")->CastDataToString(),m->FindElement("errorstring")->CastDataToString()));
			return;
		}

		RR_INTRUSIVE_PTR<MessageElement> ret;

		try
		{
			ret=m->FindElement("return");
		}
		catch (std::exception&) {}

		DIRECTOR_CALL2(handler->handler(ret,0,"",""));
	}

	void WrappedServiceStub::async_GeneratorFunctionCall_handler(const std::string& FunctionName, RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncGeneratorClientReturnDirector> handler)
	{
		if (err)
		{
			DIRECTOR_CALL2(handler->handler(RR_SHARED_PTR<WrappedGeneratorClient>(), err->ErrorCode, err->Error, err->Message));
			return;
		}
		if (m->Error != RobotRaconteur::MessageErrorType_None)
		{
			DIRECTOR_CALL2(handler->handler(RR_SHARED_PTR<WrappedGeneratorClient>(), m->Error, m->FindElement("errorname")->CastDataToString(), m->FindElement("errorstring")->CastDataToString()));
			return;
		}

		RR_INTRUSIVE_PTR<MessageElement> ret;

		try
		{
			ret = m->FindElement("return");
		}
		catch (std::exception&) {}

		RR_SHARED_PTR<WrappedGeneratorClient> gen_ret = RR_MAKE_SHARED<WrappedGeneratorClient>(FunctionName, RRArrayToScalar(m->FindElement("index")->CastData<RRArray<int32_t> >()), shared_from_this());

		DIRECTOR_CALL2(handler->handler(gen_ret, 0, "", ""));
	}

	void WrappedServiceStub::async_FindObjRef(const std::string& path, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncStubReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncStubReturnDirector>, _1, id));
		AsyncFindObjRef(path,boost::bind(&WrappedServiceStub::async_FindObjRef_handler,rr_cast<WrappedServiceStub>(shared_from_this()),_1,_2,sphandler),timeout);
	}

	void WrappedServiceStub::async_FindObjRef(const std::string& path, const std::string& ind, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncStubReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncStubReturnDirector>, _1, id));
		AsyncFindObjRef(path,ind,boost::bind(&WrappedServiceStub::async_FindObjRef_handler,rr_cast<WrappedServiceStub>(shared_from_this()),_1,_2,sphandler),timeout);
	}

	void WrappedServiceStub::async_FindObjRefTyped(const std::string& path, const std::string& type, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncStubReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncStubReturnDirector>, _1, id));
		AsyncFindObjRefTyped(path,type,boost::bind(&WrappedServiceStub::async_FindObjRef_handler,rr_cast<WrappedServiceStub>(shared_from_this()),_1,_2,sphandler),timeout);
	}

	void WrappedServiceStub::async_FindObjRefTyped(const std::string& path, const std::string& ind, const std::string& type, int32_t timeout, AsyncStubReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncStubReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncStubReturnDirector>, _1, id));
		AsyncFindObjRefTyped(path,ind,type, boost::bind(&WrappedServiceStub::async_FindObjRef_handler,rr_cast<WrappedServiceStub>(shared_from_this()),_1,_2,sphandler),timeout);
	}

	void WrappedServiceStub::async_FindObjRef_handler(RR_SHARED_PTR<RRObject> stub, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncStubReturnDirector> handler)
	{
		if (err)
		{
			DIRECTOR_CALL2(handler->handler(RR_SHARED_PTR<WrappedServiceStub>(),err->ErrorCode,err->Error,err->Message));
			return;
		}

		RR_SHARED_PTR<WrappedServiceStub> stub2=rr_cast<WrappedServiceStub>(stub);


		DIRECTOR_CALL2(handler->handler(stub2,0,"",""));
	}
		


	void WrappedServiceStub::DispatchEvent(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m)
	{
		DIRECTOR_CALL3(WrappedServiceStubDirector,RR_Director2->DispatchEvent(m->MemberName,m->elements));			
		
	}

	void WrappedServiceStub::DispatchPipeMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m)
	{
		std::map<std::string, RR_SHARED_PTR<WrappedPipeClient> >::iterator e = pipes.find(m->MemberName);
		if (e == pipes.end()) throw MemberNotFoundException("Pipe Member Not Found");
		e->second->PipePacketReceived(m);		
	}
	
	void WrappedServiceStub::DispatchWireMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m)
	{
		std::map<std::string, RR_SHARED_PTR<WrappedWireClient> >::iterator e = wires.find(m->MemberName);
		if (e == wires.end()) throw MemberNotFoundException("Pipe Member Not Found");
		e->second->WirePacketReceived(m);
	}

	RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> WrappedServiceStub::CallbackCall(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m)
	{		
		RR_SHARED_PTR<CallbackDefinition> d;

		for (std::vector<RR_SHARED_PTR<MemberDefinition> >::iterator e=RR_objecttype->Members.begin(); e!=RR_objecttype->Members.end(); ++e)
		{
			if ((*e)->Name==m->MemberName)
				d=boost::dynamic_pointer_cast<CallbackDefinition>(*e);

		}

		if (!d) throw MemberNotFoundException("Member not found");

		RR_INTRUSIVE_PTR<MessageEntry> req=CreateMessageEntry(MessageEntryType_CallbackCallRet,m->MemberName);
		RR_INTRUSIVE_PTR<MessageElement> mres;
		try
		{
			DIRECTOR_CALL(WrappedServiceStubDirector,mres=RR_Director2->CallbackCall(m->MemberName,m->elements));
		}
		catch (std::exception&)
		{
			throw;
		}
		catch (...)
		{

			throw UnknownException("RobotRaconteur.UnknownException", "Error occured in callback");

		}
		
		if (!mres)
		{			
			throw OperationFailedException("Exception occured in callback");			
		}
		
		RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> res=CreateMessageEntry( RobotRaconteur::MessageEntryType_CallbackCallRet,m->MemberName);
		res->ServicePath=m->ServicePath;
		res->RequestID=m->RequestID;
		if (d->ReturnType->Type != DataTypes_void_t)
		{
			mres->ElementName="return";
			res->AddElement(mres);
		}
		else
		{
			res->AddElement("return",RobotRaconteur::ScalarToRRArray<int32_t>(0));
		}

		return res;		
	}

	RR_SHARED_PTR<RobotRaconteur::WrappedPipeClient> WrappedServiceStub::GetPipe(const std::string& membername)
	{
		std::map<std::string, RR_SHARED_PTR<WrappedPipeClient> >::iterator e = pipes.find(membername);
		if (e == pipes.end()) throw MemberNotFoundException("Pipe Member Not Found");
		return e->second;
	}

	RR_SHARED_PTR<RobotRaconteur::ArrayMemoryBase> WrappedServiceStub::GetArrayMemory(const std::string& membername)
	{
		std::map<std::string, RR_SHARED_PTR<ArrayMemoryBase> >::iterator e = arraymemories.find(membername);
		if (e == arraymemories.end()) throw MemberNotFoundException("Member Not Found");
		return e->second;
	}

	RR_SHARED_PTR<RobotRaconteur::WrappedPodArrayMemoryClient> WrappedServiceStub::GetPodArrayMemory(const std::string& membername)
	{
		std::map<std::string, RR_SHARED_PTR<WrappedPodArrayMemoryClient> >::iterator e = pod_arraymemories.find(membername);
		if (e == pod_arraymemories.end()) throw MemberNotFoundException("Member Not Found");
		return e->second;
	}

	RR_SHARED_PTR<RobotRaconteur::WrappedNamedArrayMemoryClient> WrappedServiceStub::GetNamedArrayMemory(const std::string& membername)
	{
		std::map<std::string, RR_SHARED_PTR<WrappedNamedArrayMemoryClient> >::iterator e = namedarray_arraymemories.find(membername);
		if (e == namedarray_arraymemories.end()) throw MemberNotFoundException("Member Not Found");
		return e->second;
	}


	RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemoryBase> WrappedServiceStub::GetMultiDimArrayMemory(const std::string& membername)
	{
		std::map<std::string, RR_SHARED_PTR<MultiDimArrayMemoryBase> >::iterator e = multidimarraymemories.find(membername);
		if (e == multidimarraymemories.end()) throw MemberNotFoundException("Member Not Found");
		return e->second;
	}

	RR_SHARED_PTR<RobotRaconteur::WrappedPodMultiDimArrayMemoryClient> WrappedServiceStub::GetPodMultiDimArrayMemory(const std::string& membername)
	{
		std::map<std::string, RR_SHARED_PTR<WrappedPodMultiDimArrayMemoryClient> >::iterator e = pod_multidimarraymemories.find(membername);
		if (e == pod_multidimarraymemories.end()) throw MemberNotFoundException("Member Not Found");
		return e->second;
	}

	RR_SHARED_PTR<RobotRaconteur::WrappedNamedMultiDimArrayMemoryClient> WrappedServiceStub::GetNamedMultiDimArrayMemory(const std::string& membername)
	{
		std::map<std::string, RR_SHARED_PTR<WrappedNamedMultiDimArrayMemoryClient> >::iterator e = namedarray_multidimarraymemories.find(membername);
		if (e == namedarray_multidimarraymemories.end()) throw MemberNotFoundException("Member Not Found");
		return e->second;
	}

	RR_SHARED_PTR<RobotRaconteur::WrappedWireClient> WrappedServiceStub::GetWire(const std::string& membername)
	{
		std::map<std::string, RR_SHARED_PTR<WrappedWireClient> >::iterator e = wires.find(membername);
		if (e == wires.end()) throw MemberNotFoundException("Wire Member Not Found");
		return e->second;
	}

	RR_SHARED_PTR<PipeClientBase> WrappedServiceStub::RRGetPipeClient(const std::string& membername)
	{
		return GetPipe(membername);
	}

	RR_SHARED_PTR<WireClientBase> WrappedServiceStub::RRGetWireClient(const std::string& membername)
	{
		return GetWire(membername);
	}

	void WrappedServiceStub::RRClose()
	{
		//DIRECTOR_CALL2(DIRECTOR_DELETE( RR_Director));

		boost::unique_lock<boost::shared_mutex> lock2(RR_Director_lock);

		for (std::map<std::string, RR_SHARED_PTR<WrappedPipeClient> >::iterator e=pipes.begin(); e!=pipes.end(); ++e)
		{
			e->second->Shutdown();
		}

		for (std::map<std::string, RR_SHARED_PTR<WrappedWireClient> >::iterator e=wires.begin(); e!=wires.end(); ++e)
		{
			e->second->Shutdown();
		}


		//TODO: fill this in
		ServiceStub::RRClose();

		RR_Director.reset();

#ifdef RR_PYTHON

		boost::mutex::scoped_lock lock(pystub_lock);
		if (pystub!=NULL)
		{
			DIRECTOR_CALL2(Py_XDECREF(pystub));
			pystub=NULL;
			
		}
#else
		//RR_Director=NULL;
#endif

	}

	std::string WrappedServiceStub::RRType()
	{
		return RR_objecttype->Name;
	}

	WrappedServiceStub::~WrappedServiceStub()
	{
		//DIRECTOR_CALL2(DIRECTOR_DELETE( this->RR_Director));
#ifdef RR_PYTHON
		//DIRECTOR_CALL2(Py_XDECREF(pystub));
#endif

	}

	/*WrappedServiceStubDirector* WrappedServiceStub::GetRRDirector()
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		return RR_Director;
	}*/

	void WrappedServiceStub::SetRRDirector(WrappedServiceStubDirector* director, int32_t id)
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		objectheapid=id;
		this->RR_Director.reset(director, boost::bind(&ReleaseDirector<WrappedServiceStubDirector>, _1, id));
	}
	
	int WrappedServiceStub::GetObjectHeapID()
	{
		return objectheapid;
	}

	//Wrapped Pipe Endpoint

	uint32_t WrappedPipeEndpoint::SendPacket(RR_INTRUSIVE_PTR<MessageElement> packet)
	{
		RR_SHARED_PTR<detail::sync_async_handler<uint32_t> > t=RR_MAKE_SHARED<detail::sync_async_handler<uint32_t> >();
		AsyncSendPacketBase(rr_cast<RRValue>(packet),boost::bind(&WrappedPipeEndpoint::send_handler,_1,_2,boost::protect(boost::bind(&detail::sync_async_handler<uint32_t>::operator(),t,_1,_2))));
		return *t->end();
	}

	void WrappedPipeEndpoint::AsyncSendPacket(RR_INTRUSIVE_PTR<MessageElement> packet, AsyncUInt32ReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncUInt32ReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncUInt32ReturnDirector>, _1, id));
		AsyncSendPacketBase(rr_cast<RRValue>(packet),boost::bind(&WrappedPipeEndpoint::AsyncSendPacket_handler,rr_cast<WrappedPipeEndpoint>(shared_from_this()),_1,_2,sphandler));
	}

	void WrappedPipeEndpoint::AsyncSendPacket_handler(uint32_t id, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncUInt32ReturnDirector> handler)
	{
		if (err)
		{
			DIRECTOR_CALL2(handler->handler(0,err->ErrorCode,err->Error,err->Message));
			return;
		}

		DIRECTOR_CALL2(handler->handler(id,0,"",""));
	}
	

	RR_INTRUSIVE_PTR<MessageElement> WrappedPipeEndpoint::ReceivePacket()
	{
		return rr_cast<MessageElement>(ReceivePacketBase());
	}

	RR_INTRUSIVE_PTR<MessageElement> WrappedPipeEndpoint::PeekNextPacket()
	{
		return rr_cast<MessageElement>(PeekPacketBase());
	}

	RR_INTRUSIVE_PTR<MessageElement> WrappedPipeEndpoint::ReceivePacketWait(int32_t timeout)
	{
		return rr_cast<MessageElement>(ReceivePacketBaseWait(timeout));
	}

	RR_INTRUSIVE_PTR<MessageElement> WrappedPipeEndpoint::PeekNextPacketWait(int32_t timeout)
	{
		return rr_cast<MessageElement>(PeekPacketBaseWait(timeout));
	}

	bool WrappedPipeEndpoint::TryReceivePacketWait(RR_INTRUSIVE_PTR<MessageElement>& packet, int32_t timeout, bool peek)
	{
		RR_INTRUSIVE_PTR<RRValue> o;
		if (!TryReceivePacketBaseWait(o, timeout, peek))
		{
			return false;
		}

		packet = rr_cast<MessageElement>(o);
		return true;
	}


	WrappedPipeEndpoint::WrappedPipeEndpoint(RR_SHARED_PTR<PipeBase> parent, int32_t index, uint32_t endpoint, RR_SHARED_PTR<TypeDefinition> type, bool unreliable, MemberDefinition_Direction direction, bool message3)
		: PipeEndpointBase(parent,index,endpoint,unreliable,direction,message3) {
		this->Type=type;
		//this->RR_Director=0;
		//this->objectheapid=0;

	}

	void WrappedPipeEndpoint::fire_PipeEndpointClosedCallback()
	{
		try
		{
			boost::shared_lock<boost::shared_mutex> lock(RR_Director_lock);

			RR_SHARED_PTR<WrappedPipeBroadcaster> b = broadcaster.lock();
			if (b)
			{
				RR_SHARED_PTR<detail::PipeBroadcasterBase_connected_endpoint> cep = broadcaster_cep.lock();
				if (cep)
				{
					b->EndpointClosedBase(cep);
				}
			}
		}
		catch (std::exception&) {}

		
		DIRECTOR_CALL3(WrappedPipeEndpointDirector,RR_Director2->PipeEndpointClosedCallback());
			

	}

	void WrappedPipeEndpoint::fire_PacketReceivedEvent()
	{
		DIRECTOR_CALL3(WrappedPipeEndpointDirector,RR_Director2->PacketReceivedEvent());
				
	}

	void WrappedPipeEndpoint::fire_PacketAckReceivedEvent(uint32_t packetnum)
	{
		try
		{
			boost::shared_lock<boost::shared_mutex> lock(RR_Director_lock);

			RR_SHARED_PTR<WrappedPipeBroadcaster> b = broadcaster.lock();
			if (b)
			{
				RR_SHARED_PTR<detail::PipeBroadcasterBase_connected_endpoint> cep = broadcaster_cep.lock();
				if (cep)
				{
					b->PacketAckReceivedBase(cep, packetnum);
				}
			}
		}
		catch (std::exception&) {}

			

		DIRECTOR_CALL3(WrappedPipeEndpointDirector,RR_Director2->PacketAckReceivedEvent(packetnum));

		
	}

	//WrappedPipeClient

	/*boost::function<void(RR_SHARED_PTR<WrappedPipeEndpoint>)> WrappedPipeClient::GetPipeConnectCallback()
	{
		throw InvalidOperationException("Not valid for client");
	}
		
	void WrappedPipeClient::SetPipeConnectCallback(boost::function<void(RR_SHARED_PTR<WrappedPipeEndpoint>)> function)
	{
		throw InvalidOperationException("Not valid for client");
	}*/
	
	/*WrappedPipeEndpointDirector* WrappedPipeEndpoint::GetRRDirector()
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		return RR_Director;
	}*/
	void WrappedPipeEndpoint::SetRRDirector(WrappedPipeEndpointDirector* director, int32_t id)
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		this->RR_Director.reset(director, boost::bind(&ReleaseDirector<WrappedPipeEndpointDirector>, _1, id));
	}

	RR_SHARED_PTR<WrappedServiceStub> WrappedPipeEndpoint::GetStub()
	{
		RR_SHARED_PTR<PipeBase> p = parent.lock();
		if (!p) return RR_SHARED_PTR<WrappedServiceStub>();
		RR_SHARED_PTR<PipeClientBase> p1=RR_DYNAMIC_POINTER_CAST<PipeClientBase>(p);
		if (!p1) return RR_SHARED_PTR<WrappedServiceStub>();
		return RR_DYNAMIC_POINTER_CAST<WrappedServiceStub>(p1->GetStub());
	}

	void WrappedPipeEndpoint::Close()
	{
		{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		RR_Director.reset();
		}

		PipeEndpointBase::Close();

	}

	void WrappedPipeEndpoint::AsyncClose(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncVoidReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncVoidReturnDirector>, _1, id));

		{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		RR_Director.reset();
		}

		PipeEndpointBase::AsyncClose(boost::bind(&WrappedPipeEndpoint::AsyncClose_handler,rr_cast<WrappedPipeEndpoint>(shared_from_this()),_1,sphandler),timeout);


	}

	void WrappedPipeEndpoint::AsyncClose_handler(RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncVoidReturnDirector> handler)
	{
		if (err)
		{
			DIRECTOR_CALL2(handler->handler(err->ErrorCode,err->Error,err->Message));
			return;
		}

		DIRECTOR_CALL2(handler->handler(0,"",""));
	}


	WrappedPipeEndpoint::~WrappedPipeEndpoint()
	{
		//DIRECTOR_CALL2(DIRECTOR_DELETE(RR_Director));
	}

	void WrappedPipeEndpoint::SetPipeBroadcaster(RR_SHARED_PTR<WrappedPipeBroadcaster> broadcaster, RR_SHARED_PTR<detail::PipeBroadcasterBase_connected_endpoint> cep)
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		RR_SHARED_PTR<WrappedPipeBroadcaster> broadcaster1 = this->broadcaster.lock();
		if (broadcaster1) throw InvalidOperationException("PipeBroadcaster already set");
		
		this->broadcaster = broadcaster;
		this->broadcaster_cep = cep;
	}

	RR_SHARED_PTR<WrappedPipeEndpoint> WrappedPipeClient::Connect(int32_t index)
	{
		RR_SHARED_PTR<detail::sync_async_handler<PipeEndpointBase > > t=RR_MAKE_SHARED<detail::sync_async_handler<PipeEndpointBase> >();
		AsyncConnect_internal(index,boost::bind(&detail::sync_async_handler<PipeEndpointBase >::operator(),t,_1,_2),RobotRaconteurNode::s()->GetRequestTimeout());
		return boost::dynamic_pointer_cast<WrappedPipeEndpoint>(t->end()); 
	}

	void WrappedPipeClient::AsyncConnect(int32_t index, int32_t timeout, AsyncPipeEndpointReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncPipeEndpointReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncPipeEndpointReturnDirector>, _1, id));
		AsyncConnect_internal(index,boost::bind(&WrappedPipeClient::AsyncConnect_handler,rr_cast<WrappedPipeClient>(shared_from_this()),_1,_2,sphandler),timeout);
	}

	void WrappedPipeClient::AsyncConnect_handler(RR_SHARED_PTR<PipeEndpointBase> ep, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncPipeEndpointReturnDirector> handler)
	{
		if (err)
		{
			DIRECTOR_CALL2(handler->handler(RR_SHARED_PTR<WrappedPipeEndpoint>(),err->ErrorCode,err->Error,err->Message));
			return;
		}

		RR_SHARED_PTR<WrappedPipeEndpoint> ep2=boost::dynamic_pointer_cast<WrappedPipeEndpoint>(ep);

		DIRECTOR_CALL2(handler->handler(ep2,0,"",""));

	}

	WrappedPipeClient::WrappedPipeClient(const std::string& name, RR_SHARED_PTR<ServiceStub> stub, RR_SHARED_PTR<TypeDefinition> Type, bool unreliable, MemberDefinition_Direction direction) : PipeClientBase(name,stub,unreliable,direction)
	{
		this->Type=Type;
		this->rawelements=true;
	}	
	
	RR_SHARED_PTR<PipeEndpointBase> WrappedPipeClient::CreateNewPipeEndpoint(int32_t index, bool unreliable, MemberDefinition_Direction direction, bool message3)
	{
		return RR_MAKE_SHARED<WrappedPipeEndpoint>(rr_cast<WrappedPipeClient>(shared_from_this()),index,0,Type,unreliable,direction,message3);
	}

	//WrappedPipeServerConnectDirector
	void WrappedPipeServerConnectDirector::PipeConnectCallbackFire(RR_SHARED_PTR<PipeEndpointBase> e)
	{
		boost::shared_ptr<WrappedPipeEndpoint> e2 = rr_cast<WrappedPipeEndpoint>(e);
		DIRECTOR_CALL2(PipeConnectCallback(e2));		
	}

	//WrappedPipeServer
	WrappedPipeServer::WrappedPipeServer(const std::string& name, RR_SHARED_PTR<ServiceSkel> skel, RR_SHARED_PTR<TypeDefinition> Type,bool unreliable,MemberDefinition_Direction direction): PipeServerBase(name,skel,unreliable,direction)
	{
		this->Type=Type;
		this->rawelements=true;
		//this->objectheapid=0;
		//this->RR_Director=0;
	}

	WrappedPipeServer::~WrappedPipeServer()
	{
		//DIRECTOR_CALL2(DIRECTOR_DELETE(RR_Director));
	}

	void WrappedPipeServer::Shutdown()
	{
		PipeServerBase::Shutdown();

		boost::mutex::scoped_lock lock(callback_lock);		
		callback.clear();		
	}

	RR_SHARED_PTR<PipeEndpointBase> WrappedPipeServer::CreateNewPipeEndpoint(int32_t index, uint32_t endpoint,bool unreliable, MemberDefinition_Direction direction,bool message3)
	{
		return RR_MAKE_SHARED<WrappedPipeEndpoint>(rr_cast<WrappedPipeServer>(shared_from_this()),index,endpoint,Type,unreliable,direction,message3);
	}

	void WrappedPipeServer::fire_PipeConnectCallback(RR_SHARED_PTR<PipeEndpointBase> e)
	{
		boost::function<void(RR_SHARED_PTR<PipeEndpointBase >)> callback1;
		{
			boost::mutex::scoped_lock lock(callback_lock);
			callback1 = callback;
		}

		if (callback1)
		{
			callback1(e);
		}
	}

	/*WrappedPipeServerDirector* WrappedPipeServer::GetRRDirector()
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		return RR_Director;
	}*/

	void WrappedPipeServer::SetWrappedPipeConnectCallback(WrappedPipeServerConnectDirector* director, int32_t id)
	{
		boost::mutex::scoped_lock lock(callback_lock);
		
		if (!director)
		{
			callback.clear();
			return;
		}

		RR_SHARED_PTR<WrappedPipeServerConnectDirector> director1(director, boost::bind(&ReleaseDirector<WrappedPipeServerConnectDirector>, _1, id));

		callback = boost::bind(&WrappedPipeServerConnectDirector::PipeConnectCallbackFire, director1, _1);
	}

	boost::function<void(RR_SHARED_PTR<PipeEndpointBase >)> WrappedPipeServer::GetPipeConnectCallback()
	{
		boost::mutex::scoped_lock lock(callback_lock);
		return callback;
	}

	void WrappedPipeServer::SetPipeConnectCallback(boost::function<void(RR_SHARED_PTR<PipeEndpointBase >)> function)
	{
		boost::mutex::scoped_lock lock(callback_lock);
		callback = function;
	}

	//WrappedPipeBroadcaster

	bool WrappedPipeBroadcasterPredicateDirector::CallPredicate(uint32_t client_endpoint, int32_t index)
	{
		bool res;
		DIRECTOR_CALL2(res = Predicate(client_endpoint,index));
		return res;
	}

	void WrappedPipeBroadcaster::Init(RR_SHARED_PTR<WrappedPipeServer> pipe, int32_t maximum_backlog)
	{
		copy_element = true;
		InitBase(pipe, maximum_backlog);
	}

	void WrappedPipeBroadcaster::SendPacket(RR_INTRUSIVE_PTR<MessageElement> packet)
	{
		SendPacketBase(packet);		
	}

	void WrappedPipeBroadcaster::AsyncSendPacket(RR_INTRUSIVE_PTR<MessageElement> packet, AsyncVoidNoErrReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncVoidNoErrReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncVoidNoErrReturnDirector>, _1, id));

		AsyncSendPacketBase(packet, boost::bind(&WrappedPipeBroadcaster::AsyncSendPacket_handler, RR_STATIC_POINTER_CAST<WrappedPipeBroadcaster>(shared_from_this()), sphandler));				
	}

	void WrappedPipeBroadcaster::AsyncSendPacket_handler(RR_SHARED_PTR<AsyncVoidNoErrReturnDirector> handler)
	{
		DIRECTOR_CALL2(handler->handler());
	}

	void WrappedPipeBroadcaster::AttachPipeServerEvents(RR_SHARED_PTR<PipeServerBase> p)
	{
		RR_SHARED_PTR<WrappedPipeServer> p_T = rr_cast<WrappedPipeServer>(p);
		p_T->SetPipeConnectCallback(boost::bind(&WrappedPipeBroadcaster::EndpointConnectedBase, shared_from_this(), _1));

	}

	void WrappedPipeBroadcaster::AttachPipeEndpointEvents(RR_SHARED_PTR<PipeEndpointBase> ep, RR_SHARED_PTR<detail::PipeBroadcasterBase_connected_endpoint> cep)
	{
		RR_SHARED_PTR<WrappedPipeEndpoint> ep_T = rr_cast<WrappedPipeEndpoint>(ep);
		ep_T->SetPipeBroadcaster(RR_STATIC_POINTER_CAST<WrappedPipeBroadcaster>(shared_from_this()), cep);
	}

	void WrappedPipeBroadcaster::SetPredicateDirector(WrappedPipeBroadcasterPredicateDirector* f, int32_t id)
	{		
		RR_SHARED_PTR<WrappedPipeBroadcasterPredicateDirector> f_sp(f, boost::bind(&ReleaseDirector<WrappedPipeBroadcasterPredicateDirector>, _1, id));
		this->SetPredicate(boost::bind(&WrappedPipeBroadcasterPredicateDirector::CallPredicate, f_sp, _2, _3));		
	}
	

	//WrappedWireConnection
	RR_INTRUSIVE_PTR<MessageElement> WrappedWireConnection::GetInValue()
	{
		return RRPrimUtil<RR_INTRUSIVE_PTR<MessageElement> >::PreUnpack(GetInValueBase());
	}

	RR_INTRUSIVE_PTR<MessageElement> WrappedWireConnection::GetOutValue()
	{
		return RRPrimUtil<RR_INTRUSIVE_PTR<MessageElement> >::PreUnpack(GetOutValueBase());
	}

	void WrappedWireConnection::SetOutValue(RR_INTRUSIVE_PTR<MessageElement> value)
	{
		SetOutValueBase(RRPrimUtil<RR_INTRUSIVE_PTR<MessageElement> >::PrePack(value));
	}

	WrappedWireConnection::WrappedWireConnection(RR_SHARED_PTR<WireBase> parent, uint32_t endpoint, RR_SHARED_PTR<TypeDefinition> Type, MemberDefinition_Direction direction, bool message3)
		: WireConnectionBase(parent,endpoint,direction,message3) 
	{
		this->Type=Type;
		//this->RR_Director=0;
		//this->objectheapid=0;
	
	}

	void WrappedWireConnection::fire_WireValueChanged(RR_INTRUSIVE_PTR<RRValue> value, TimeSpec time)
	{
		
		RR_SHARED_PTR<WrappedWireUnicastReceiver> r = unicast_receiver.lock();
		if (r)
		{
			r->ConnectionInValueChanged(RR_DYNAMIC_POINTER_CAST<WrappedWireConnection>(shared_from_this()),
				RR_DYNAMIC_POINTER_CAST<MessageElement>(value), time);
		}

		RR_INTRUSIVE_PTR<MessageElement> m=RRPrimUtil<RR_INTRUSIVE_PTR<MessageElement> >::PreUnpack(value);
		DIRECTOR_CALL3(WrappedWireConnectionDirector,RR_Director2->WireValueChanged(m,time));

		
	}

	void WrappedWireConnection::fire_WireClosedCallback()
	{
		try
		{
			boost::shared_lock<boost::shared_mutex> lock(RR_Director_lock);

			RR_SHARED_PTR<WrappedWireBroadcaster> b = broadcaster.lock();
			if (b)
			{
				RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> cep = broadcaster_cep.lock();
				if (cep)
				{
					b->ConnectionClosedBase(cep);
				}
			}
		}
		catch (std::exception&) {}

		try
		{
			boost::shared_lock<boost::shared_mutex> lock(RR_Director_lock);
			RR_SHARED_PTR<WrappedWireUnicastReceiver> r = unicast_receiver.lock();
			if (r)
			{
				r->ConnectionClosed(RR_DYNAMIC_POINTER_CAST<WrappedWireConnection>(shared_from_this()));
			}
		}
		catch (std::exception&) {}

		DIRECTOR_CALL3(WrappedWireConnectionDirector,RR_Director2->WireConnectionClosedCallback());
				
	}

	/*WrappedWireConnectionDirector* WrappedWireConnection::GetRRDirector()
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		return RR_Director;
	}*/

	void WrappedWireConnection::SetRRDirector(WrappedWireConnectionDirector* director, int32_t id)
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		this->RR_Director.reset(director, boost::bind(&ReleaseDirector<WrappedWireConnectionDirector>, _1, id));
	}

	RR_SHARED_PTR<WrappedServiceStub> WrappedWireConnection::GetStub()
	{
		RR_SHARED_PTR<WireBase> w = parent.lock();
		if (!w) return RR_SHARED_PTR<WrappedServiceStub>();
		RR_SHARED_PTR<WireClientBase> w2 = RR_DYNAMIC_POINTER_CAST<WireClientBase>(w);
		if (!w2) return RR_SHARED_PTR<WrappedServiceStub>();
		return RR_DYNAMIC_POINTER_CAST<WrappedServiceStub>(w2->GetStub());
	}

	WrappedWireConnection::~WrappedWireConnection()
	{
		//DIRECTOR_CALL2(DIRECTOR_DELETE(RR_Director));
	}

	void WrappedWireConnection::Close()
	{
		
		{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		
		RR_Director.reset();
		}

		WireConnectionBase::Close();
		
	}

	void WrappedWireConnection::AsyncClose(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncVoidReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncVoidReturnDirector>, _1, id));

		{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		RR_Director.reset();
		}

		WireConnectionBase::AsyncClose(boost::bind(&WrappedWireConnection::AsyncClose_handler,rr_cast<WrappedWireConnection>(shared_from_this()),_1,sphandler),timeout);


	}

	void WrappedWireConnection::AsyncClose_handler(RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncVoidReturnDirector> handler)
	{
		if (err)
		{
			DIRECTOR_CALL2(handler->handler(err->ErrorCode,err->Error,err->Message));
			return;
		}

		DIRECTOR_CALL2(handler->handler(0,"",""));
	}

	void WrappedWireConnection::SetWireBroadcaster(RR_SHARED_PTR<WrappedWireBroadcaster> broadcaster, RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> cep)
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		RR_SHARED_PTR<WrappedWireBroadcaster> broadcaster1 = this->broadcaster.lock();
		if (broadcaster1) throw InvalidOperationException("WireBroadcaster already set");

		this->broadcaster = broadcaster;
		this->broadcaster_cep = cep;
	}

	void WrappedWireConnection::SetWireUnicastReceiver(RR_SHARED_PTR<WrappedWireUnicastReceiver> receiver)
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		RR_SHARED_PTR<WrappedWireUnicastReceiver> receiver1 = this->unicast_receiver.lock();
		if (receiver1) throw InvalidOperationException("WireUnicastReceiver already set");

		this->unicast_receiver = receiver;		
	}

	bool WrappedWireConnection::TryGetInValue(RR_INTRUSIVE_PTR<MessageElement>& value, TimeSpec& ts)
	{
		RR_INTRUSIVE_PTR<RRValue> value1;
		if (!TryGetInValueBase(value1, ts))
		{
			return false;
		}

		value = RR_DYNAMIC_POINTER_CAST<MessageElement>(value1);
		return true;
	}

	bool WrappedWireConnection::TryGetOutValue(RR_INTRUSIVE_PTR<MessageElement>& value, TimeSpec& ts)
	{
		RR_INTRUSIVE_PTR<RRValue> value1;
		if (!TryGetOutValueBase(value1, ts))
		{
			return false;
		}

		value = RR_DYNAMIC_POINTER_CAST<MessageElement>(value1);
		return true;
	}

	//WrappedWireClient

	boost::function<void(RR_SHARED_PTR<WrappedWireConnection>)> WrappedWireClient::GetWireConnectCallback()
	{
		throw InvalidOperationException("Not valid for client");
	}
		
	void WrappedWireClient::SetWireConnectCallback(boost::function<void(RR_SHARED_PTR<WrappedWireConnection>)> function)
	{
		throw InvalidOperationException("Not valid for client");
	}
		
	RR_SHARED_PTR<WrappedWireConnection> WrappedWireClient::Connect()
	{
		RR_SHARED_PTR<detail::sync_async_handler<WireConnectionBase > > t=RR_MAKE_SHARED<detail::sync_async_handler<WireConnectionBase> >();
		AsyncConnect_internal(boost::bind(&detail::sync_async_handler<WireConnectionBase >::operator(),t,_1,_2),RobotRaconteurNode::s()->GetRequestTimeout());
		return boost::dynamic_pointer_cast<WrappedWireConnection>(t->end()); 
	}

	void WrappedWireClient::AsyncConnect(int32_t timeout, AsyncWireConnectionReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncWireConnectionReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncWireConnectionReturnDirector>, _1, id));
		AsyncConnect_internal(boost::bind(&WrappedWireClient::AsyncConnect_handler,rr_cast<WrappedWireClient>(shared_from_this()),_1,_2,sphandler),timeout);
	}

	void WrappedWireClient::AsyncConnect_handler(RR_SHARED_PTR<WireConnectionBase> ep, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncWireConnectionReturnDirector> handler)
	{
		if (err)
		{
			DIRECTOR_CALL2(handler->handler(RR_SHARED_PTR<WrappedWireConnection>(),err->ErrorCode,err->Error,err->Message));
			return;
		}

		RR_SHARED_PTR<WrappedWireConnection> ep2=boost::dynamic_pointer_cast<WrappedWireConnection>(ep);

		DIRECTOR_CALL2(handler->handler(ep2,0,"",""));

	}

	WrappedWireClient::WrappedWireClient(const std::string& name, RR_SHARED_PTR<ServiceStub> stub, RR_SHARED_PTR<TypeDefinition> Type, MemberDefinition_Direction direction) : WireClientBase(name,stub,direction)
	{
		this->Type=Type;
		this->rawelements=true;
	}

	RR_SHARED_PTR<WireConnectionBase> WrappedWireClient::CreateNewWireConnection(MemberDefinition_Direction direction,bool message3)
	{
		return RR_MAKE_SHARED<WrappedWireConnection>(rr_cast<WrappedWireClient>(shared_from_this()),0,Type,direction,message3);
	}

	RR_INTRUSIVE_PTR<MessageElement> WrappedWireClient::PeekInValue(TimeSpec& ts)
	{
		return RR_DYNAMIC_POINTER_CAST<MessageElement>(PeekInValueBase(ts));
	}
	RR_INTRUSIVE_PTR<MessageElement> WrappedWireClient::PeekOutValue(TimeSpec& ts)
	{
		return RR_DYNAMIC_POINTER_CAST<MessageElement>(PeekOutValueBase(ts));
	}
	void WrappedWireClient::PokeOutValue(const RR_INTRUSIVE_PTR<MessageElement>& value)
	{
		PokeOutValueBase(value);
	}

	void WrappedWireClient::AsyncPeekInValue(int32_t timeout, AsyncWirePeekReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncWirePeekReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncWirePeekReturnDirector>, _1, id));
		AsyncPeekInValueBase(boost::bind(&WrappedWireClient::AsyncPeekValue_handler, rr_cast<WrappedWireClient>(shared_from_this()), _1, _2, _3, sphandler), timeout);
	}
	void WrappedWireClient::AsyncPeekOutValue(int32_t timeout, AsyncWirePeekReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncWirePeekReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncWirePeekReturnDirector>, _1, id));
		AsyncPeekOutValueBase(boost::bind(&WrappedWireClient::AsyncPeekValue_handler, rr_cast<WrappedWireClient>(shared_from_this()), _1, _2, _3, sphandler), timeout);
	}
	void WrappedWireClient::AsyncPokeOutValue(const RR_INTRUSIVE_PTR<MessageElement>& value, int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncVoidReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncVoidReturnDirector>, _1, id));
		AsyncPokeOutValueBase(value, boost::bind(&WrappedWireClient::AsyncPokeValue_handler, rr_cast<WrappedWireClient>(shared_from_this()), _1, sphandler), timeout);
	}

	void WrappedWireClient::AsyncPeekValue_handler(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncWirePeekReturnDirector> handler)
	{
		if (err)
		{
			RR_INTRUSIVE_PTR<MessageElement> el;
			TimeSpec ts;
			DIRECTOR_CALL2(handler->handler(el, ts, err->ErrorCode, err->Error, err->Message));
			return;
		}
		
		RR_INTRUSIVE_PTR<MessageElement> value2 = RR_DYNAMIC_POINTER_CAST<MessageElement>(value);
		DIRECTOR_CALL2(handler->handler(value2, ts, 0, "", ""));
	}
	void WrappedWireClient::AsyncPokeValue_handler(RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncVoidReturnDirector> handler)
	{
		if (err)
		{			
			DIRECTOR_CALL2(handler->handler(err->ErrorCode, err->Error, err->Message));
			return;
		}
				
		DIRECTOR_CALL2(handler->handler(0, "", ""));
	}


	void WrappedWireServerConnectDirector::WireConnectCallbackFire(RR_SHARED_PTR<WireConnectionBase> e)
	{
		boost::shared_ptr<WrappedWireConnection> e2 = rr_cast<WrappedWireConnection>(e);
		DIRECTOR_CALL2(WireConnectCallback(e2));
	}

	WrappedWireServer::WrappedWireServer(const std::string& name, RR_SHARED_PTR<ServiceSkel> skel, RR_SHARED_PTR<TypeDefinition> Type, MemberDefinition_Direction direction) : WireServerBase(name,skel,direction)
	{
		this->Type=Type;
		this->rawelements=true;
		//this->objectheapid=0;
		//this->RR_Director=0;
	}

	WrappedWireServer::~WrappedWireServer()
	{
		//DIRECTOR_CALL2(DIRECTOR_DELETE(RR_Director));
	}

	RR_SHARED_PTR<WireConnectionBase> WrappedWireServer::CreateNewWireConnection(uint32_t endpoint, MemberDefinition_Direction direction, bool message3)
	{
		return RR_MAKE_SHARED<WrappedWireConnection>(rr_cast<WrappedWireServer>(shared_from_this()),endpoint,Type,direction,message3);
	}

	void WrappedWireServer::fire_WireConnectCallback(RR_SHARED_PTR<WireConnectionBase> e)
	{
		boost::function<void(RR_SHARED_PTR<WireConnectionBase >)> callback1;
		{
			boost::mutex::scoped_lock lock(callback_lock);
			callback1 = callback;
		}

		if (callback1)
		{
			callback1(e);
		}

	}

	void WrappedWireServer::SetWrappedWireConnectCallback(WrappedWireServerConnectDirector* director, int32_t id)
	{
		boost::mutex::scoped_lock lock(callback_lock);

		if (!director)
		{
			callback.clear();
			return;
		}

		RR_SHARED_PTR<WrappedWireServerConnectDirector> director1(director, boost::bind(&ReleaseDirector<WrappedWireServerConnectDirector>, _1, id));

		callback = boost::bind(&WrappedWireServerConnectDirector::WireConnectCallbackFire, director1, _1);
	}

	void WrappedWireServer::SetWireConnectCallback(boost::function<void(RR_SHARED_PTR<WireConnectionBase >)> function)
	{
		boost::mutex::scoped_lock lock(callback_lock);
		callback = function;
	}

	void WrappedWireServer::SetWireWrappedPeekPokeCallbacks(RR_SHARED_PTR<IWrappedWirePeekPokeCallbacks> peek_poke_callbacks)
	{
		boost::mutex::scoped_lock lock(callback_lock);
		wrapped_peek_poke_callbacks = peek_poke_callbacks;
	}

	void WrappedWireServer::Shutdown()
	{
		WireServerBase::Shutdown();
		boost::mutex::scoped_lock lock(callback_lock);
		callback.clear();
		peek_invalue_director.reset();
		peek_outvalue_director.reset();
		poke_outvalue_director.reset();
	}

	void WrappedWireServer::SetPeekInValueCallback(WrappedWireServerPeekValueDirector* director, int32_t id)
	{
		if (!director)
		{
			peek_invalue_director.reset(); 
			return;
		}

		RR_SHARED_PTR<WrappedWireServerPeekValueDirector> spdirector(director, boost::bind(&ReleaseDirector<WrappedWireServerPeekValueDirector>, _1, id));
		peek_invalue_director = spdirector;
	}
	void WrappedWireServer::SetPeekOutValueCallback(WrappedWireServerPeekValueDirector* director, int32_t id)
	{
		if (!director)
		{
			peek_outvalue_director.reset();
			return;
		}

		RR_SHARED_PTR<WrappedWireServerPeekValueDirector> spdirector(director, boost::bind(&ReleaseDirector<WrappedWireServerPeekValueDirector>, _1, id));
		peek_outvalue_director = spdirector;
	}
	void WrappedWireServer::SetPokeOutValueCallback(WrappedWireServerPokeValueDirector* director, int32_t id)
	{
		if (!director)
		{
			poke_outvalue_director.reset();
			return;
		}

		RR_SHARED_PTR<WrappedWireServerPokeValueDirector> spdirector(director, boost::bind(&ReleaseDirector<WrappedWireServerPokeValueDirector>, _1, id));
		poke_outvalue_director = spdirector;
	}


	RR_INTRUSIVE_PTR<RRValue> WrappedWireServer::do_PeekInValue(const uint32_t& ep)
	{
		boost::mutex::scoped_lock lock(callback_lock);
		RR_SHARED_PTR<IWrappedWirePeekPokeCallbacks> cb = wrapped_peek_poke_callbacks.lock();
		if (cb)
		{
			return cb->do_PeekInValue(ep);
		}

		if (peek_invalue_director)
		{
			RR_INTRUSIVE_PTR<MessageElement> el;
			DIRECTOR_CALL2(el = peek_invalue_director->PeekValue(ep));
				return el;
		}

		throw InvalidOperationException("");
	}

	RR_INTRUSIVE_PTR<RRValue> WrappedWireServer::do_PeekOutValue(const uint32_t& ep)
	{
		boost::mutex::scoped_lock lock(callback_lock);
		RR_SHARED_PTR<IWrappedWirePeekPokeCallbacks> cb = wrapped_peek_poke_callbacks.lock();
		if (cb)
		{
			return cb->do_PeekOutValue(ep);
		}

		if (peek_outvalue_director)
		{
			RR_INTRUSIVE_PTR<MessageElement> el;
			DIRECTOR_CALL2(el = peek_outvalue_director->PeekValue(ep))
				return el;
		}

		throw InvalidOperationException("");
	}

	void WrappedWireServer::do_PokeOutValue(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts, const uint32_t& ep)
	{
		boost::mutex::scoped_lock lock(callback_lock);
		RR_SHARED_PTR<IWrappedWirePeekPokeCallbacks> cb = wrapped_peek_poke_callbacks.lock();
		if (cb)
		{
			cb->do_PokeOutValue(value, ts, ep);
			return;
		}

		if (poke_outvalue_director)
		{
			RR_INTRUSIVE_PTR<MessageElement> el=RR_DYNAMIC_POINTER_CAST<MessageElement>(value);
			DIRECTOR_CALL2(poke_outvalue_director->PokeValue(el, ts, ep));
			return;
		}

		throw InvalidOperationException("");
	}


	/*WrappedWireServerDirector* WrappedWireServer::GetRRDirector()
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		return RR_Director;
	}*/
	
	bool WrappedWireBroadcasterPredicateDirector::CallPredicate(uint32_t client_endpoint)
	{
		bool res;
		DIRECTOR_CALL2(res = Predicate(client_endpoint));
		return res;
	}

	void WrappedWireBroadcaster::Init(RR_SHARED_PTR<WrappedWireServer> wire)
	{
		copy_element = true;
		InitBase(wire);
	}

	void WrappedWireBroadcaster::SetOutValue(RR_INTRUSIVE_PTR<MessageElement> value)
	{
		SetOutValueBase(value);
	}

	void WrappedWireBroadcaster::AttachWireServerEvents(RR_SHARED_PTR<WireServerBase> w)
	{
		RR_SHARED_PTR<WrappedWireServer> w_T = rr_cast<WrappedWireServer>(w);
		w_T->SetWireConnectCallback(boost::bind(&WrappedWireBroadcaster::ConnectionConnectedBase, shared_from_this(), _1));
		w_T->SetWireWrappedPeekPokeCallbacks(RR_DYNAMIC_POINTER_CAST<IWrappedWirePeekPokeCallbacks>(shared_from_this()));
	}

	void WrappedWireBroadcaster::AttachWireConnectionEvents(RR_SHARED_PTR<WireConnectionBase> w, RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> cep)
	{
		RR_SHARED_PTR<WrappedWireConnection> w_T = rr_cast<WrappedWireConnection>(w);
		w_T->SetWireBroadcaster(RR_STATIC_POINTER_CAST<WrappedWireBroadcaster>(shared_from_this()), cep);
	}

	void WrappedWireBroadcaster::SetPredicateDirector(WrappedWireBroadcasterPredicateDirector* f, int32_t id)
	{
		RR_SHARED_PTR<WrappedWireBroadcasterPredicateDirector> f_sp(f, boost::bind(&ReleaseDirector<WrappedWireBroadcasterPredicateDirector>, _1, id));
		this->SetPredicate(boost::bind(&WrappedWireBroadcasterPredicateDirector::CallPredicate, f_sp, _2));
	}

	RR_INTRUSIVE_PTR<RRValue> WrappedWireBroadcaster::do_PeekInValue(const uint32_t& ep)
	{
		boost::mutex::scoped_lock lock(connected_wires_lock);
		RR_INTRUSIVE_PTR<MessageElement> value2 = ShallowCopyMessageElement(rr_cast<MessageElement>(out_value));
		return value2;
	}
	RR_INTRUSIVE_PTR<RRValue> WrappedWireBroadcaster::do_PeekOutValue(const uint32_t& ep)
	{
		throw ReadOnlyMemberException("Read only member");
	}
	void WrappedWireBroadcaster::do_PokeOutValue(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts, const uint32_t& ep)
	{
		throw ReadOnlyMemberException("Read only member");
	}

	void WrappedWireUnicastReceiver::Init(RR_SHARED_PTR<WrappedWireServer> server)
	{
		RR_SHARED_PTR<wireserver_type> wire_server = RR_DYNAMIC_POINTER_CAST<wireserver_type>(server);
		if (!wire_server) throw InvalidOperationException("WireServer required for WireUnicastReceiver");
		this->wire = wire_server;
		wire_server->SetWireConnectCallback(boost::bind(&WrappedWireUnicastReceiver::WrappedConnectionConnected, RR_DYNAMIC_POINTER_CAST<WrappedWireUnicastReceiver>(shared_from_this()), _1));
		wire_server->SetWireWrappedPeekPokeCallbacks(RR_DYNAMIC_POINTER_CAST<IWrappedWirePeekPokeCallbacks>(shared_from_this()));
	}

	void WrappedWireUnicastReceiver::WrappedConnectionConnected(RR_SHARED_PTR<WireConnectionBase> connection)
	{
		RR_SHARED_PTR<WrappedWireConnection> connection1 = RR_DYNAMIC_POINTER_CAST<WrappedWireConnection>(connection);
		boost::mutex::scoped_lock lock(this_lock);
		if (active_connection)
		{
			try
			{
				active_connection->Close();
			}
			catch (std::exception&) {}
			active_connection.reset();
		}
		active_connection = connection1;
		connection1->SetWireUnicastReceiver(RR_DYNAMIC_POINTER_CAST<WrappedWireUnicastReceiver>(shared_from_this()));
	}

	RR_INTRUSIVE_PTR<RRValue> WrappedWireUnicastReceiver::do_PeekInValue(const uint32_t& ep)
	{
		return ClientPeekInValue();
	}

	RR_INTRUSIVE_PTR<RRValue> WrappedWireUnicastReceiver::do_PeekOutValue(const uint32_t& ep)
	{
		return ClientPeekOutValue();
	}

	void WrappedWireUnicastReceiver::do_PokeOutValue(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts, const uint32_t& ep)
	{
		ClientPokeOutValue(rr_cast<MessageElement>(value), ts, ep);
	}

	void WrappedWireUnicastReceiver::AddInValueChangedListener(WrappedWireServerPokeValueDirector* director, int32_t id)
	{		
		RR_SHARED_PTR<WrappedWireServerPokeValueDirector> spdirector(director, boost::bind(&ReleaseDirector<WrappedWireServerPokeValueDirector>, _1, id));
		InValueChanged.connect(boost::bind(&WrappedWireUnicastReceiver::ValueChanged,_1,_2,_3, spdirector));
	}

	void WrappedWireUnicastReceiver::ValueChanged(const RR_INTRUSIVE_PTR<MessageElement>& m, const TimeSpec& ts, const uint32_t& ep, RR_SHARED_PTR<WrappedWireServerPokeValueDirector> spdirector)
	{
		DIRECTOR_CALL2(spdirector->PokeValue(m, ts, ep))
	}

	//Generator Function

	WrappedGeneratorClient::WrappedGeneratorClient(const std::string& name, int32_t id, RR_SHARED_PTR<ServiceStub> stub)
		: GeneratorClientBase(name, id, stub)
	{

	}

	RR_INTRUSIVE_PTR<MessageElement> WrappedGeneratorClient::Next(RR_INTRUSIVE_PTR<MessageElement> v)
	{
		return NextBase(v);
	}
	void WrappedGeneratorClient::AsyncNext(RR_INTRUSIVE_PTR<MessageElement> v, int32_t timeout, AsyncRequestDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncRequestDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncRequestDirector>, _1, id));
		AsyncNextBase(v, boost::bind(&WrappedGeneratorClient::AsyncNext_handler,  _1, _2, sphandler), timeout);
	}

	void WrappedGeneratorClient::AsyncNext_handler(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> m, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncRequestDirector> handler)
	{
		if (err)
		{

			DIRECTOR_CALL2(handler->handler(RR_INTRUSIVE_PTR<MessageElement>(), err->ErrorCode, err->Error, err->Message));

			return;
		}		
		DIRECTOR_CALL2(handler->handler(m, 0, "", ""));
	}
		
	void WrappedGeneratorClient::AsyncAbort(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncVoidReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncVoidReturnDirector>, _1, id));
		GeneratorClientBase::AsyncAbort(boost::bind(&WrappedGeneratorClient::AsyncAbort_handler, _1, sphandler), timeout);
	}

	void WrappedGeneratorClient::AsyncClose(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncVoidReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncVoidReturnDirector>, _1, id));
		GeneratorClientBase::AsyncClose(boost::bind(&WrappedGeneratorClient::AsyncAbort_handler, _1, sphandler), timeout);
	}

	void WrappedGeneratorClient::AsyncAbort_handler(RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncVoidReturnDirector> handler)
	{
		if (err)
		{
			DIRECTOR_CALL2(handler->handler(err->ErrorCode, err->Error, err->Message));
			return;
		}

		DIRECTOR_CALL2(handler->handler(0, "", ""));
	}

	std::vector<RR_INTRUSIVE_PTR<MessageElement> > WrappedGeneratorClient::NextAll()
	{
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > o;
		try
		{
			while (true)
			{
				o.push_back(Next(RR_INTRUSIVE_PTR<MessageElement>()));
			}
		}
		catch (StopIterationException) {}
		return o;
	}


	WrappedGeneratorServer::WrappedGeneratorServer(const std::string& name, int32_t index, RR_SHARED_PTR<ServiceSkel> skel, RR_SHARED_PTR<ServerEndpoint> ep, WrappedGeneratorServerDirector* director)
		: GeneratorServerBase(name, index, skel, ep)
	{
		this->RR_Director.reset(director, boost::bind(&ReleaseDirector<WrappedGeneratorServerDirector>, _1, director->objectheapid));
	}

	void WrappedGeneratorServer::CallNext(RR_INTRUSIVE_PTR<MessageEntry> m)
	{
		if (m->Error != MessageErrorType_None)
		{			
			try
			{
				if (m->Error == MessageErrorType_StopIteration)
				{
					DIRECTOR_CALL2(RR_Director->Close());
				}
				else
				{
					DIRECTOR_CALL2(RR_Director->Abort());
				}
			}
			catch (std::exception& exp)
			{
				GeneratorServerBase::CallNext1<int32_t>(0, RobotRaconteurExceptionUtil::ExceptionToSharedPtr(exp), index, skel, m, ep);
				return;
			}
			GeneratorServerBase::CallNext1<int32_t>(0, RR_SHARED_PTR<RobotRaconteurException>(), index, skel, m, ep);
			return;
		}

		RR_INTRUSIVE_PTR<MessageElement> m2;
		try
		{
			RR_INTRUSIVE_PTR<MessageElement> m1;
			m->TryFindElement("parameter", m1);			
			DIRECTOR_CALL2(m2 = RR_Director->Next(m1));
			if (m2)
			{
				m2->ElementName = "return";
			}
		}
		catch (std::exception& exp)
		{
			GeneratorServerBase::EndAsyncCallNext(skel, RR_INTRUSIVE_PTR<MessageElement>(), RobotRaconteurExceptionUtil::ExceptionToSharedPtr(exp), index, m, ep);
			return;
		}
		GeneratorServerBase::EndAsyncCallNext(skel, m2, RR_SHARED_PTR<RobotRaconteurException>(), index, m, ep);
	}

	
	//Memory Membor

#define RR_WAMCU_READ_TYPE(type) RR_SHARED_PTR<ArrayMemory<type> > type ## _var=boost::dynamic_pointer_cast<ArrayMemory<type> >(mem); \
	if (type ## _var) \
	{ \
		RR_INTRUSIVE_PTR<RRArray<type> > dat=AllocateRRArray<type>((size_t)count); \
		type ## _var->Read(memorypos,dat,0,count); \
		return dat; \
	} \

	RR_INTRUSIVE_PTR<RRBaseArray> WrappedArrayMemoryClientUtil::Read(RR_SHARED_PTR<ArrayMemoryBase> mem, uint64_t memorypos, uint64_t count)
	{
		/*RR_SHARED_PTR<ArrayMemory<int8_t> > i8=rr_cast<ArrayMemory<int8_t> >(mem);
		if (i8)
		{
			RR_SHARED_PTR<RRArray<int8_t> > dat=AllocateRRArray<int8_t>(count);
			i8->Read(memorypos,dat,bufferpos,count);
			return dat;
		}*/

		RR_WAMCU_READ_TYPE(int8_t);
		RR_WAMCU_READ_TYPE(uint8_t);
		RR_WAMCU_READ_TYPE(int16_t);
		RR_WAMCU_READ_TYPE(uint16_t);
		RR_WAMCU_READ_TYPE(int32_t);
		RR_WAMCU_READ_TYPE(uint32_t);
		RR_WAMCU_READ_TYPE(int64_t);
		RR_WAMCU_READ_TYPE(uint64_t);
		RR_WAMCU_READ_TYPE(double);
		RR_WAMCU_READ_TYPE(float);
		RR_WAMCU_READ_TYPE(cdouble);
		RR_WAMCU_READ_TYPE(cfloat);
		RR_WAMCU_READ_TYPE(rr_bool);

		throw DataTypeException("Invalid memory data type");

	}

#define RR_WAMCU_WRITE_TYPE(type) RR_SHARED_PTR<ArrayMemory<type> > type ## _var=boost::dynamic_pointer_cast<ArrayMemory<type> >(mem); \
	if (type ## _var) \
	{ \
		type ## _var->Write(memorypos,rr_cast<RRArray<type> >(buffer),bufferpos,count); \
		return; \
	} \

	void WrappedArrayMemoryClientUtil::Write(RR_SHARED_PTR<ArrayMemoryBase> mem, uint64_t memorypos, RR_INTRUSIVE_PTR<RRBaseArray> buffer, uint64_t bufferpos, uint64_t count)
	{
		/*RR_SHARED_PTR<ArrayMemory<int8_t> > i8=rr_cast<ArrayMemory<int8_t> >(mem);
		if (i8)
		{
			i8->Write(memorypos,rr_cast<RRArray<int8_t> >(buffer),bufferpos,count);

		}*/

		RR_WAMCU_WRITE_TYPE(int8_t);
		RR_WAMCU_WRITE_TYPE(uint8_t);
		RR_WAMCU_WRITE_TYPE(int16_t);
		RR_WAMCU_WRITE_TYPE(uint16_t);
		RR_WAMCU_WRITE_TYPE(int32_t);
		RR_WAMCU_WRITE_TYPE(uint32_t);
		RR_WAMCU_WRITE_TYPE(int64_t);
		RR_WAMCU_WRITE_TYPE(uint64_t);
		RR_WAMCU_WRITE_TYPE(double);
		RR_WAMCU_WRITE_TYPE(float);
		RR_WAMCU_WRITE_TYPE(cdouble);
		RR_WAMCU_WRITE_TYPE(cfloat);
		RR_WAMCU_WRITE_TYPE(rr_bool);

		throw DataTypeException("Invalid memory data type");

	}

#define RR_WAMCU_DIRECTION_TYPE(type) RR_SHARED_PTR<ArrayMemoryClient<type> > type ## _var=boost::dynamic_pointer_cast<ArrayMemoryClient<type> >(mem); \
	if (type ## _var) \
	{ \
		return type ## _var->Direction(); \
	} \

	MemberDefinition_Direction WrappedArrayMemoryClientUtil::Direction(RR_SHARED_PTR<ArrayMemoryBase> mem)
	{
		RR_WAMCU_DIRECTION_TYPE(int8_t);
		RR_WAMCU_DIRECTION_TYPE(uint8_t);
		RR_WAMCU_DIRECTION_TYPE(int16_t);
		RR_WAMCU_DIRECTION_TYPE(uint16_t);
		RR_WAMCU_DIRECTION_TYPE(int32_t);
		RR_WAMCU_DIRECTION_TYPE(uint32_t);
		RR_WAMCU_DIRECTION_TYPE(int64_t);
		RR_WAMCU_DIRECTION_TYPE(uint64_t);
		RR_WAMCU_DIRECTION_TYPE(double);
		RR_WAMCU_DIRECTION_TYPE(float);
		RR_WAMCU_DIRECTION_TYPE(cdouble);
		RR_WAMCU_DIRECTION_TYPE(cfloat);
		RR_WAMCU_DIRECTION_TYPE(rr_bool);

		throw DataTypeException("Invalid memory data type");

	}

#define RR_WMDAMCU_READ_TYPE(type) RR_SHARED_PTR<MultiDimArrayMemory<type> > type ## _var=boost::dynamic_pointer_cast<MultiDimArrayMemory<type> >(mem); \
	if (type ## _var) \
	{ \
		RR_INTRUSIVE_PTR<RRArray<type> > arraydat=AllocateRRArray<type>(elems); \
\
		RR_INTRUSIVE_PTR<RRMultiDimArray<type> > dat=AllocateRRMultiDimArray<type>(VectorToRRArray<uint32_t>(count),arraydat); \
\
		std::vector<uint64_t> bufferpos(count.size()); \
		std::fill(bufferpos.begin(),bufferpos.end(),0); \
\
		type ## _var->Read(memorypos,dat,bufferpos,count); \
\
		RR_SHARED_PTR<RRMultiDimArrayUntyped> dat2 = RR_MAKE_SHARED<RRMultiDimArrayUntyped>();\
		dat2->Dims=dat->Dims; \
		dat2->Array=dat->Array; \
		\
		return dat2; \
	}

	RR_SHARED_PTR<RRMultiDimArrayUntyped> WrappedMultiDimArrayMemoryClientUtil::Read(RR_SHARED_PTR<MultiDimArrayMemoryBase> mem, std::vector<uint64_t> memorypos, std::vector<uint64_t> count)
	{
		size_t elems=1;
		for(std::vector<uint64_t>::iterator e=count.begin(); e!=count.end(); ++e)
		{
			elems*=(size_t)*e;
		}

		/*RR_SHARED_PTR<MultiDimArrayMemory<int8_t> > i8=rr_cast<MultiDimArrayMemory<int8_t> >(mem);
		if (i8)
		{
			RR_SHARED_PTR<RRArray<int8_t> > realdat=AllocateRRArray<int8_t>(elems);
			RR_SHARED_PTR<RRArray<int8_t> > imagdat;
			if (i8->Complex())
			{
				imagdat=AllocateRRArray<int8_t>(elems);
			}

			RR_SHARED_PTR<RRMultiDimArray<int8_t> > dat=RR_MAKE_SHARED<RRMultiDimArray<int8_t> >(VectorToRRArray<int32_t>(count),realdat,imagdat);

			std::vector<uint64_t> bufferpos(count.size());
			std::fill(bufferpos.begin(),bufferpos.end(),0);

			i8->Read(memorypos,dat,bufferpos,count);

			RR_SHARED_PTR<RRMultiDimArrayUntyped> dat2=RR_MAKE_SHARED<RRMultiDimArrayUntyped>();
			dat2->DimCount=dat->DimCount;
			dat2->Dims=dat->Dims;
			dat2->Complex=dat->Complex;
			dat2->Real=dat->Real;
			dat2->Imag=dat->Imag;

			return dat2;
		}*/

		RR_WMDAMCU_READ_TYPE(int8_t);
		RR_WMDAMCU_READ_TYPE(uint8_t);
		RR_WMDAMCU_READ_TYPE(int16_t);
		RR_WMDAMCU_READ_TYPE(uint16_t);
		RR_WMDAMCU_READ_TYPE(int32_t);
		RR_WMDAMCU_READ_TYPE(uint32_t);
		RR_WMDAMCU_READ_TYPE(int64_t);
		RR_WMDAMCU_READ_TYPE(uint64_t);
		RR_WMDAMCU_READ_TYPE(double);
		RR_WMDAMCU_READ_TYPE(float);
		RR_WMDAMCU_READ_TYPE(cdouble);
		RR_WMDAMCU_READ_TYPE(cfloat);
		RR_WMDAMCU_READ_TYPE(rr_bool);

		throw DataTypeException("Invalid memory data type");

	}

#define RR_WMDAMCU_WRITE_TYPE(type) RR_SHARED_PTR<MultiDimArrayMemory<type> > type ## _var=boost::dynamic_pointer_cast<MultiDimArrayMemory<type> >(mem); \
	if (type ## _var) \
	{ \
		RR_INTRUSIVE_PTR<RRMultiDimArray<type> > buffer2=AllocateRRMultiDimArray<type>(rr_cast<RRArray<uint32_t> >(buffer->Dims),rr_cast<RRArray<type> >(buffer->Array)); \
		type ## _var->Write(memorypos,buffer2,bufferpos,count); \
		return; \
	}

	void WrappedMultiDimArrayMemoryClientUtil::Write(RR_SHARED_PTR<MultiDimArrayMemoryBase> mem, std::vector<uint64_t> memorypos, RR_SHARED_PTR<RRMultiDimArrayUntyped> buffer, std::vector<uint64_t> bufferpos, std::vector<uint64_t> count)
	{
		/*RR_SHARED_PTR<MultiDimArrayMemory<int8_t> > i8=rr_cast<MultiDimArrayMemory<int8_t> >(mem);
		if (i8)
		{
			RR_SHARED_PTR<RRMultiDimArray<int8_t> > buffer2=RR_MAKE_SHARED<RRMultiDimArray<int8_t> >(buffer->Dims,rr_cast<RRMultiDimArray<int8_t> >(buffer->Real),rr_cast<RRMultiDimArray<int8_t> >(buffer->Imag));
			i8->Write(memorypos,buffer2,bufferpos,count);
		}*/

		RR_WMDAMCU_WRITE_TYPE(int8_t);
		RR_WMDAMCU_WRITE_TYPE(uint8_t);
		RR_WMDAMCU_WRITE_TYPE(int16_t);
		RR_WMDAMCU_WRITE_TYPE(uint16_t);
		RR_WMDAMCU_WRITE_TYPE(int32_t);
		RR_WMDAMCU_WRITE_TYPE(uint32_t);
		RR_WMDAMCU_WRITE_TYPE(int64_t);
		RR_WMDAMCU_WRITE_TYPE(uint64_t);
		RR_WMDAMCU_WRITE_TYPE(double);
		RR_WMDAMCU_WRITE_TYPE(float);
		RR_WMDAMCU_WRITE_TYPE(cdouble);
		RR_WMDAMCU_WRITE_TYPE(cfloat);
		RR_WMDAMCU_WRITE_TYPE(rr_bool);

		throw DataTypeException("Invalid memory data type");
	}
#define RR_WMDAMCU_DIRECTION_TYPE(type) RR_SHARED_PTR<MultiDimArrayMemoryClient<type> > type ## _var=boost::dynamic_pointer_cast<MultiDimArrayMemoryClient<type> >(mem); \
	if (type ## _var) \
	{ \
		return type ## _var->Direction(); \
	} \

	MemberDefinition_Direction WrappedMultiDimArrayMemoryClientUtil::Direction(RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
	{
		RR_WMDAMCU_DIRECTION_TYPE(int8_t);
		RR_WMDAMCU_DIRECTION_TYPE(uint8_t);
		RR_WMDAMCU_DIRECTION_TYPE(int16_t);
		RR_WMDAMCU_DIRECTION_TYPE(uint16_t);
		RR_WMDAMCU_DIRECTION_TYPE(int32_t);
		RR_WMDAMCU_DIRECTION_TYPE(uint32_t);
		RR_WMDAMCU_DIRECTION_TYPE(int64_t);
		RR_WMDAMCU_DIRECTION_TYPE(uint64_t);
		RR_WMDAMCU_DIRECTION_TYPE(double);
		RR_WMDAMCU_DIRECTION_TYPE(float);
		RR_WMDAMCU_DIRECTION_TYPE(cdouble);
		RR_WMDAMCU_DIRECTION_TYPE(cfloat);
		RR_WMDAMCU_DIRECTION_TYPE(rr_bool);

		throw DataTypeException("Invalid memory data type");

	}

	// PodArrayMemory

	WrappedPodArrayMemoryClient::WrappedPodArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction)
		: ArrayMemoryClientBase(membername, stub, DataTypes_pod_t, element_size, direction)
	{
	}

	void WrappedPodArrayMemoryClient::Read(uint64_t memorypos, WrappedPodArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count)
	{
		if (!buffer) throw NullValueException("Buffer must not be null");
		ReadBase(memorypos, buffer, bufferpos, count);
	}

	void WrappedPodArrayMemoryClient::Write(uint64_t memorypos, WrappedPodArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count)
	{
		if (!buffer) throw NullValueException("Buffer must not be null");
		WriteBase(memorypos, buffer, bufferpos, count);
	}
		
	void WrappedPodArrayMemoryClient::UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, uint64_t bufferpos, uint64_t count)
	{
		DIRECTOR_CALL2(((WrappedPodArrayMemoryClientBuffer*)buffer)->UnpackReadResult(rr_cast<MessageElementPodArray>(res), bufferpos, count));
	}

	RR_INTRUSIVE_PTR<MessageElementData> WrappedPodArrayMemoryClient::PackWriteRequest(void* buffer, uint64_t bufferpos, uint64_t count)
	{
		RR_INTRUSIVE_PTR<MessageElementData> ret;
		DIRECTOR_CALL2(ret = ((WrappedPodArrayMemoryClientBuffer*)buffer)->PackWriteRequest(bufferpos, count));
		return ret;
	}

	size_t WrappedPodArrayMemoryClient::GetBufferLength(void* buffer)
	{
		uint64_t ret;
		DIRECTOR_CALL2(ret = ((WrappedPodArrayMemoryClientBuffer*)buffer)->GetBufferLength());
		return ret;
	}

	WrappedPodMultiDimArrayMemoryClient::WrappedPodMultiDimArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction)
		: MultiDimArrayMemoryClientBase(membername, stub, DataTypes_pod_t, element_size, direction)
	{

	}
	void WrappedPodMultiDimArrayMemoryClient::Read(const std::vector<uint64_t>& memorypos, WrappedPodMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
	{
		if (!buffer) throw NullValueException("Buffer must not be null");
		ReadBase(memorypos, buffer, bufferpos, count);
	}
	void WrappedPodMultiDimArrayMemoryClient::Write(const std::vector<uint64_t>& memorypos, WrappedPodMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
	{
		if (!buffer) throw NullValueException("Buffer must not be null");
		WriteBase(memorypos, buffer, bufferpos, count);
	}
	void WrappedPodMultiDimArrayMemoryClient::UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
	{
		DIRECTOR_CALL2(((WrappedPodMultiDimArrayMemoryClientBuffer*)buffer)->UnpackReadResult(rr_cast<MessageElementPodMultiDimArray>(res), bufferpos, count));
	}

	RR_INTRUSIVE_PTR<MessageElementData> WrappedPodMultiDimArrayMemoryClient::PackWriteRequest(void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
	{
		RR_INTRUSIVE_PTR<MessageElementData> ret;
		DIRECTOR_CALL2(ret = ((WrappedPodMultiDimArrayMemoryClientBuffer*)buffer)->PackWriteRequest(bufferpos, count));
		return ret;
	}
	
	// NamedArrayMemory

	WrappedNamedArrayMemoryClient::WrappedNamedArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction)
		: ArrayMemoryClientBase(membername, stub, DataTypes_pod_t, element_size, direction)
	{
	}

	void WrappedNamedArrayMemoryClient::Read(uint64_t memorypos, WrappedNamedArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count)
	{
		if (!buffer) throw NullValueException("Buffer must not be null");
		ReadBase(memorypos, buffer, bufferpos, count);
	}

	void WrappedNamedArrayMemoryClient::Write(uint64_t memorypos, WrappedNamedArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count)
	{
		if (!buffer) throw NullValueException("Buffer must not be null");
		WriteBase(memorypos, buffer, bufferpos, count);
	}

	void WrappedNamedArrayMemoryClient::UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, uint64_t bufferpos, uint64_t count)
	{
		DIRECTOR_CALL2(((WrappedNamedArrayMemoryClientBuffer*)buffer)->UnpackReadResult(rr_cast<MessageElementNamedArray>(res), bufferpos, count));
	}

	RR_INTRUSIVE_PTR<MessageElementData> WrappedNamedArrayMemoryClient::PackWriteRequest(void* buffer, uint64_t bufferpos, uint64_t count)
	{
		RR_INTRUSIVE_PTR<MessageElementData> ret;
		DIRECTOR_CALL2(ret = ((WrappedNamedArrayMemoryClientBuffer*)buffer)->PackWriteRequest(bufferpos, count));
		return ret;
	}

	size_t WrappedNamedArrayMemoryClient::GetBufferLength(void* buffer)
	{
		uint64_t ret;
		DIRECTOR_CALL2(ret = ((WrappedNamedArrayMemoryClientBuffer*)buffer)->GetBufferLength());
		return ret;
	}

	WrappedNamedMultiDimArrayMemoryClient::WrappedNamedMultiDimArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction)
		: MultiDimArrayMemoryClientBase(membername, stub, DataTypes_pod_t, element_size, direction)
	{

	}
	void WrappedNamedMultiDimArrayMemoryClient::Read(const std::vector<uint64_t>& memorypos, WrappedNamedMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
	{
		if (!buffer) throw NullValueException("Buffer must not be null");
		ReadBase(memorypos, buffer, bufferpos, count);
	}
	void WrappedNamedMultiDimArrayMemoryClient::Write(const std::vector<uint64_t>& memorypos, WrappedNamedMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
	{
		if (!buffer) throw NullValueException("Buffer must not be null");
		WriteBase(memorypos, buffer, bufferpos, count);
	}
	void WrappedNamedMultiDimArrayMemoryClient::UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
	{
		DIRECTOR_CALL2(((WrappedNamedMultiDimArrayMemoryClientBuffer*)buffer)->UnpackReadResult(rr_cast<MessageElementNamedMultiDimArray>(res), bufferpos, count));
	}

	RR_INTRUSIVE_PTR<MessageElementData> WrappedNamedMultiDimArrayMemoryClient::PackWriteRequest(void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
	{
		RR_INTRUSIVE_PTR<MessageElementData> ret;
		DIRECTOR_CALL2(ret = ((WrappedNamedMultiDimArrayMemoryClientBuffer*)buffer)->PackWriteRequest(bufferpos, count));
		return ret;
	}

	// Service Discovery
	ServiceInfo2Wrapped::ServiceInfo2Wrapped(const ServiceInfo2& value)
	{
		Name=value.Name;
		RootObjectType=value.RootObjectType;
		RootObjectImplements=value.RootObjectImplements;
		ConnectionURL=value.ConnectionURL;
		NodeID=value.NodeID;
		NodeName=value.NodeName;

		RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > map=AllocateEmptyRRMap<std::string,RRValue>();
		map->GetStorageContainer()=value.Attributes;
		RR_INTRUSIVE_PTR<MessageElementMap<std::string> > mmap=RobotRaconteurNode::s()->PackMapType<std::string,RRValue>(map);
		Attributes= CreateMessageElement("value",mmap);

	}

	std::vector<RobotRaconteur::ServiceInfo2Wrapped> WrappedFindServiceByType(RR_SHARED_PTR<RobotRaconteurNode> node, const std::string &servicetype, const std::vector<std::string>& transportschemes)
	{

		std::vector<ServiceInfo2> dat=node->FindServiceByType(servicetype, transportschemes);
		std::vector<ServiceInfo2Wrapped> ret;
		for (std::vector<ServiceInfo2>::iterator e=dat.begin(); e!=dat.end(); ++e)
		{
			ret.push_back(ServiceInfo2Wrapped(*e));

		}
		return ret;
	}

	std::vector<RobotRaconteur::NodeInfo2> WrappedFindNodeByID(RR_SHARED_PTR<RobotRaconteurNode> node, const NodeID& id, const std::vector<std::string>& transportschemes)
	{
		return node->FindNodeByID(id,transportschemes);
	}

	std::vector<RobotRaconteur::NodeInfo2> WrappedFindNodeByName(RR_SHARED_PTR<RobotRaconteurNode> node, const std::string& name, const std::vector<std::string>& transportschemes)
	{
		return node->FindNodeByName(name,transportschemes);
	}

	void AsyncServiceInfo2VectorReturn_handler(RR_SHARED_PTR<std::vector<ServiceInfo2> > ret, RR_SHARED_PTR<AsyncServiceInfo2VectorReturnDirector> handler)
	{
		std::vector<ServiceInfo2Wrapped> ret1;
		if (ret)
		{
			for(std::vector<ServiceInfo2>::iterator e=ret->begin(); e!=ret->end(); e++)
			{
				ret1.push_back(ServiceInfo2Wrapped(*e));
			}

		}
		
		DIRECTOR_CALL2(handler->handler(ret1));
		return;
	}

	void AsyncNodeInfo2VectorReturn_handler(RR_SHARED_PTR<std::vector<NodeInfo2> > ret, RR_SHARED_PTR<AsyncNodeInfo2VectorReturnDirector> handler)
	{
		std::vector<NodeInfo2> ret1=*ret.get();
		
		DIRECTOR_CALL2(handler->handler(ret1));
		return;
	}

	void AsyncWrappedFindServiceByType(RR_SHARED_PTR<RobotRaconteurNode> node, const std::string &servicetype, const std::vector<std::string>& transportschemes, int32_t timeout, AsyncServiceInfo2VectorReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncServiceInfo2VectorReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector <AsyncServiceInfo2VectorReturnDirector>, _1, id));
		node->AsyncFindServiceByType(servicetype,transportschemes,boost::bind(&AsyncServiceInfo2VectorReturn_handler,_1,sphandler),timeout);
	}

	void AsyncWrappedFindNodeByID(RR_SHARED_PTR<RobotRaconteurNode> node, const NodeID& id, const std::vector<std::string>& transportschemes, int32_t timeout, AsyncNodeInfo2VectorReturnDirector* handler, int32_t id1)
	{
		RR_SHARED_PTR<AsyncNodeInfo2VectorReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncNodeInfo2VectorReturnDirector>, _1, id1));
		node->AsyncFindNodeByID(id,transportschemes,boost::bind(&AsyncNodeInfo2VectorReturn_handler,_1,sphandler),timeout);
	}
	
	void AsyncWrappedFindNodeByName(RR_SHARED_PTR<RobotRaconteurNode> node, const std::string& name, const std::vector<std::string>& transportschemes, int32_t timeout, AsyncNodeInfo2VectorReturnDirector* handler, int32_t id)
	{
		RR_SHARED_PTR<AsyncNodeInfo2VectorReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncNodeInfo2VectorReturnDirector>, _1, id));
		node->AsyncFindNodeByName(name,transportschemes,boost::bind(&AsyncNodeInfo2VectorReturn_handler,_1,sphandler),timeout);
	}
	
	void WrappedUpdateDetectedNodes(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<std::string>& schemes)
	{
		node->UpdateDetectedNodes(schemes);
	}

	void AsyncWrappedUpdateDetectedNodes(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<std::string>& schemes, int32_t timeout, AsyncVoidNoErrReturnDirector* handler, int32_t id1)
	{
		RR_SHARED_PTR<AsyncVoidNoErrReturnDirector> sphandler(handler, boost::bind(&ReleaseDirector<AsyncVoidNoErrReturnDirector>, _1, id1));
		node->AsyncUpdateDetectedNodes(schemes, boost::bind(&AsyncVoidNoErrReturn_handler, sphandler), timeout);
	}

	std::vector<std::string> WrappedGetDetectedNodes(RR_SHARED_PTR<RobotRaconteurNode> node)
	{
		std::vector<std::string> o;
		std::vector<NodeDiscoveryInfo> o1=node->GetDetectedNodes();
		for (std::vector<NodeDiscoveryInfo>::iterator e = o1.begin(); e != o1.end(); e++)
		{
			o.push_back(e->NodeID.ToString());
		}
		return o;
	}

	void WrappedServiceSkel::Init(const std::string &s, RR_SHARED_PTR<RRObject> o, RR_SHARED_PTR<ServerContext> c)
	{

		boost::shared_ptr<WrappedRRObject> o2=rr_cast<WrappedRRObject>(o);
		castobj=o2;

		boost::shared_ptr<WrappedServiceSkelDirector> RR_Director1=o2->GetRRDirector();

		

		boost::shared_ptr<ServiceEntryDefinition> type1;
		std::string type=o->RRType();
		boost::tuple<std::string,std::string> s1=SplitQualifiedName(type);
		
		
		std::vector<boost::shared_ptr<ServiceEntryDefinition> > objs=RobotRaconteurNode::s()->GetServiceType(s1.get<0>())->ServiceDef()->Objects;
		for (std::vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator e=objs.begin(); e!=objs.end(); ++e)
		{
			if ((*e)->Name==s1.get<1>())
			{
				type1=*e;
			}
		}

		if (!type1) throw ServiceException("Unknown object type");

		Type=type1;
		ServiceSkel::Init(s,o,c);

		for (std::vector<RR_SHARED_PTR<MemberDefinition> >::iterator e=Type->Members.begin(); e!=Type->Members.end(); ++e)
		{
			RR_SHARED_PTR<PipeDefinition> p=boost::dynamic_pointer_cast<PipeDefinition>(*e);
			if (p)
			{
				bool unreliable = p->IsUnreliable();
				MemberDefinition_Direction direction = p->Direction();
				
				RR_SHARED_PTR<WrappedPipeServer> c=RR_MAKE_SHARED<WrappedPipeServer>(p->Name,shared_from_this(),p->Type,unreliable,direction);
				pipes.insert(std::make_pair(p->Name,c));				

			}

			RR_SHARED_PTR<WireDefinition> w=boost::dynamic_pointer_cast<WireDefinition>(*e);
			if (w)
			{
				MemberDefinition_Direction direction = w->Direction();
				RR_SHARED_PTR<WrappedWireServer> c=RR_MAKE_SHARED<WrappedWireServer>(w->Name,shared_from_this(),w->Type,direction);
				wires.insert(std::make_pair(w->Name,c));

			}

			RR_SHARED_PTR<MemoryDefinition> m=boost::dynamic_pointer_cast<MemoryDefinition>(*e);
			if (m)
			{
				MemberDefinition_Direction direction = m->Direction();
				memorytypes.insert(make_pair(m->Name,m));
				if (IsTypeNumeric(m->Type->Type))
				{
					if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
					{
						RR_SHARED_PTR<void> o;
						switch (m->Type->Type)
						{
						case DataTypes_double_t:
							o = RR_MAKE_SHARED<ArrayMemoryServiceSkel<double> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_single_t:
							o = RR_MAKE_SHARED<ArrayMemoryServiceSkel<float> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int8_t:
							o = RR_MAKE_SHARED<ArrayMemoryServiceSkel<int8_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint8_t:
							o = RR_MAKE_SHARED<ArrayMemoryServiceSkel<uint8_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int16_t:
							o = RR_MAKE_SHARED<ArrayMemoryServiceSkel<int16_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint16_t:
							o = RR_MAKE_SHARED<ArrayMemoryServiceSkel<uint16_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int32_t:
							o = RR_MAKE_SHARED<ArrayMemoryServiceSkel<int32_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint32_t:
							o = RR_MAKE_SHARED<ArrayMemoryServiceSkel<uint32_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int64_t:
							o = RR_MAKE_SHARED<ArrayMemoryServiceSkel<int64_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint64_t:
							o = RR_MAKE_SHARED<ArrayMemoryServiceSkel<uint64_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_cdouble_t:
							o = RR_MAKE_SHARED<ArrayMemoryServiceSkel<cdouble> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_csingle_t:
							o = RR_MAKE_SHARED<ArrayMemoryServiceSkel<cfloat> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_bool_t:
							o = RR_MAKE_SHARED<ArrayMemoryServiceSkel<rr_bool> >(m->Name, shared_from_this(), direction);
							break;							
						default:
							throw DataTypeException("Invalid memory data type");

						}
						memories.insert(std::make_pair(m->Name, o));

					}
					else
					{
						RR_SHARED_PTR<void> o;
						switch (m->Type->Type)
						{
						case DataTypes_double_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryServiceSkel<double> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_single_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryServiceSkel<float> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int8_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryServiceSkel<int8_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint8_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryServiceSkel<uint8_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int16_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryServiceSkel<int16_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint16_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryServiceSkel<uint16_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int32_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryServiceSkel<int32_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint32_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryServiceSkel<uint32_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_int64_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryServiceSkel<int64_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_uint64_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryServiceSkel<uint64_t> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_cdouble_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryServiceSkel<cdouble> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_csingle_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryServiceSkel<cfloat> >(m->Name, shared_from_this(), direction);
							break;
						case DataTypes_bool_t:
							o = RR_MAKE_SHARED<MultiDimArrayMemoryServiceSkel<rr_bool> >(m->Name, shared_from_this(), direction);
							break;
						default:
							throw DataTypeException("Invalid memory data type");

						}
						memories.insert(std::make_pair(m->Name, o));
					}
				}
				else
				{
					RR_SHARED_PTR<ServiceEntryDefinition> service_entry = rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType());
					if (service_entry->RRDataType() == DataTypes_pod_t)
					{
						if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
						{
							std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs;
							size_t element_size = EstimatePodPackedElementSize(rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType()), other_defs, RRGetNode());
							pod_memories.insert(std::make_pair(m->Name, RR_MAKE_SHARED<WrappedPodArrayMemoryServiceSkel>(m->Name, shared_from_this(), element_size, direction)));
						}
						else
						{
							std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs;
							size_t element_size = EstimatePodPackedElementSize(rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType()), other_defs, RRGetNode());
							pod_multidimmemories.insert(std::make_pair(m->Name, RR_MAKE_SHARED<WrappedPodMultiDimArrayMemoryServiceSkel>(m->Name, shared_from_this(), element_size, direction)));
						}
					}
					else
					{
						if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
						{
							std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs;
							boost::tuple<DataTypes, size_t> t4 = GetNamedArrayElementTypeAndCount(service_entry);
							size_t element_size = t4.get<1>();
							namedarray_memories.insert(std::make_pair(m->Name, RR_MAKE_SHARED<WrappedNamedArrayMemoryServiceSkel>(m->Name, shared_from_this(), element_size, direction)));
						}
						else
						{
							std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs;
							boost::tuple<DataTypes, size_t> t4 = GetNamedArrayElementTypeAndCount(service_entry);
							size_t element_size = t4.get<1>();
							namedarray_multidimmemories.insert(std::make_pair(m->Name, RR_MAKE_SHARED<WrappedNamedMultiDimArrayMemoryServiceSkel>(m->Name, shared_from_this(), element_size, direction)));
						}
					}
				}
			}


		}

		boost::shared_ptr<WrappedServiceSkel> sk=rr_cast<WrappedServiceSkel>(shared_from_this());
		this->RR_Director=RR_Director1;
		DIRECTOR_CALL(WrappedServiceSkelDirector,RR_Director2->Init(sk));
		

	}

	RR_INTRUSIVE_PTR<MessageEntry> WrappedServiceSkel::CallGetProperty(RR_INTRUSIVE_PTR<MessageEntry> m)
	{
		RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr=CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertyGetRes,m->MemberName);
		RR_INTRUSIVE_PTR<MessageElement> ret;
		
		DIRECTOR_CALL(WrappedServiceSkelDirector,ret=RR_Director2->CallGetProperty(m->MemberName))
		
		ret->ElementName="value";
		mr->AddElement(ret);
		return mr;
	}

	RR_INTRUSIVE_PTR<MessageEntry> WrappedServiceSkel::CallSetProperty(RR_INTRUSIVE_PTR<MessageEntry> m)
	{
		RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr=CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertySetRes,m->MemberName);
		RR_INTRUSIVE_PTR<MessageElement> m2=m->FindElement("value");
		DIRECTOR_CALL(WrappedServiceSkelDirector,RR_Director2->CallSetProperty(m->MemberName,m2))
		return mr;
	}

	RR_INTRUSIVE_PTR<MessageEntry> WrappedServiceSkel::CallFunction(RR_INTRUSIVE_PTR<MessageEntry> m)
	{
		RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr=CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallRes,m->MemberName);
		RR_INTRUSIVE_PTR<MessageElement> ret;		
		DIRECTOR_CALL(WrappedServiceSkelDirector,ret=RR_Director2->CallFunction(m->MemberName,m->elements))
		
		if (ret->ElementName != "return" && ret->ElementName != "index")
		{
			ret->ElementName = "return";
		}
		mr->AddElement(ret);
		return mr;
	}

	RR_SHARED_PTR<RRObject> WrappedServiceSkel::GetSubObj(const std::string &name, const std::string &ind)
	{
		boost::shared_ptr<WrappedRRObject> ret;
		
		DIRECTOR_CALL(WrappedServiceSkelDirector,ret=RR_Director2->GetSubObj(name,ind));
		
		return ret;
	}


	std::string WrappedServiceSkel::GetObjectType()
	{
		return castobj->RRType();
	}

	void WrappedServiceSkel::ReleaseCastObject()
	{
		for (std::map<std::string, RR_SHARED_PTR<WrappedPipeServer> >::iterator e=pipes.begin(); e!=pipes.end(); ++e)
		{
			e->second->Shutdown();
		}

		for (std::map<std::string, RR_SHARED_PTR<WrappedWireServer> >::iterator e=wires.begin(); e!=wires.end(); ++e)
		{
			e->second->Shutdown();
		}


		DIRECTOR_CALL(WrappedServiceSkelDirector,RR_Director2->ReleaseCastObject());
		//castobj.reset();

	}

	RR_SHARED_PTR<RobotRaconteur::WrappedPipeServer> WrappedServiceSkel::GetPipe(const std::string& membername)
	{
		std::map<std::string, RR_SHARED_PTR<WrappedPipeServer> >::iterator e = pipes.find(membername);
		if (e == pipes.end()) throw MemberNotFoundException("Pipe Member Not Found");
		return e->second;
	}

	RR_SHARED_PTR<RobotRaconteur::WrappedWireServer> WrappedServiceSkel::GetWire(const std::string& membername)
	{
		std::map<std::string, RR_SHARED_PTR<WrappedWireServer> >::iterator e = wires.find(membername);
		if (e == wires.end()) throw MemberNotFoundException("Wire Member Not Found");
		return e->second;
	}

	void WrappedServiceSkel::DispatchPipeMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, uint32_t endpoint)
	{
		std::map<std::string, RR_SHARED_PTR<WrappedPipeServer> >::iterator e = pipes.find(m->MemberName);
		if (e == pipes.end()) throw MemberNotFoundException("Pipe Member Not Found");
		e->second->PipePacketReceived(m, endpoint);		
	}

	void WrappedServiceSkel::DispatchWireMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, uint32_t endpoint)
	{
		std::map<std::string, RR_SHARED_PTR<WrappedWireServer> >::iterator e = wires.find(m->MemberName);
		if (e == wires.end()) throw MemberNotFoundException("Wire Member Not Found");
		e->second->WirePacketReceived(m, endpoint);		
	}


	RR_INTRUSIVE_PTR<MessageEntry> WrappedServiceSkel::CallPipeFunction(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, uint32_t endpoint)
	{
		std::map<std::string, RR_SHARED_PTR<WrappedPipeServer> >::iterator e = pipes.find(m->MemberName);
		if (e == pipes.end()) throw MemberNotFoundException("Pipe Member Not Found");
		return e->second->PipeCommand(m, endpoint);
	}

	RR_INTRUSIVE_PTR<MessageEntry> WrappedServiceSkel::CallWireFunction(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, uint32_t endpoint)
	{
		std::map<std::string, RR_SHARED_PTR<WrappedWireServer> >::iterator e = wires.find(m->MemberName);
		if (e == wires.end()) throw MemberNotFoundException("Wire Member Not Found");
		return e->second->WireCommand(m, endpoint);		
	}

	void WrappedServiceSkel::WrappedDispatchEvent(const std::string& EventName, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& args)
	{
		RR_INTRUSIVE_PTR<MessageEntry> req=CreateMessageEntry(MessageEntryType_EventReq, EventName);
		req->elements=args;
		req->ServicePath=GetServicePath();
		GetContext()->SendEvent(req);
	}

	RR_INTRUSIVE_PTR<MessageElement> WrappedServiceSkel::WrappedCallbackCall(const std::string& CallbackName, uint32_t endpoint, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& m)
	{
		RR_INTRUSIVE_PTR<MessageEntry> req=CreateMessageEntry(MessageEntryType_CallbackCallReq, CallbackName);
		req->elements=m;
		req->ServicePath=GetServicePath();
		RR_INTRUSIVE_PTR<MessageEntry> res=GetContext()->ProcessCallbackRequest(req, endpoint);
		return res->FindElement("return");
	}

	RR_INTRUSIVE_PTR<MessageEntry> WrappedServiceSkel::CallMemoryFunction(RR_INTRUSIVE_PTR<MessageEntry> mm, RR_SHARED_PTR<Endpoint> e)
	{

		if (memorytypes.count(mm->MemberName)==0) throw MemberNotFoundException("Member not found");

		boost::shared_ptr<MemoryDefinition> m=memorytypes.at(mm->MemberName);


		if (IsTypeNumeric(m->Type->Type))
		{
			if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
			{
				WrappedArrayMemoryDirector* mem;
				DIRECTOR_CALL(WrappedServiceSkelDirector, mem = RR_Director2->GetArrayMemory(mm->MemberName));

				switch (m->Type->Type)
				{
				case DataTypes_double_t:
					return boost::static_pointer_cast<ArrayMemoryServiceSkel<double> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedArrayMemory<double> >(mem));
					break;
				case DataTypes_single_t:
					return boost::static_pointer_cast<ArrayMemoryServiceSkel<float> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedArrayMemory<float> >(mem));
					break;
				case DataTypes_int8_t:
					return boost::static_pointer_cast<ArrayMemoryServiceSkel<int8_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedArrayMemory<int8_t> >(mem));
					break;
				case DataTypes_uint8_t:
					return boost::static_pointer_cast<ArrayMemoryServiceSkel<uint8_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedArrayMemory<uint8_t> >(mem));
					break;
				case DataTypes_int16_t:
					return boost::static_pointer_cast<ArrayMemoryServiceSkel<int16_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedArrayMemory<int16_t> >(mem));
					break;
				case DataTypes_uint16_t:
					return boost::static_pointer_cast<ArrayMemoryServiceSkel<uint16_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedArrayMemory<uint16_t> >(mem));
					break;
				case DataTypes_int32_t:
					return boost::static_pointer_cast<ArrayMemoryServiceSkel<int32_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedArrayMemory<int32_t> >(mem));
					break;
				case DataTypes_uint32_t:
					return boost::static_pointer_cast<ArrayMemoryServiceSkel<uint32_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedArrayMemory<uint32_t> >(mem));
					break;
				case DataTypes_int64_t:
					return boost::static_pointer_cast<ArrayMemoryServiceSkel<int64_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedArrayMemory<int64_t> >(mem));
					break;
				case DataTypes_uint64_t:
					return boost::static_pointer_cast<ArrayMemoryServiceSkel<uint64_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedArrayMemory<uint64_t> >(mem));
					break;
				case DataTypes_cdouble_t:
					return boost::static_pointer_cast<ArrayMemoryServiceSkel<cdouble> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedArrayMemory<cdouble> >(mem));
					break;
				case DataTypes_csingle_t:
					return boost::static_pointer_cast<ArrayMemoryServiceSkel<cfloat> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedArrayMemory<cfloat> >(mem));
					break;
				case DataTypes_bool_t:
					return boost::static_pointer_cast<ArrayMemoryServiceSkel<rr_bool> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedArrayMemory<rr_bool> >(mem));
					break;
				default:
					throw DataTypeException("Invalid memory data type");

				}


			}
			else
			{
				WrappedMultiDimArrayMemoryDirector* mem;
				DIRECTOR_CALL(WrappedServiceSkelDirector, mem = RR_Director2->GetMultiDimArrayMemory(mm->MemberName));

				switch (m->Type->Type)
				{
				case DataTypes_double_t:
					return boost::static_pointer_cast<MultiDimArrayMemoryServiceSkel<double> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedMultiDimArrayMemory<double> >(mem));
					break;
				case DataTypes_single_t:
					return boost::static_pointer_cast<MultiDimArrayMemoryServiceSkel<float> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedMultiDimArrayMemory<float> >(mem));
					break;
				case DataTypes_int8_t:
					return boost::static_pointer_cast<MultiDimArrayMemoryServiceSkel<int8_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedMultiDimArrayMemory<int8_t> >(mem));
					break;
				case DataTypes_uint8_t:
					return boost::static_pointer_cast<MultiDimArrayMemoryServiceSkel<uint8_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedMultiDimArrayMemory<uint8_t> >(mem));
					break;
				case DataTypes_int16_t:
					return boost::static_pointer_cast<MultiDimArrayMemoryServiceSkel<int16_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedMultiDimArrayMemory<int16_t> >(mem));
					break;
				case DataTypes_uint16_t:
					return boost::static_pointer_cast<MultiDimArrayMemoryServiceSkel<uint16_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedMultiDimArrayMemory<uint16_t> >(mem));
					break;
				case DataTypes_int32_t:
					return boost::static_pointer_cast<MultiDimArrayMemoryServiceSkel<int32_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedMultiDimArrayMemory<int32_t> >(mem));
					break;
				case DataTypes_uint32_t:
					return boost::static_pointer_cast<MultiDimArrayMemoryServiceSkel<uint32_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedMultiDimArrayMemory<uint32_t> >(mem));
					break;
				case DataTypes_int64_t:
					return boost::static_pointer_cast<MultiDimArrayMemoryServiceSkel<int64_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedMultiDimArrayMemory<int64_t> >(mem));
					break;
				case DataTypes_uint64_t:
					return boost::static_pointer_cast<MultiDimArrayMemoryServiceSkel<uint64_t> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedMultiDimArrayMemory<uint64_t> >(mem));
					break;
				case DataTypes_cdouble_t:
					return boost::static_pointer_cast<MultiDimArrayMemoryServiceSkel<cdouble> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedMultiDimArrayMemory<cdouble> >(mem));
					break;
				case DataTypes_csingle_t:
					return boost::static_pointer_cast<MultiDimArrayMemoryServiceSkel<cfloat> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedMultiDimArrayMemory<cfloat> >(mem));
					break;
				case DataTypes_bool_t:
					return boost::static_pointer_cast<MultiDimArrayMemoryServiceSkel<rr_bool> >(memories.at(mm->MemberName))->CallMemoryFunction(mm, e, boost::make_shared<WrappedMultiDimArrayMemory<rr_bool> >(mem));
					break;
				default:
					throw DataTypeException("Invalid memory data type");

				}

			}
		}
		else
		{
			RR_SHARED_PTR<ServiceEntryDefinition> service_entry = rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType());
			if (service_entry->RRDataType() == DataTypes_pod_t)
			{
				if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
				{
					WrappedPodArrayMemoryDirector* mem;
					DIRECTOR_CALL(WrappedServiceSkelDirector, mem = RR_Director2->GetPodArrayMemory(mm->MemberName));

					return pod_memories.at(mm->MemberName)->CallMemoryFunction(mm, e, boost::make_shared<WrappedPodArrayMemory>(mem));
				}
				else
				{
					WrappedPodMultiDimArrayMemoryDirector* mem;
					DIRECTOR_CALL(WrappedServiceSkelDirector, mem = RR_Director2->GetPodMultiDimArrayMemory(mm->MemberName));

					return pod_multidimmemories.at(mm->MemberName)->CallMemoryFunction(mm, e, boost::make_shared<WrappedPodMultiDimArrayMemory>(mem));
				}
			}
			else
			{
				if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
				{
					WrappedNamedArrayMemoryDirector* mem;
					DIRECTOR_CALL(WrappedServiceSkelDirector, mem = RR_Director2->GetNamedArrayMemory(mm->MemberName));

					return namedarray_memories.at(mm->MemberName)->CallMemoryFunction(mm, e, boost::make_shared<WrappedNamedArrayMemory>(mem));
				}
				else
				{
					WrappedNamedMultiDimArrayMemoryDirector* mem;
					DIRECTOR_CALL(WrappedServiceSkelDirector, mem = RR_Director2->GetNamedMultiDimArrayMemory(mm->MemberName));

					return namedarray_multidimmemories.at(mm->MemberName)->CallMemoryFunction(mm, e, boost::make_shared<WrappedNamedMultiDimArrayMemory>(mem));
				}
			}
		}

		throw InternalErrorException("Internal error");
	}

	int32_t WrappedServiceSkel::RegisterGeneratorServer(const std::string& function_name, WrappedGeneratorServerDirector* gen)
	{
		{
			boost::mutex::scoped_lock lock(generators_lock);
			int32_t rr_index = get_new_generator_index();
			generators.insert(std::make_pair(rr_index, RR_MAKE_SHARED<WrappedGeneratorServer>(function_name, rr_index, shared_from_this(), RobotRaconteur::ServerEndpoint::GetCurrentEndpoint(), gen)));
			return rr_index;
		}


	}

	/*void WrappedServiceSkel::SetRRDirector(WrappedServiceSkelDirector* director, int32_t id)
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		this->RR_Director.reset(director,boost::bind(&ReleaseDirector,_1,id));
	}*/

	// Pod array memory skels

	WrappedPodArrayMemory::WrappedPodArrayMemory(WrappedPodArrayMemoryDirector* RR_Director)
	{
		if (!RR_Director) throw InvalidArgumentException("RR_Director cannot be null");
		this->RR_Director.reset(RR_Director, boost::bind(&ReleaseDirector<WrappedPodArrayMemoryDirector>, _1, RR_Director->objectheapid));
	}

	uint64_t WrappedPodArrayMemory::Length()
	{
		uint64_t l;
		DIRECTOR_CALL(WrappedPodArrayMemoryDirector, l = RR_Director2->Length());
		return l;
	}
	DataTypes WrappedPodArrayMemory::ElementTypeID()
	{
		return DataTypes_pod_t;
	}

	RR_INTRUSIVE_PTR<MessageElementPodArray> WrappedPodArrayMemory::Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count)
	{
		RR_INTRUSIVE_PTR<MessageElementPodArray> o;
		DIRECTOR_CALL(WrappedPodArrayMemoryDirector, o = RR_Director2->Read(memorypos, bufferpos, count));
		return o;
	}
	void WrappedPodArrayMemory::Write(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementPodArray> buffer, uint64_t bufferpos, uint64_t count)
	{
		DIRECTOR_CALL(WrappedPodArrayMemoryDirector, RR_Director2->Write(memorypos, buffer, bufferpos, count));
	}


	WrappedPodArrayMemoryServiceSkel::WrappedPodArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction)
		: ArrayMemoryServiceSkelBase(membername, skel, DataTypes_pod_t, element_size, direction)
	{
	}

	RR_INTRUSIVE_PTR<MessageElementData> WrappedPodArrayMemoryServiceSkel::DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
	{
		RR_SHARED_PTR<WrappedPodArrayMemory> mem1 = rr_cast<WrappedPodArrayMemory>(mem);
		return mem1->Read(memorypos, bufferpos, count);
	}
	void WrappedPodArrayMemoryServiceSkel::DoWrite(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
	{
		RR_SHARED_PTR<WrappedPodArrayMemory> mem1 = rr_cast<WrappedPodArrayMemory>(mem);
		mem1->Write(memorypos, rr_cast<MessageElementPodArray>(buffer), bufferpos, count);
	}

	WrappedPodMultiDimArrayMemory::WrappedPodMultiDimArrayMemory(WrappedPodMultiDimArrayMemoryDirector* RR_Director)
	{
		if (!RR_Director) throw InvalidArgumentException("RR_Director cannot be null");
		this->RR_Director.reset(RR_Director, boost::bind(&ReleaseDirector<WrappedPodMultiDimArrayMemoryDirector>, _1, RR_Director->objectheapid));
	}
	std::vector<uint64_t> WrappedPodMultiDimArrayMemory::Dimensions()
	{
		std::vector<uint64_t> l;
		DIRECTOR_CALL(WrappedPodMultiDimArrayMemoryDirector, l = RR_Director2->Dimensions());
		return l;
	}
	uint64_t WrappedPodMultiDimArrayMemory::DimCount()
	{
		uint64_t l;
		DIRECTOR_CALL(WrappedPodMultiDimArrayMemoryDirector, l = RR_Director2->DimCount());
		return l;
	}
	bool WrappedPodMultiDimArrayMemory::Complex()
	{
		return false;
	}
	DataTypes WrappedPodMultiDimArrayMemory::ElementTypeID()
	{
		return DataTypes_pod_t;
	}
	RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> WrappedPodMultiDimArrayMemory::Read(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
	{
		RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> o;
		DIRECTOR_CALL(WrappedPodMultiDimArrayMemoryDirector, o = RR_Director2->Read(memorypos, bufferpos, count));
		return o;
	}
	void WrappedPodMultiDimArrayMemory::Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
	{
		DIRECTOR_CALL(WrappedPodMultiDimArrayMemoryDirector, RR_Director2->Write(memorypos, buffer, bufferpos, count));
	}

	WrappedPodMultiDimArrayMemoryServiceSkel::WrappedPodMultiDimArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction)
		: MultiDimArrayMemoryServiceSkelBase(membername, skel, DataTypes_pod_t, element_size, direction)
	{

	}
	RR_INTRUSIVE_PTR<MessageElementData> WrappedPodMultiDimArrayMemoryServiceSkel::DoRead(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elem_count, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
	{
		RR_SHARED_PTR<WrappedPodMultiDimArrayMemory> mem1 = rr_cast<WrappedPodMultiDimArrayMemory>(mem);
		return mem1->Read(memorypos, bufferpos, count);
	}
	void WrappedPodMultiDimArrayMemoryServiceSkel::DoWrite(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elem_count, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
	{
		RR_SHARED_PTR<WrappedPodMultiDimArrayMemory> mem1 = rr_cast<WrappedPodMultiDimArrayMemory>(mem);
		mem1->Write(memorypos, rr_cast<MessageElementPodMultiDimArray>(buffer), bufferpos, count);
	}


	// WrappedNamedArray Service Skels
	WrappedNamedArrayMemoryServiceSkel::WrappedNamedArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction)
		: ArrayMemoryServiceSkelBase(membername, skel, DataTypes_namedarray_t, element_size, direction)
	{
	}

	RR_INTRUSIVE_PTR<MessageElementData> WrappedNamedArrayMemoryServiceSkel::DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
	{
		RR_SHARED_PTR<WrappedNamedArrayMemory> mem1 = rr_cast<WrappedNamedArrayMemory>(mem);
		return mem1->Read(memorypos, bufferpos, count);
	}
	void WrappedNamedArrayMemoryServiceSkel::DoWrite(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
	{
		RR_SHARED_PTR<WrappedNamedArrayMemory> mem1 = rr_cast<WrappedNamedArrayMemory>(mem);
		mem1->Write(memorypos, rr_cast<MessageElementNamedArray>(buffer), bufferpos, count);
	}

	WrappedNamedArrayMemory::WrappedNamedArrayMemory(WrappedNamedArrayMemoryDirector* RR_Director)
	{
		if (!RR_Director) throw InvalidArgumentException("RR_Director cannot be null");
		this->RR_Director.reset(RR_Director, boost::bind(&ReleaseDirector<WrappedNamedArrayMemoryDirector>, _1, RR_Director->objectheapid));
	}

	uint64_t WrappedNamedArrayMemory::Length()
	{
		uint64_t l;
		DIRECTOR_CALL(WrappedNamedArrayMemoryDirector, l = RR_Director2->Length());
		return l;
	}
	DataTypes WrappedNamedArrayMemory::ElementTypeID()
	{
		return DataTypes_namedarray_t;
	}

	RR_INTRUSIVE_PTR<MessageElementNamedArray> WrappedNamedArrayMemory::Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count)
	{
		RR_INTRUSIVE_PTR<MessageElementNamedArray> o;
		DIRECTOR_CALL(WrappedNamedArrayMemoryDirector, o = RR_Director2->Read(memorypos, bufferpos, count));
		return o;
	}
	void WrappedNamedArrayMemory::Write(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementNamedArray> buffer, uint64_t bufferpos, uint64_t count)
	{
		DIRECTOR_CALL(WrappedNamedArrayMemoryDirector, RR_Director2->Write(memorypos, buffer, bufferpos, count));
	}

	WrappedNamedMultiDimArrayMemory::WrappedNamedMultiDimArrayMemory(WrappedNamedMultiDimArrayMemoryDirector* RR_Director)
	{
		if (!RR_Director) throw InvalidArgumentException("RR_Director cannot be null");
		this->RR_Director.reset(RR_Director, boost::bind(&ReleaseDirector<WrappedNamedMultiDimArrayMemoryDirector>, _1, RR_Director->objectheapid));
	}
	std::vector<uint64_t> WrappedNamedMultiDimArrayMemory::Dimensions()
	{
		std::vector<uint64_t> l;
		DIRECTOR_CALL(WrappedNamedMultiDimArrayMemoryDirector, l = RR_Director2->Dimensions());
		return l;
	}
	uint64_t WrappedNamedMultiDimArrayMemory::DimCount()
	{
		uint64_t l;
		DIRECTOR_CALL(WrappedNamedMultiDimArrayMemoryDirector, l = RR_Director2->DimCount());
		return l;
	}
	bool WrappedNamedMultiDimArrayMemory::Complex()
	{
		return false;
	}
	DataTypes WrappedNamedMultiDimArrayMemory::ElementTypeID()
	{
		return DataTypes_pod_t;
	}
	RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> WrappedNamedMultiDimArrayMemory::Read(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
	{
		RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> o;
		DIRECTOR_CALL(WrappedNamedMultiDimArrayMemoryDirector, o = RR_Director2->Read(memorypos, bufferpos, count));
		return o;
	}
	void WrappedNamedMultiDimArrayMemory::Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
	{
		DIRECTOR_CALL(WrappedNamedMultiDimArrayMemoryDirector, RR_Director2->Write(memorypos, buffer, bufferpos, count));
	}

	WrappedNamedMultiDimArrayMemoryServiceSkel::WrappedNamedMultiDimArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction)
		: MultiDimArrayMemoryServiceSkelBase(membername, skel, DataTypes_namedarray_t, element_size, direction)
	{

	}
	RR_INTRUSIVE_PTR<MessageElementData> WrappedNamedMultiDimArrayMemoryServiceSkel::DoRead(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elem_count, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
	{
		RR_SHARED_PTR<WrappedNamedMultiDimArrayMemory> mem1 = rr_cast<WrappedNamedMultiDimArrayMemory>(mem);
		return mem1->Read(memorypos, bufferpos, count);
	}
	void WrappedNamedMultiDimArrayMemoryServiceSkel::DoWrite(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elem_count, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
	{
		RR_SHARED_PTR<WrappedNamedMultiDimArrayMemory> mem1 = rr_cast<WrappedNamedMultiDimArrayMemory>(mem);
		mem1->Write(memorypos, rr_cast<MessageElementNamedMultiDimArray>(buffer), bufferpos, count);
	}

	//WrappedRRObject

	WrappedRRObject::WrappedRRObject(const std::string& type, WrappedServiceSkelDirector* RR_Director, int32_t id)
	{
		this->Type=type;
		//std::cout  << "objid=" << id << std::endl;
		this->RR_Director.reset(RR_Director, boost::bind(&ReleaseDirector<WrappedServiceSkelDirector>, _1, id));
	}
	std::string WrappedRRObject::RRType()
	{
		return Type;
	}

	WrappedRRObject::~WrappedRRObject()
	{
		
	}

	boost::shared_ptr<WrappedServiceSkelDirector> WrappedRRObject::GetRRDirector()
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		return RR_Director;
	}

	

	boost::thread_specific_ptr<RR_INTRUSIVE_PTR<MessageEntry> > RRDirectorExceptionHelper::last_err;

	void RRDirectorExceptionHelper::Reset()
	{
		last_err.reset();
	}
	void RRDirectorExceptionHelper::SetError(RR_INTRUSIVE_PTR<MessageEntry> err)
	{
		last_err.reset(new RR_INTRUSIVE_PTR<MessageEntry>(err));
	}
	
	bool RRDirectorExceptionHelper::IsErrorPending()
	{
		return last_err.get()!=NULL;
	}
	
	RR_INTRUSIVE_PTR<MessageEntry> RRDirectorExceptionHelper::GetError()
	{
		return *last_err.get();
	}


    void AsyncStubReturn_handler(RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncStubReturnDirector> handler)
	{
		if (err)
		{
			DIRECTOR_CALL2(handler->handler(RR_SHARED_PTR<WrappedServiceStub>(),err->ErrorCode,err->Error,err->Message));
			return;
		}

		RR_SHARED_PTR<WrappedServiceStub> stub=boost::dynamic_pointer_cast<WrappedServiceStub>(obj);

		DIRECTOR_CALL2(handler->handler(stub,0,"",""));
	}

	void AsyncVoidNoErrReturn_handler(RR_SHARED_PTR<AsyncVoidNoErrReturnDirector> handler)
	{
		DIRECTOR_CALL2(handler->handler());
	}

	void AsyncStringReturn_handler(RR_SHARED_PTR<std::string> str, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<AsyncStringReturnDirector> handler)
	{
		if (err)
		{
			std::string ret = "";
			DIRECTOR_CALL2(handler->handler(ret,err->ErrorCode,err->Error,err->Message));
			return;
		}

		std::string* str2=str.get();

		DIRECTOR_CALL2(handler->handler(*str2,0,"",""));
	}

	void WrappedExceptionHandler(const std::exception* err, RR_SHARED_PTR<AsyncVoidReturnDirector> handler)
	{
		if (err==NULL) return;
		try
		{
			const RobotRaconteurException* rrerr=dynamic_cast<const RobotRaconteurException*>(err);
			if (rrerr)
			{
				DIRECTOR_CALL2(handler->handler(rrerr->ErrorCode,rrerr->Error,rrerr->Message));
			}
			else
			{
				DIRECTOR_CALL2(handler->handler(MessageErrorType_UnknownError,std::string(typeid(*err).name()),err->what()));
			}
		}
		catch (std::exception&) {}

	}

	void TimerHandlerFunc(const TimerEvent& ev, RR_SHARED_PTR<AsyncTimerEventReturnDirector> d)
	{
		TimerEvent ev2=ev;
		DIRECTOR_CALL2(d->handler(ev2,0,"","");)
	}

	void WrappedUserAuthenticator::SetRRDirector(WrappedUserAuthenticatorDirector* director, int id)
	{
		RR_Director.reset(director, boost::bind(&ReleaseDirector<WrappedUserAuthenticatorDirector>, _1, id));
	}

	RR_SHARED_PTR<AuthenticatedUser> WrappedUserAuthenticator::AuthenticateUser(const std::string &username, const std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > &credentials, RR_SHARED_PTR<ServerContext> context)
	{
		if (!RR_Director) throw InvalidOperationException("Director not set for authenticator");
		RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > cred1=AllocateEmptyRRMap<std::string,RRValue>();
		cred1->GetStorageContainer() =credentials;
		RR_INTRUSIVE_PTR<MessageElement> cred2=CreateMessageElement("credentials",context->GetNode()->PackMapType<std::string,RRValue>(cred1));
		RR_SHARED_PTR<AuthenticatedUser> ares2;
		DIRECTOR_CALL2(ares2=RR_Director->AuthenticateUser(username,cred2,context));		
		return ares2;
	}
		

	// Subscriptions

	bool WrappedServiceSubscriptionFilterPredicateDirector::CallPredicate(const ServiceInfo2& info)
	{
		ServiceInfo2Wrapped info2(info);

		bool res;
		DIRECTOR_CALL2(res = this->Predicate(info2));
		return res;
	}

	void WrappedServiceSubscriptionFilter::SetRRPredicateDirector(WrappedServiceSubscriptionFilterPredicateDirector* director, int32_t id)
	{
		Predicate.reset(director, boost::bind(&ReleaseDirector<WrappedServiceSubscriptionFilterPredicateDirector>, _1, id));
	}


	WrappedServiceInfo2Subscription::WrappedServiceInfo2Subscription(RR_SHARED_PTR<ServiceInfo2Subscription> subscription)
	{
		this->subscription = subscription;
	}

	std::map<ServiceSubscriptionClientID, ServiceInfo2Wrapped > WrappedServiceInfo2Subscription::GetDetectedServiceInfo2()
	{

		std::map<ServiceSubscriptionClientID, ServiceInfo2> a = subscription->GetDetectedServiceInfo2();
		std::map<ServiceSubscriptionClientID, ServiceInfo2Wrapped > b;

		typedef std::map<ServiceSubscriptionClientID, ServiceInfo2>::value_type e_type;

		BOOST_FOREACH(const e_type& e, a)
		{
			b.insert(std::make_pair(e.first, ServiceInfo2Wrapped(e.second)));
		}

		return b;
	}

	void WrappedServiceInfo2Subscription::Close()
	{
		{
			boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
			RR_Director.reset();
		}

		subscription->Close();
	}

	void WrappedServiceInfo2Subscription::SetRRDirector(WrappedServiceInfo2SubscriptionDirector* director, int32_t id)
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		RR_Director.reset(director, boost::bind(&ReleaseDirector<WrappedServiceInfo2SubscriptionDirector>, _1, id));
		if (!events_connected)
		{
			events_connected.data() = true;
			RR_WEAK_PTR<WrappedServiceInfo2Subscription> weak_this = shared_from_this();
			subscription->AddServiceDetectedListener(boost::bind(&WrappedServiceInfo2Subscription::ServiceDetected, weak_this, _1, _2, _3));
			subscription->AddServiceLostListener(boost::bind(&WrappedServiceInfo2Subscription::ServiceLost, weak_this, _1, _2, _3));
		}
	}

	void WrappedServiceInfo2Subscription::ServiceDetected(RR_WEAK_PTR<WrappedServiceInfo2Subscription> this_, RR_SHARED_PTR<ServiceInfo2Subscription> subscription, const ServiceSubscriptionClientID& id, const ServiceInfo2& info)
	{
		RR_SHARED_PTR<WrappedServiceInfo2Subscription> this1 = this_.lock();
		if (!this1) return;
		this1->ServiceDetected1(subscription, id, info);
	}

	void WrappedServiceInfo2Subscription::ServiceLost(RR_WEAK_PTR<WrappedServiceInfo2Subscription> this_, RR_SHARED_PTR<ServiceInfo2Subscription> subscription, const ServiceSubscriptionClientID& id, const ServiceInfo2& info)
	{
		RR_SHARED_PTR<WrappedServiceInfo2Subscription> this1 = this_.lock();
		if (!this1) return;
		this1->ServiceLost1(subscription, id, info);
	}

	void WrappedServiceInfo2Subscription::ServiceDetected1(RR_SHARED_PTR<ServiceInfo2Subscription>& subscription, const ServiceSubscriptionClientID& id, const ServiceInfo2& info)
	{
		RR_SHARED_PTR<WrappedServiceInfo2Subscription> s = shared_from_this();
		ServiceInfo2Wrapped info2(info);
		DIRECTOR_CALL3(WrappedServiceInfo2SubscriptionDirector, RR_Director->ServiceDetected(s, id, info2));
	}

	void WrappedServiceInfo2Subscription::ServiceLost1(RR_SHARED_PTR<ServiceInfo2Subscription>& subscription, const ServiceSubscriptionClientID& id, const ServiceInfo2& info)
	{
		RR_SHARED_PTR<WrappedServiceInfo2Subscription> s = shared_from_this();
		ServiceInfo2Wrapped info2(info);
		DIRECTOR_CALL3(WrappedServiceInfo2SubscriptionDirector, RR_Director->ServiceLost(s, id, info2));
	}
	
	WrappedServiceSubscription::WrappedServiceSubscription(RR_SHARED_PTR<ServiceSubscription> subscription)
	{
		this->subscription = subscription;
	}

	std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<WrappedServiceStub> > WrappedServiceSubscription::GetConnectedClients()
	{
		std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<WrappedServiceStub> > o;
		typedef std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> >::value_type e_type;
		std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> > a = subscription->GetConnectedClients();
		BOOST_FOREACH(e_type& e, a)
		{
			RR_SHARED_PTR<WrappedServiceStub> e1 = RR_DYNAMIC_POINTER_CAST<WrappedServiceStub>(e.second);

			if (!e1) continue;
			o.insert(std::make_pair(e.first, e1));			
		}

		return o;
	}

	void WrappedServiceSubscription::Close()
	{
		{
			boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
			RR_Director.reset();
		}

		subscription->Close();
	}

	void WrappedServiceSubscription::ClaimClient(RR_SHARED_PTR<WrappedServiceStub> client)
	{
		subscription->ClaimClient(client);
	}

	void WrappedServiceSubscription::ReleaseClient(RR_SHARED_PTR<WrappedServiceStub> client)
	{
		subscription->ReleaseClient(client);
	}

	uint32_t WrappedServiceSubscription::GetConnectRetryDelay()
	{
		return subscription->GetConnectRetryDelay();
	}

	void WrappedServiceSubscription::SetConnectRetryDelay(uint32_t delay_milliseconds)
	{
		subscription->SetConnectRetryDelay(delay_milliseconds);
	}

	RR_SHARED_PTR<WrappedWireSubscription> WrappedServiceSubscription::SubscribeWire(const std::string& membername)
	{
		RR_SHARED_PTR<WrappedWireSubscription> o = RR_MAKE_SHARED<WrappedWireSubscription>(subscription, membername);
		detail::ServiceSubscription_custom_member_subscribers::SubscribeWire(subscription, o);
		return o;
	}

	RR_SHARED_PTR<WrappedPipeSubscription> WrappedServiceSubscription::SubscribePipe(const std::string& membername, uint32_t max_recv_packets)
	{
		RR_SHARED_PTR<WrappedPipeSubscription> o = RR_MAKE_SHARED<WrappedPipeSubscription>(subscription, membername, max_recv_packets);
		detail::ServiceSubscription_custom_member_subscribers::SubscribePipe(subscription, o);
		return o;
	}

	void WrappedServiceSubscription::SetRRDirector(WrappedServiceSubscriptionDirector* director, int32_t id)
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		RR_Director.reset(director, boost::bind(&ReleaseDirector<WrappedServiceSubscriptionDirector>, _1, id));
		if (!events_connected)
		{
			events_connected.data() = true;
			RR_WEAK_PTR<WrappedServiceSubscription> weak_this=shared_from_this();
			subscription->AddClientConnectListener(boost::bind(&WrappedServiceSubscription::ClientConnected, weak_this, _1, _2, _3));
			subscription->AddClientDisconnectListener(boost::bind(&WrappedServiceSubscription::ClientDisconnected, weak_this, _1, _2, _3));
		}
	}

	void WrappedServiceSubscription::ClientConnected(RR_WEAK_PTR<WrappedServiceSubscription> this_, RR_SHARED_PTR<ServiceSubscription> subscription, const ServiceSubscriptionClientID& id, RR_SHARED_PTR<RRObject> client)
	{
		RR_SHARED_PTR<WrappedServiceSubscription> this1 = this_.lock();
		if (!this1) return;
		this1->ClientConnected1(subscription, id, client);
	}
	void WrappedServiceSubscription::ClientDisconnected(RR_WEAK_PTR<WrappedServiceSubscription> this_, RR_SHARED_PTR<ServiceSubscription> subscription, const ServiceSubscriptionClientID& id, RR_SHARED_PTR<RRObject> client)
	{
		RR_SHARED_PTR<WrappedServiceSubscription> this1 = this_.lock();
		if (!this1) return;
		this1->ClientDisconnected1(subscription, id, client);
	}

	void WrappedServiceSubscription::ClientConnected1(RR_SHARED_PTR<ServiceSubscription>& subscription, const ServiceSubscriptionClientID& id, RR_SHARED_PTR<RRObject>& client)
	{
		RR_SHARED_PTR<WrappedServiceSubscription> s = shared_from_this();
		RR_SHARED_PTR<WrappedServiceStub> client2 = RR_DYNAMIC_POINTER_CAST<WrappedServiceStub>(client);
		DIRECTOR_CALL3(WrappedServiceSubscriptionDirector, RR_Director->ClientConnected(s, id, client2));
	}
	void WrappedServiceSubscription::ClientDisconnected1(RR_SHARED_PTR<ServiceSubscription>& subscription, const ServiceSubscriptionClientID& id, RR_SHARED_PTR<RRObject>& client)
	{
		RR_SHARED_PTR<WrappedServiceSubscription> s = shared_from_this();
		RR_SHARED_PTR<WrappedServiceStub> client2 = RR_DYNAMIC_POINTER_CAST<WrappedServiceStub>(client);
		DIRECTOR_CALL3(WrappedServiceSubscriptionDirector, RR_Director->ClientDisconnected(s, id, client2));
	}

	WrappedWireSubscription::WrappedWireSubscription(RR_SHARED_PTR<ServiceSubscription> parent, const std::string& membername)
		: WireSubscriptionBase(parent, membername)
	{
		
	}

	WrappedService_typed_packet WrappedWireSubscription::GetInValue(TimeSpec* time)
	{
		RR_SHARED_PTR<WireConnectionBase> connection1;

		WrappedService_typed_packet o;

		o.packet = RR_STATIC_POINTER_CAST<MessageElement>(GetInValueBase(time, &connection1));
		if (!connection1) throw InvalidOperationException("Invalid subscription wire client");
		RR_SHARED_PTR<WrappedWireConnection> connection2 = rr_cast<WrappedWireConnection>(connection1);
		o.type = connection2->Type;
		o.stub = connection2->GetStub();
		return o;		
	}

	bool WrappedWireSubscription::TryGetInValue(WrappedService_typed_packet& val, TimeSpec* time)
	{
		RR_SHARED_PTR<WireConnectionBase> connection1;		

		RR_INTRUSIVE_PTR<RRValue> packet1;
		bool ret = TryGetInValueBase(packet1, time, &connection1);
		if (!ret) return false;
		val.packet = RR_STATIC_POINTER_CAST<MessageElement>(packet1);
		if (!connection1) throw InvalidOperationException("Invalid subscription wire client");
		RR_SHARED_PTR<WrappedWireConnection> connection2 = rr_cast<WrappedWireConnection>(connection1);
		val.type = connection2->Type;
		val.stub = connection2->GetStub();
		return ret;
	}
		
	void WrappedWireSubscription::SetRRDirector(WrappedWireSubscriptionDirector* director, int32_t id)
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		RR_Director.reset(director, boost::bind(&ReleaseDirector<WrappedWireSubscriptionDirector>, _1, id));		
	}

	
	void WrappedWireSubscription::fire_WireValueChanged(RR_INTRUSIVE_PTR<RRValue> value, const TimeSpec& time, RR_SHARED_PTR<WireConnectionBase> connection)
	{
		WrappedService_typed_packet val;
		val.packet = RR_STATIC_POINTER_CAST<MessageElement>(value);
		RR_SHARED_PTR<WrappedWireConnection> connection2 = rr_cast<WrappedWireConnection>(connection);
		val.type = connection2->Type;
		val.stub = connection2->GetStub();
		RR_SHARED_PTR<WrappedWireSubscription> s = RR_STATIC_POINTER_CAST<WrappedWireSubscription>(shared_from_this());
		DIRECTOR_CALL3(WrappedWireSubscriptionDirector, RR_Director->WireValueChanged(s, val, time));
	}

	WrappedWireSubscription_send_iterator::WrappedWireSubscription_send_iterator(const RR_SHARED_PTR<WrappedWireSubscription>& sub)
		: iter(sub)
	{

	}

	RR_SHARED_PTR<WrappedWireConnection> WrappedWireSubscription_send_iterator::Next()
	{
		current_connection = RR_DYNAMIC_POINTER_CAST<WrappedWireConnection>(iter.Next());
		return current_connection;
	}

	RR_SHARED_PTR<WrappedServiceStub> WrappedWireSubscription_send_iterator::GetStub()
	{
		if (!current_connection)
		{
			return RR_SHARED_PTR<WrappedServiceStub>();
		}
		return current_connection->GetStub();
	}

	void WrappedWireSubscription_send_iterator::SetOutValue(const RR_INTRUSIVE_PTR<MessageElement>& value)
	{
		iter.SetOutValue(value);
	}

	RR_SHARED_PTR<TypeDefinition> WrappedWireSubscription_send_iterator::GetType()
	{
		if (!current_connection)
		{
			throw InvalidOperationException("Invalid operation");
		}

		return current_connection->Type;
	}

	WrappedWireSubscription_send_iterator::~WrappedWireSubscription_send_iterator()
	{

	}


	WrappedPipeSubscription::WrappedPipeSubscription(RR_SHARED_PTR<ServiceSubscription> parent, const std::string& membername, int32_t max_recv_packets, int32_t max_send_backlog)
		: PipeSubscriptionBase(parent, membername, max_recv_packets, max_send_backlog)
	{
		
	}

	WrappedService_typed_packet WrappedPipeSubscription::ReceivePacket()
	{
		WrappedService_typed_packet o;
		bool ret = TryReceivePacket(o);
		if (!ret)
		{
			throw InvalidOperationException("PipeSubscription Receive Queue Empty");
		}
		return o;
	}
	bool WrappedPipeSubscription::TryReceivePacket(WrappedService_typed_packet& packet)
	{
		return TryReceivePacketWait(packet, -1, false);
	}
	bool WrappedPipeSubscription::TryReceivePacketWait(WrappedService_typed_packet& packet, int32_t timeout, bool peek)
	{
		RR_SHARED_PTR<PipeEndpointBase> endpoint1;
		RR_INTRUSIVE_PTR<RRValue> packet1;
		bool ret = TryReceivePacketBaseWait(packet1, timeout, peek, &endpoint1);
		if (!ret) return false;
		packet.packet = RR_STATIC_POINTER_CAST<MessageElement>(packet.packet);
		if (!endpoint1) throw InvalidOperationException("Invalid subscription pipe endpoint");
		RR_SHARED_PTR<WrappedPipeEndpoint> endpoint2 = rr_cast<WrappedPipeEndpoint>(endpoint1);
		packet.type = endpoint2->Type;
		packet.stub = endpoint2->GetStub();
		return ret;
	}
		
	void WrappedPipeSubscription::SetRRDirector(WrappedPipeSubscriptionDirector* director, int32_t id)
	{
		boost::unique_lock<boost::shared_mutex> lock(RR_Director_lock);
		RR_Director.reset(director, boost::bind(&ReleaseDirector<WrappedPipeSubscriptionDirector>, _1, id));		
	}

	
	void WrappedPipeSubscription::fire_PipePacketReceived()
	{
		RR_SHARED_PTR<WrappedPipeSubscription> s = RR_STATIC_POINTER_CAST<WrappedPipeSubscription>(shared_from_this());
		DIRECTOR_CALL3(WrappedPipeSubscriptionDirector, RR_Director->PipePacketReceived(s));
	}

	WrappedPipeSubscription_send_iterator::WrappedPipeSubscription_send_iterator(const RR_SHARED_PTR<WrappedPipeSubscription>& sub)
		: iter(sub)
	{

	}

	RR_SHARED_PTR<WrappedPipeEndpoint> WrappedPipeSubscription_send_iterator::Next()
	{
		current_connection = RR_DYNAMIC_POINTER_CAST<WrappedPipeEndpoint>(iter.Next());
		return current_connection;
	}

	void WrappedPipeSubscription_send_iterator::AsyncSendPacket(const RR_INTRUSIVE_PTR<MessageElement>& value)
	{
		iter.AsyncSendPacket(value);
	}

	RR_SHARED_PTR<TypeDefinition> WrappedPipeSubscription_send_iterator::GetType()
	{
		if (!current_connection)
		{
			throw InvalidOperationException("Invalid operation");
		}
		return current_connection->Type;
	}

	RR_SHARED_PTR<WrappedServiceStub> WrappedPipeSubscription_send_iterator::GetStub()
	{

		if (!current_connection)
		{
			return RR_SHARED_PTR<WrappedServiceStub>();
		}
		return current_connection->GetStub();
	}

	WrappedPipeSubscription_send_iterator::~WrappedPipeSubscription_send_iterator()
	{

	}

	std::vector<ServiceSubscriptionClientID> WrappedServiceInfo2SubscriptionServicesToVector(std::map<ServiceSubscriptionClientID, ServiceInfo2Wrapped >& infos)
	{
		std::vector<ServiceSubscriptionClientID> o;
		BOOST_FOREACH(const ServiceSubscriptionClientID& id, infos | boost::adaptors::map_keys)
		{
			o.push_back(id);
		}
		return o;
	}

	std::vector<ServiceSubscriptionClientID> WrappedServiceSubscriptionClientsToVector(std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<WrappedServiceStub> >& clients)
	{
		std::vector<ServiceSubscriptionClientID> o;
		BOOST_FOREACH(const ServiceSubscriptionClientID& id, clients | boost::adaptors::map_keys)
		{
			o.push_back(id);
		}
		return o;
	}

	static RR_SHARED_PTR<ServiceSubscriptionFilter> WrappedSubscribeService_LoadFilter(RR_SHARED_PTR<RobotRaconteurNode>& node, RR_SHARED_PTR<WrappedServiceSubscriptionFilter>& filter)
	{
		RR_SHARED_PTR<ServiceSubscriptionFilter> filter2;
		if (filter)
		{
			filter2 = RR_MAKE_SHARED<ServiceSubscriptionFilter>();
			filter2->ServiceNames = filter->ServiceNames;
			filter2->TransportSchemes = filter->TransportSchemes;
			filter2->MaxConnections = filter->MaxConnections;
			BOOST_FOREACH(RR_SHARED_PTR<WrappedServiceSubscriptionFilterNode>& n, filter->Nodes)
			{
				if (!n) continue;
				RR_SHARED_PTR<ServiceSubscriptionFilterNode> n2 = RR_MAKE_SHARED<ServiceSubscriptionFilterNode>();
				n2->NodeID = n->NodeID;
				n2->NodeName = n->NodeName;
				n2->Username = n->Username;
				n2->Credentials = node->UnpackMapType<std::string, RRValue>(RR_DYNAMIC_POINTER_CAST<MessageElementMap<std::string> >(n->Credentials));
				filter2->Nodes.push_back(n2);
			}

			if (filter->Predicate)
			{
				filter2->Predicate = boost::bind(&WrappedServiceSubscriptionFilterPredicateDirector::CallPredicate, filter->Predicate, _1);
			}
		}

		return filter2;
	}

	RR_SHARED_PTR<WrappedServiceInfo2Subscription> WrappedSubscribeServiceInfo2(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<std::string>& service_types, RR_SHARED_PTR<WrappedServiceSubscriptionFilter> filter)
	{
		RR_SHARED_PTR<ServiceSubscriptionFilter> filter2 = WrappedSubscribeService_LoadFilter(node, filter);

		RR_SHARED_PTR<ServiceInfo2Subscription> sub = node->SubscribeServiceInfo2(service_types, filter2);

		return RR_MAKE_SHARED<WrappedServiceInfo2Subscription>(sub);
	}

	RR_SHARED_PTR<WrappedServiceSubscription> WrappedSubscribeService(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<std::string>& service_types, RR_SHARED_PTR<WrappedServiceSubscriptionFilter> filter)
	{
		
		RR_SHARED_PTR<ServiceSubscriptionFilter> filter2=WrappedSubscribeService_LoadFilter(node, filter);

		RR_SHARED_PTR<ServiceSubscription> sub = node->SubscribeService(service_types, filter2);

		return RR_MAKE_SHARED<WrappedServiceSubscription>(sub);
	}

}
