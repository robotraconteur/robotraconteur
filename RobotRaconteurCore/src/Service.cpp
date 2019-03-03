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

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "RobotRaconteur/Service.h"
#include "RobotRaconteur/ServiceDefinition.h"
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/Error.h"
#include <boost/algorithm/string.hpp>
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/AutoResetEvent.h"
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/Security.h"

#include "Service_lock_private.h"

#include "RobotRaconteur/ErrorUtil.h"
#include "RobotRaconteur/Generator.h"

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>


#undef SendMessage


namespace RobotRaconteur
{
	static void rr_context_emptyhandler(RR_SHARED_PTR<RobotRaconteurException>)
	{

	}

	static void rr_context_node_handler(RR_SHARED_PTR<RobotRaconteurNode> n,RR_SHARED_PTR<RobotRaconteurException> e)
	{
		n->HandleException(e.get());
	}
		
	void ServiceSkel::Init(const std::string &s, RR_SHARED_PTR<RRObject> o, RR_SHARED_PTR<ServerContext> c)
	{
		InitializeInstanceFields();
		m_ServicePath = s;
		m_context = c;
		uncastobj = o;
		if (o == 0)
			throw InvalidArgumentException("");

		this->node=c->GetNode();

		RegisterEvents(o);
		InitPipeServers(o);
		InitWireServers(o);
		InitCallbackServers(o);

		monitorlock.reset();
		objectlock.reset();

		std::string object_type_q = GetObjectType();
		std::string object_def;
		std::string object_type;

		boost::tie(object_def, object_type) = SplitQualifiedName(object_type_q);
		RR_SHARED_PTR<ServiceDefinition> d = RRGetNode()->GetServiceType(object_def)->ServiceDef();
		object_type_ver.push_back(boost::make_tuple(d->StdVer, object_type_q));
		if (d->StdVer)
		{
			RR_SHARED_PTR<ServiceEntryDefinition> e = TryFindByName(d->Objects, object_type);
			if (e)
			{
				std::set<std::string> found_defs;
				std::set<RobotRaconteurVersion> found_versions;
				BOOST_FOREACH(const std::string& s, e->Implements)
				{
					if (!boost::contains(s, "."))
						continue;
					
					std::string implement_def;
					boost::tie(implement_def, boost::tuples::ignore) = SplitQualifiedName(s);
					bool implement_def_b;
					boost::tie(boost::tuples::ignore, implement_def_b) = found_defs.insert(implement_def);
					if (!implement_def_b)
						continue;

					RR_SHARED_PTR<ServiceDefinition> d2 = RRGetNode()->GetServiceType(implement_def)->ServiceDef();
					bool version_b;
					boost::tie(boost::tuples::ignore, version_b) = found_versions.insert(d2->StdVer);
					if (!version_b)
						continue;

					object_type_ver.push_back(boost::make_tuple(d2->StdVer, s));

					if (!d2->StdVer)
						break;

				}
			}
		}

	}

	RR_SHARED_PTR<RobotRaconteurNode> ServiceSkel::RRGetNode()
	{
		RR_SHARED_PTR<RobotRaconteurNode> n=node.lock();
		if (!n) throw InvalidOperationException("Node has been released");
		return n;
	}

	RR_WEAK_PTR<RobotRaconteurNode> ServiceSkel::RRGetNodeWeak()
	{
		return node;
	}

	void ServiceSkel::InitCallbackServers(RR_SHARED_PTR<RRObject> o)
	{
	}

	std::string ServiceSkel::GetServicePath() const
	{
		return m_ServicePath;
	}

	RR_SHARED_PTR<ServerContext> ServiceSkel::GetContext()
	{
		RR_SHARED_PTR<ServerContext> out=m_context.lock();
		if (!out) throw InvalidOperationException("Skel has been released");
		return out;
	}

	RR_SHARED_PTR<RRObject> ServiceSkel::GetUncastObject() const
	{
		return uncastobj;
	}

	RR_SHARED_PTR<RRObject> ServiceSkel::GetSubObj(const std::string &name)
	{		
		std::vector<std::string> s1;
		boost::split(s1,name,boost::is_from_range('[','['));
		//= name.Split(std::vector<int8_t>(tempVector2, tempVector2 + sizeof(tempVector2) / sizeof(tempVector2[0])));
		if (s1.size() == 1)
		{
			return GetSubObj(name, "");
		}
		else
		{
			std::string ind = detail::decode_index(boost::replace_last_copy(s1.at(1),"]",""));
			return GetSubObj(s1.at(0), ind);
		}
	}

	void ServiceSkel::RegisterEvents(RR_SHARED_PTR<RRObject> obj1)
	{
	

	}

	void ServiceSkel::UnregisterEvents(RR_SHARED_PTR<RRObject> obj1)
	{
		
	}

	void ServiceSkel::InitPipeServers(RR_SHARED_PTR<RRObject> obj1)
	{
	}

	void ServiceSkel::InitWireServers(RR_SHARED_PTR<RRObject> obj1)
	{
	}

	void ServiceSkel::ObjRefChanged(const std::string &name)
	{
		std::string path = GetServicePath() + "." + name;
		GetContext()->ReplaceObject(path);
	}

	void ServiceSkel::EndAsyncCallGetProperty(RR_WEAK_PTR<ServiceSkel> skel, RR_INTRUSIVE_PTR<MessageElement> value, RR_SHARED_PTR<RobotRaconteurException> err, RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep)
	{
		RR_SHARED_PTR<ServiceSkel> skel1=skel.lock();
		if (!skel1) return;
		

		try
		{
			RR_INTRUSIVE_PTR<MessageEntry> ret=CreateMessageEntry(MessageEntryType_PropertyGetRes,m->MemberName);
			ret->RequestID = m->RequestID;
			ret->ServicePath = m->ServicePath;
					
			if (err)
			{
				RobotRaconteurExceptionUtil::ExceptionToMessageEntry(*err,ret);
			}
			else
			{
				if (!value) throw InternalErrorException("Internal error");
				value->ElementName="value";
				ret->AddElement(value);
			}

			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) > h = boost::bind(&rr_context_emptyhandler,_1);
			skel1->GetContext()->AsyncSendMessage(ret,ep,h);
		}
		catch (std::exception& exp)
		{
			RobotRaconteurNode::TryHandleException(skel1->node, &exp);
		}
	}

	void ServiceSkel::EndAsyncCallSetProperty(RR_WEAK_PTR<ServiceSkel> skel, RR_SHARED_PTR<RobotRaconteurException> err, RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep)
	{
		RR_SHARED_PTR<ServiceSkel> skel1=skel.lock();
		if (!skel1) return;
		
		try
		{
			RR_INTRUSIVE_PTR<MessageEntry> ret=CreateMessageEntry(MessageEntryType_PropertySetRes,m->MemberName);
			ret->RequestID = m->RequestID;
			ret->ServicePath = m->ServicePath;
			
			if (err)
			{
				RobotRaconteurExceptionUtil::ExceptionToMessageEntry(*err,ret);
			}

			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) > h = boost::bind(&rr_context_emptyhandler,_1);
			skel1->GetContext()->AsyncSendMessage(ret,ep,h);
		}
		catch (std::exception& exp)
		{
			RobotRaconteurNode::TryHandleException(skel1->node, &exp);
		}

	}

	void ServiceSkel::EndAsyncCallFunction(RR_WEAK_PTR<ServiceSkel> skel, RR_INTRUSIVE_PTR<MessageElement> ret, RR_SHARED_PTR<RobotRaconteurException> err, RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep)
	{
		RR_SHARED_PTR<ServiceSkel> skel1=skel.lock();
		if (!skel1) return;

		try
		{
			RR_INTRUSIVE_PTR<MessageEntry> ret1=CreateMessageEntry(MessageEntryType_FunctionCallRes,m->MemberName);
			ret1->RequestID = m->RequestID;
			ret1->ServicePath = m->ServicePath;

			if (err)
			{
				RobotRaconteurExceptionUtil::ExceptionToMessageEntry(*err,ret1);
			}
			else
			{
				if (!ret)
				{
					ret1->AddElement("return",ScalarToRRArray<int32_t>(0));
				}
				else
				{
					ret->ElementName="return";
					ret1->AddElement(ret);
				}
			}

			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) > h = boost::bind(&rr_context_emptyhandler,_1);
			skel1->GetContext()->AsyncSendMessage(ret1,ep,h);
		}
		catch (std::exception& exp)
		{
			RobotRaconteurNode::TryHandleException(skel1->node, &exp);
		}
	}


	void ServiceSkel::SendEvent(RR_INTRUSIVE_PTR<MessageEntry> m)
	{
		m->ServicePath = GetServicePath();
		GetContext()->SendEvent(m);
	}

	void ServiceSkel::ReleaseObject()
	{
		{
			boost::mutex::scoped_lock lock(monitorlocks_lock);
			BOOST_FOREACH (RR_SHARED_PTR<ServerContext_MonitorObjectSkel>& e, monitorlocks | boost::adaptors::map_values)
			{
				e->Shutdown();
			}
		}

		UnregisterEvents(uncastobj);
		ReleaseCastObject();
		uncastobj.reset();		
	}

	void ServiceSkel::AsyncSendPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e, bool unreliable,  RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) >) handler)
	{
		m->ServicePath = GetServicePath();	
		
		if (!unreliable)
		{			
			GetContext()->AsyncSendPipeMessage(m, e, unreliable, handler);
		}
		else
		{
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) > h = boost::bind(&rr_context_emptyhandler,_1);
			GetContext()->AsyncSendPipeMessage(m, e, unreliable, h);
		}
	}

	void ServiceSkel::SendWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
	{
		m->ServicePath = GetServicePath();		
		GetContext()->SendWireMessage(m, e);
	}

	void ServiceSkel::DispatchPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
	{
	}

	void ServiceSkel::DispatchWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
	{
	}

	RR_INTRUSIVE_PTR<MessageEntry> ServiceSkel::CallPipeFunction(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
	{
		throw MemberNotFoundException("Pipe " + m->MemberName + " not found");
	}

	RR_INTRUSIVE_PTR<MessageEntry> ServiceSkel::CallWireFunction(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
	{
		throw MemberNotFoundException("Wire " + m->MemberName + " not found");
	}

	RR_SHARED_PTR<void> ServiceSkel::GetCallbackFunction(uint32_t endpoint, const std::string &membername)
	{
		throw MemberNotFoundException("Callback " + membername + " not found");
	}

	RR_INTRUSIVE_PTR<MessageEntry> ServiceSkel::CallMemoryFunction(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e)
	{
		throw MemberNotFoundException("Memory " + m->MemberName + " not found");
	}

	bool ServiceSkel::IsLocked()
	{
		boost::mutex::scoped_lock lock2(objectlock_lock);
		RR_SHARED_PTR<ServerContext_ObjectLock> lock=objectlock.lock();
		if (!lock)
			return false;

		return lock->IsLocked();
	}

	bool ServiceSkel::IsMonitorLocked()
	{
		//boost::mutex::scoped_lock lock2(monitorlocks_lock);
		if (!monitorlock)
			return false;
		return monitorlock->IsLocked();
	}

	void ServiceSkel::InitializeInstanceFields()
	{
		monitorlocks.clear();
	}

	std::string ServiceSkel::GetObjectType(RobotRaconteurVersion client_version)
	{		
		if (!client_version)
		{
			return GetObjectType();
		}

		typedef boost::tuple<RobotRaconteurVersion, std::string> e_type;
		BOOST_FOREACH(e_type e, object_type_ver)
		{
			if (!e.get<0>())
				return e.get<1>();

			if (e.get<0>() <= client_version)
			{
				return e.get<1>();
			}
		}

		throw ObjectNotFoundException("Service requires newer client version");

	}

	void ServiceSkel::CallGeneratorNext(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> ep)
	{
		int32_t index = RRArrayToScalar(m->FindElement("index")->CastData<RRArray<int32_t> >());
		RR_SHARED_PTR<GeneratorServerBase> gen;
		{
			boost::mutex::scoped_lock lock(generators_lock);
			boost::unordered_map<int32_t, RR_SHARED_PTR<GeneratorServerBase> >::iterator e = generators.find(index);
			if (e == generators.end())
			{
				throw InvalidOperationException("Invalid generator");
			}
			gen = e->second;
		}

		if (gen->GetEndpoint() != ep->GetLocalEndpoint())
		{
			throw InvalidOperationException("Invalid generator");
		}
		gen->CallNext(m);
	}

	void ServiceSkel::SendGeneratorResponse(int32_t index, RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep)
	{
		if (m->Error != MessageErrorType_None)
		{
			RR_SHARED_PTR<GeneratorServerBase> gen;
			{
				boost::mutex::scoped_lock lock(generators_lock);
				boost::unordered_map<int32_t, RR_SHARED_PTR<GeneratorServerBase> >::iterator e = generators.find(index);
				if (e == generators.end())
				{
					throw InvalidOperationException("Invalid generator");
				}
				gen = e->second;
			}
		}

		boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) > h = boost::bind(&rr_context_emptyhandler, _1);
		GetContext()->AsyncSendMessage(m,ep,h);
	}

	int32_t ServiceSkel::get_new_generator_index()
	{
		RR_SHARED_PTR<RobotRaconteurNode> node = RRGetNode();
		int32_t index;
		do
		{
			index = node->GetRandomInt<int32_t>(0,std::numeric_limits<int32_t>::max());
		} while (generators.find(index) != generators.end());
		return index;
	}

	RR_SHARED_PTR<ServiceFactory> ServerContext::GetServiceDef() const
	{
		return m_ServiceDef;
	}

	 RR_SHARED_PTR<ServiceFactory> ServerContext::GetRootObjectServiceDef(RobotRaconteurVersion client_version)
	 {
		 std::string root_object_type = GetRootObjectType(client_version);
		 std::string root_object_def;
		 boost::tie(root_object_def, boost::tuples::ignore) = SplitQualifiedName(root_object_type);
		 return GetNode()->GetServiceType(root_object_def);		 
	 }

	 std::string ServerContext::GetServiceName() const
	{
		return m_ServiceName;
	}

	 std::string ServerContext::GetRootObjectType(RobotRaconteurVersion client_version)
	{		 		 
		return GetObjectType(m_ServiceName, client_version);
	}

	ServerContext::ServerContext(RR_SHARED_PTR<ServiceFactory> f, RR_SHARED_PTR<RobotRaconteurNode> node)
	{
		InitializeInstanceFields();
		m_ServiceDef = f;
		this->node=node;
	}

	RR_SHARED_PTR<RobotRaconteurNode> ServerContext::GetNode()
	{
		RR_SHARED_PTR<RobotRaconteurNode> n=node.lock();
		if (!n) throw InvalidOperationException("Node has been released");
		return n;
	}

	void ServerContext::SendEvent(RR_INTRUSIVE_PTR<MessageEntry> m)
	{


		RR_INTRUSIVE_PTR<Message> mm = CreateMessage();

		std::vector<RR_SHARED_PTR<ServerEndpoint> > cc;
		
		{
			{
				boost::mutex::scoped_lock lock(client_endpoints_lock);
				boost::copy(client_endpoints | boost::adaptors::map_values, std::back_inserter(cc));
			}

			BOOST_FOREACH (RR_SHARED_PTR<RobotRaconteur::ServerEndpoint>& c, cc)
			{

				if (m_RequireValidUser)
				{
					try
					{
						if (c->GetAuthenticatedUsername()=="")
							continue;
					}
					catch (AuthenticationException&)
					{
						continue;
					}
				}

				RR_INTRUSIVE_PTR<MessageEntry> m2;
				try
				{
					m2 = ShallowCopyMessageEntry(m);
				}
				catch (std::exception& exp)
				{
					RobotRaconteurNode::TryHandleException(node, &exp);
					continue;
				}

				try
				{
					GetNode()->CheckConnection(c->GetLocalEndpoint());
					boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) > h = boost::bind(&rr_context_emptyhandler,_1);
					AsyncSendMessage(m2,c, h);
				}
				catch (std::exception&)
				{
					try
					{
						RemoveClient(c);
					}
					catch (std::exception&)
					{
					};
				}
			}
		}

	}
#undef SendMessage

	void ServerContext::SendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
	{
		
		RR_SHARED_PTR<ServerEndpoint> s;
		{
			boost::mutex::scoped_lock lock(client_endpoints_lock);
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> >::iterator e1 = client_endpoints.find(e);
			if (e1 == client_endpoints.end()) throw InvalidEndpointException("Invalid client endpoint");
			s=e1->second;
		}
		SendMessage(m,s);
	}

	void ServerContext::SendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e)
	{
		//m.ServicePath = ServiceName;

		RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
		mm->header = CreateMessageHeader();
		//mm.header.ReceiverEndpoint = RemoteEndpoint;
		mm->entries.push_back(m);		

		e->SendMessage(mm);


	}

	void ServerContext::AsyncSendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )>& callback)
	{
		
		RR_SHARED_PTR<ServerEndpoint> s;
		{
			boost::mutex::scoped_lock lock(client_endpoints_lock);
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> >::iterator e1 = client_endpoints.find(e);
			if (e1 == client_endpoints.end()) throw InvalidEndpointException("Invalid client endpoint");
			s = e1->second;

		}
		
		AsyncSendMessage(m,s,(callback));
	}

	void ServerContext::AsyncSendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )>& callback)
	{

		//m.ServicePath = ServiceName;

		RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
		mm->header = CreateMessageHeader();
		//mm.header.ReceiverEndpoint = RemoteEndpoint;
		mm->entries.push_back(m);

		e->AsyncSendMessage(mm,callback);
	}

	void ServerContext::AsyncSendUnreliableMessage(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> e, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
	{
		if (!e->UseMessage3())
		{
			RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
			mm->header = CreateMessageHeader();
			//mm.header.ReceiverEndpoint = RemoteEndpoint;
			mm->entries.push_back(m);
			mm->header->MetaData = "unreliable\n";
			e->AsyncSendMessage(mm, callback);
		}
		else
		{
			RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
			mm->header = CreateMessageHeader();
			//mm.header.ReceiverEndpoint = RemoteEndpoint;
			mm->entries.push_back(m);
			mm->header->MessageFlags |= MessageFlags_UNRELIABLE;
			e->AsyncSendMessage(mm, callback);
		}
	}

	void ServerContext::AsyncSendPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e, bool unreliable, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
	{		
		if (!unreliable)
		{
			AsyncSendMessage(m, e, (callback));
		}
		else
		{
			RR_SHARED_PTR<ServerEndpoint> s;
			{
				boost::mutex::scoped_lock lock(client_endpoints_lock);
				RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> >::iterator e1 = client_endpoints.find(e);
				if (e1 == client_endpoints.end()) throw InvalidEndpointException("Invalid client endpoint");
				s = e1->second;
			}

			AsyncSendUnreliableMessage(m, s, (callback));
		}
	}

	void ServerContext::SendWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
	{		
		RR_SHARED_PTR<ServerEndpoint> s;
		{
			boost::mutex::scoped_lock lock(client_endpoints_lock);
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> >::iterator e1 = client_endpoints.find(e);
			if (e1 == client_endpoints.end()) throw InvalidEndpointException("Invalid client endpoint");
			s = e1->second;
		}
		boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) > h = boost::bind(&rr_context_emptyhandler,_1);
		AsyncSendUnreliableMessage(m, s, h);
	}


	void ServerContext::SetSecurityPolicy(RR_SHARED_PTR<ServiceSecurityPolicy> policy)
	{
		user_authenticator = policy->Authenticator;
		security_policies = policy->Policies;

		if (security_policies.count("requirevaliduser")!=0)
		{
			if (boost::algorithm::to_lower_copy(  security_policies.at("requirevaliduser")) == "true")
				m_RequireValidUser = true;
		}

		if (security_policies.count("allowobjectlock")!=0)
		{
			if (boost::algorithm::to_lower_copy(security_policies.at("allowobjectlock")) == "true")
				AllowObjectLock = true;
		}


	}

	void ServerContext::SetBaseObject(const std::string &name, RR_SHARED_PTR<RRObject> o, RR_SHARED_PTR<ServiceSecurityPolicy> policy)
	{
		if (base_object_set)
			throw InvalidOperationException("Base object already set");

		if (policy != 0)
		{
			SetSecurityPolicy(policy);
		}

		

		m_ServiceName = name;

		m_CurrentServicePath.reset(new std::string(name));
		m_CurrentServerContext.reset(new RR_SHARED_PTR<ServerContext>(shared_from_this()));
		

		RR_SHARED_PTR<ServiceSkel> s = GetServiceDef()->CreateSkel(o->RRType(),name, o, shared_from_this());

		m_RootObjectType = o->RRType(); //boost::algorithm::replace_all_copy(o->RRType(),"::",".");
		base_object_set = true;
		
		{
			boost::mutex::scoped_lock lock (skels_lock);
			skels.insert(std::make_pair(name, s));
		}

		m_CurrentServicePath.reset(0);

		m_CurrentServerContext.reset(0);

	}

	RR_SHARED_PTR<ServiceSkel> ServerContext::GetObjectSkel(const std::string &servicepath)
	{
		
		//object obj = null;
		
		std::vector<std::string> p;
		boost::split(p,servicepath,boost::is_from_range('.','.'));
		
		std::string ppath = p.at(0);

		RR_SHARED_PTR<ServiceSkel> skel;
		{
			boost::mutex::scoped_lock lock (skels_lock);

			RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceSkel> >::iterator e1 = skels.find(servicepath);
			if (e1 != skels.end()) return e1->second;

			skel = skels.at(ppath);
		}
		//obj = skel.uncastobj;

		RR_SHARED_PTR<ServiceSkel> skel1 = skel;

		for (size_t i = 1; i < p.size(); i++)
		{
			{
				boost::mutex::scoped_lock lock (skels_lock);
				std::string ppath1 = ppath;

				ppath=ppath+"." + p.at(i);

				skel1.reset();
				RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceSkel> >::iterator e1 = skels.find(ppath);
				if (e1 != skels.end())
				{
					skel1 = e1->second;
				}

				if (skel1 == 0)
				{

					m_CurrentServicePath.reset( new std::string(ppath1));
					m_CurrentServerContext.reset( new RR_SHARED_PTR<ServerContext>(shared_from_this()));
					RR_SHARED_PTR<RRObject> obj1 = skel->GetSubObj(p.at(i));
										
					m_CurrentServicePath.reset(0);
					
					m_CurrentServerContext.reset(0);

					if (!obj1) throw ServiceException("Requested object is null");

					skel1 = GetServiceDef()->CreateSkel(obj1->RRType(),ppath, obj1, shared_from_this());
					boost::mutex::scoped_lock lock2(skel->objectlock_lock);
					RR_SHARED_PTR<ServerContext_ObjectLock> lock=skel->objectlock.lock();
					if (lock)
					{
						lock->AddSkel(skel1);
					}
					skels.insert(std::make_pair(ppath,skel1));
				}

				skel = skel1;
			}


		}

		return skel;
	}

	void ServerContext::ReplaceObject(const std::string &path)
	{

		ReleaseServicePath(path);


	}

	std::string ServerContext::GetObjectType(const std::string &servicepath, RobotRaconteurVersion client_version)
	{
		//TODO: check client_version
		if (servicepath != GetServiceName())
		{
			if (m_RequireValidUser)
			{
				if (ServerEndpoint::GetCurrentAuthenticatedUser() == 0)
					throw PermissionDeniedException("User must authenticate before accessing this service");
			}
		}

		RR_SHARED_PTR<ServiceSkel> s = GetObjectSkel(servicepath);
				
		return s->GetObjectType(client_version);
		
	}

	RR_SHARED_PTR<ServerContext> ServerContext::GetCurrentServerContext()
	{
		if (!m_CurrentServerContext.get()) throw InvalidOperationException("Not set");
		RR_SHARED_PTR<ServerContext> out= *m_CurrentServerContext.get();
		if (!out) throw InvalidOperationException("Not set");
		return out;
	}
	
	

boost::thread_specific_ptr<RR_SHARED_PTR<ServerContext> > ServerContext::m_CurrentServerContext;

	std::string ServerContext::GetCurrentServicePath()
	{
		if (m_CurrentServicePath.get()==0) throw InvalidOperationException("Not set");
		return std::string(*m_CurrentServicePath);
	}

boost::thread_specific_ptr<std::string> ServerContext::m_CurrentServicePath;

	RR_INTRUSIVE_PTR<MessageEntry> ServerContext::ProcessMessageEntry(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> c)
	{
		//lock (rec_sync)
		//{
		bool noreturn = false;
			RR_INTRUSIVE_PTR<MessageEntry> ret=RR_INTRUSIVE_PTR<MessageEntry>();


			if (m->EntryType == MessageEntryType_ServicePathReleasedRet)
				return RR_INTRUSIVE_PTR<MessageEntry>();

			try
			{
				//ClientSessionOp methods
				if (m->EntryType == MessageEntryType_ClientSessionOpReq)
				{
					return ClientSessionOp(m, c);

				}

				if (m->EntryType == MessageEntryType_ClientKeepAliveReq)
				{
					ret = CreateMessageEntry(MessageEntryType_ClientKeepAliveRet, m->MemberName);
					ret->RequestID = m->RequestID;
					ret->ServicePath = m->ServicePath;
					return ret;
				}

				if (m->EntryType == MessageEntryType_ServiceCheckCapabilityReq)
				{
					ret = CheckServiceCapability(m, c);
				}



				if (m_RequireValidUser)
				{
					if (ServerEndpoint::GetCurrentAuthenticatedUser() == 0)
						throw PermissionDeniedException("User must authenticate before accessing this service");
				}

				
				if (m->EntryType == MessageEntryType_PipePacket || m->EntryType == MessageEntryType_PipePacketRet)
				{
					GetObjectSkel(m->ServicePath)->DispatchPipeMessage(m, c->GetLocalEndpoint());
					ret.reset();
					noreturn = true;
				}

				if (m->EntryType == MessageEntryType_WirePacket)
				{
					GetObjectSkel(m->ServicePath)->DispatchWireMessage(m, c->GetLocalEndpoint());
					ret.reset();
					noreturn = true;
				}





				m_CurrentServicePath.reset(new std::string(m->ServicePath));
				m_CurrentServerContext.reset(new RR_SHARED_PTR<ServerContext>(shared_from_this()));


				if (m->EntryType == MessageEntryType_ObjectTypeName)
				{
					RobotRaconteurVersion v;
					RR_INTRUSIVE_PTR<MessageElement> m_ver;
					if (m->TryFindElement("clientversion", m_ver))
					{
						v.FromString(m_ver->CastDataToString());
					}

					ret = CreateMessageEntry(MessageEntryType_ObjectTypeNameRet, m->MemberName);
					std::string path = static_cast<std::string>(m->ServicePath);
					std::string objtype = GetObjectType(path,v);
					ret->AddElement("objecttype", stringToRRArray(objtype));
				}

				//Object member methods

				if (m->EntryType == MessageEntryType_PropertyGetReq)
				{
					RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(m->ServicePath);
					check_lock(skel);
					ret = skel->CallGetProperty(m);
					if(!ret) noreturn=true;
				}

				if (m->EntryType == MessageEntryType_PropertySetReq)
				{
					RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(m->ServicePath);
					check_lock(skel);
					ret = skel->CallSetProperty(m);
					if (!ret) noreturn=true;
				}

				if (m->EntryType == MessageEntryType_FunctionCallReq)
				{
					RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(m->ServicePath);
					check_lock(skel);
					ret = skel->CallFunction(m);
					if (!ret) noreturn=true;
				}

				if (m->EntryType == MessageEntryType_PipeConnectReq || m->EntryType == MessageEntryType_PipeDisconnectReq)
				{
					RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(m->ServicePath);
					check_lock(skel);
					ret = skel->CallPipeFunction(m,c->GetLocalEndpoint());
				}

				if (m->EntryType == MessageEntryType_WireConnectReq || m->EntryType == MessageEntryType_WireDisconnectReq
					|| m->EntryType == MessageEntryType_WirePeekInValueReq || m->EntryType == MessageEntryType_WirePeekOutValueReq
					|| m->EntryType == MessageEntryType_WirePokeOutValueReq)
				{
					RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(m->ServicePath);
					check_lock(skel);
					ret = skel->CallWireFunction(m, c->GetLocalEndpoint());
				}



				if (m->EntryType == MessageEntryType_MemoryWrite || m->EntryType == MessageEntryType_MemoryRead || m->EntryType == MessageEntryType_MemoryGetParam)
				{
					RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(m->ServicePath);
					check_lock(skel);
					ret = skel->CallMemoryFunction(m, c);
				}

				else if (m->EntryType == MessageEntryType_CallbackCallRet)
				{
					// Console.WriteLine("Got " + m.RequestID + " " + m.EntryType + " " + m.MemberName);
					noreturn=true;
					RR_SHARED_PTR<outstanding_request> t;
					uint32_t transid = m->RequestID;
					{
						
						boost::mutex::scoped_lock lock (outstanding_requests_lock);
						uint32_t transid = m->RequestID;
						RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_request> >::iterator e1 = outstanding_requests.find(m->RequestID);
						if (e1 == outstanding_requests.end())
						{
							noreturn = true;
							return ret;
						}
						t = e1->second;
						t->ret=m;
						try
						{
						if (t->timer) t->timer->Stop();
						}
						catch (std::exception&) {}
						t->evt->Set();
						noreturn=true;
					}

					if (t)
					{
						try
						{
							if (t->handler)
							{
							
								{
								boost::mutex::scoped_lock lock (outstanding_requests_lock);
								outstanding_requests.erase(transid);
								}

								if (m->Error==MessageErrorType_None)
								{
									t->handler(m,RR_SHARED_PTR<RobotRaconteurException>());
								}
								else if (m->Error==MessageErrorType_RemoteError)
								{
									RR_SHARED_PTR<RobotRaconteurException> err=RobotRaconteurExceptionUtil::MessageEntryToException(m);
									RR_SHARED_PTR<RobotRaconteurException> err2=m_ServiceDef->DownCastException(err);
									t->handler(RR_INTRUSIVE_PTR<MessageEntry>(),err2);
								}
								else
								{
									t->handler(RR_INTRUSIVE_PTR<MessageEntry>(),RobotRaconteurExceptionUtil::MessageEntryToException(m));
								}
							}
						}
						catch (std::exception& exp) 
						{
							RobotRaconteurNode::TryHandleException(node, &exp);
						}

					}


				}

				if (m->EntryType == MessageEntryType_GeneratorNextReq)
				{
					RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(m->ServicePath);
					check_lock(skel);
					skel->CallGeneratorNext(m,c);
					noreturn = true;
				}

			}
			catch (std::exception &e)
			{
				if (!noreturn)
				{
				ret = CreateMessageEntry(((MessageEntryType)(m->EntryType+1)), m->MemberName);
				RobotRaconteurExceptionUtil::ExceptionToMessageEntry(e, ret);
				}

			}
			catch (...)
			{
				if (!noreturn)
				{
				ret = CreateMessageEntry(((MessageEntryType)(m->EntryType+1)), m->MemberName);
				ret->Error=MessageErrorType_RemoteError;
				ret->AddElement("errorname", stringToRRArray("std::exception"));
				ret->AddElement("errorstring", stringToRRArray("Unknown exception occured in remote service"));
				}



			}

			m_CurrentServicePath.reset(0);
			
			m_CurrentServerContext.reset(0);

			if (ret == 0 && !noreturn)
			{
				ret = CreateMessageEntry((MessageEntryType)(m->EntryType+1), m->MemberName);
				ret->Error = MessageErrorType_ProtocolError;
				ret->AddElement("errorname", stringToRRArray("RobotRaconteur.ProtocolError"));
				ret->AddElement("errorstring", stringToRRArray("Unknown request type"));

			}

			if (!noreturn)
			{
				ret->ServicePath = m->ServicePath;
				ret->RequestID = m->RequestID;
			}

			return ret;


		//}



	}

	void ServerContext::AsyncProcessCallbackRequest(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t endpoint, RR_MOVE_ARG(boost::function<void ( RR_INTRUSIVE_PTR<MessageEntry>, RR_SHARED_PTR<RobotRaconteurException> )>) handler, int32_t timeout)
	{
						
		uint32_t mytransid;
		
		RR_SHARED_PTR<outstanding_request> t=RR_MAKE_SHARED<outstanding_request>();
		t->handler=handler;
		t->evt=GetNode()->CreateAutoResetEvent();
		
		{
			boost::mutex::scoped_lock lock(outstanding_requests_lock);

			do {
				request_number++;
				if (request_number >= std::numeric_limits<uint32_t>::max()) request_number=0;
				m->RequestID = request_number;
				mytransid = request_number;
			}
			while (outstanding_requests.count(mytransid)!=0);

			if (timeout!=RR_TIMEOUT_INFINITE)
			{
				RR_SHARED_PTR<Timer> timer=GetNode()->CreateTimer(boost::posix_time::milliseconds(timeout),boost::bind(&ServerContext::AsyncProcessCallbackRequest_timeout,shared_from_this(),_1,mytransid),true);
				timer->Start();
				t->timer=timer;

			}

			outstanding_requests.insert(std::make_pair(mytransid,t));

		}
				
		boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&ServerContext::AsyncProcessCallbackRequest_err, shared_from_this(), _1, mytransid);
		AsyncSendMessage(m,endpoint,h);
	}

	void ServerContext::AsyncProcessCallbackRequest_err(RR_SHARED_PTR<RobotRaconteurException> error, uint32_t requestid)
	{
		
		try
		{
			RR_SHARED_PTR<outstanding_request> t;

			{
				boost::mutex::scoped_lock lock(outstanding_requests_lock);
				RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_request> >::iterator e1 = outstanding_requests.find(requestid);
				if (e1 == outstanding_requests.end()) return;
				t = e1->second;
				outstanding_requests.erase(e1);
			}

			detail::InvokeHandlerWithException(node, t->handler, error);
		}
		catch (std::exception& exp) 
		{
			RobotRaconteurNode::TryHandleException(node, &exp);
		}
		

	}

	void ServerContext::AsyncProcessCallbackRequest_timeout(const TimerEvent& error, uint32_t requestid)
	{
		if (!error.stopped)
		{
			try
			{
				RR_SHARED_PTR<outstanding_request> t;

				{
				boost::mutex::scoped_lock lock (outstanding_requests_lock);
				RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_request> >::iterator e1 = outstanding_requests.find(requestid);
				if (e1 == outstanding_requests.end()) return;
				t = e1->second;
				outstanding_requests.erase(e1);
				}

				detail::InvokeHandlerWithException(node, t->handler,RR_MAKE_SHARED<RequestTimeoutException>("Request timed out"));
			}
			catch (std::exception& exp) 
			{
				RobotRaconteurNode::TryHandleException(node, &exp);
			}
		}
	}

	void ServerContext::Close()
	{


		

		try
		{
			RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_ServiceClosed, "");
			SendEvent(e);
		}
		catch (std::exception&)
		{
		};

		{
			boost::mutex::scoped_lock lock (outstanding_requests_lock);
			BOOST_FOREACH (RR_SHARED_PTR<outstanding_request> e, outstanding_requests | boost::adaptors::map_values)
			{
				e->evt->Set();
			}
		}

		{
			boost::mutex::scoped_lock lock (outstanding_requests_lock);
			outstanding_requests.clear();
		}
		
		

		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> > endpoints;
		{
		boost::mutex::scoped_lock lock(client_endpoints_lock);

		endpoints=client_endpoints;
		}

		BOOST_FOREACH (RR_SHARED_PTR<ServerEndpoint>& ee, endpoints | boost::adaptors::map_values)
		{
			try
			{
			RemoveClient(ee);
			}
			catch (std::exception&) {}			
		}		
		
		{
		boost::mutex::scoped_lock lock(client_endpoints_lock);
		client_endpoints.clear();
		}

		{
		boost::mutex::scoped_lock lock(skels_lock);
		BOOST_FOREACH (RR_SHARED_PTR<ServiceSkel>& s, skels | boost::adaptors::map_values)
		{
			try
			{
				s->ReleaseObject();
			}
			catch (std::exception&)
			{
			};
		}
		}

		{
			boost::mutex::scoped_lock lock(monitor_thread_pool_lock);
			try
			{
				if (monitor_thread_pool) monitor_thread_pool->Shutdown();
				monitor_thread_pool.reset();
			}
			catch (std::exception&) {}

		}

		try
		{	
			
			ServerServiceListener(shared_from_this(), ServerServiceListenerEventType_ServiceClosed, RR_SHARED_PTR<RRObject>());
		}
		catch (std::exception& exp)
		{
			RobotRaconteurNode::TryHandleException(node, &exp);
		}
	}

	void ServerContext::MessageReceived(RR_INTRUSIVE_PTR<Message> m, RR_SHARED_PTR<ServerEndpoint> e)
	{

		

		RR_INTRUSIVE_PTR<Message> mret = CreateMessage();
		mret->header = CreateMessageHeader();

		BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& mm, m->entries)
		{
			if (mm->Error == MessageErrorType_InvalidEndpoint)
			{
				this->RemoveClient(e);
				return;
			}

			RR_INTRUSIVE_PTR<MessageEntry> mmret = ProcessMessageEntry(mm,e);
			if (mmret != 0)
			mret->entries.push_back(mmret);
		}
		if (mret->entries.size() > 0)
		{
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&rr_context_node_handler, GetNode(), _1);
			e->AsyncSendMessage(mret, h);
		}
		
	}

	void ServerContext::AddClient(RR_SHARED_PTR<ServerEndpoint> cendpoint)
	{
		
		{
			boost::mutex::scoped_lock lock (client_endpoints_lock);
			client_endpoints.insert(std::make_pair(cendpoint->GetLocalEndpoint(), cendpoint));
		}

		try
		{
			RR_SHARED_PTR<uint32_t> endpt=RR_MAKE_SHARED<uint32_t>(cendpoint->GetLocalEndpoint());
			ServerServiceListener(shared_from_this(), ServerServiceListenerEventType_ClientConnected, endpt);
		}
		catch (std::exception& exp)
		{
			RobotRaconteurNode::TryHandleException(node, &exp);
		}

	}

	void ServerContext::RemoveClient(RR_SHARED_PTR<ServerEndpoint> cendpoint)
	{

		//TODO: possible deadlock

		std::string cusername = cendpoint->GetAuthenticatedUsername();
		uint32_t ce = cendpoint->GetLocalEndpoint();

		{
			boost::mutex::scoped_lock lock (ClientLockOp_lockobj);
			RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServerContext_ObjectLock> > oo=active_object_locks;
			typedef RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServerContext_ObjectLock> >::value_type o_type;
			BOOST_FOREACH (o_type& o, oo)
			{
				try
				{
					if (o.second->GetUsername() == cusername)
					{
						if (o.second->GetEndpoint() == ce)
						{
							o.second->ReleaseLock();
							active_object_locks.erase(o.first);
						}
						else
						{
							
							{
								

								bool lastone=true;

								{
									boost::mutex::scoped_lock lock (client_endpoints_lock);
									BOOST_FOREACH (RR_SHARED_PTR<ServerEndpoint>& cc, client_endpoints | boost::adaptors::map_values)
									{
										if (cc->GetLocalEndpoint() !=cendpoint->GetLocalEndpoint())
											if (cc->GetAuthenticatedUsername()==cusername) lastone=false;
									}
								}

								if (lastone)
								{
									o.second->ReleaseLock();
									active_object_locks.erase(o.first);
								}
							}
						}
					}
				}
				catch (std::exception&)
				{
				}

			}
		}


		try
		
		{
			boost::mutex::scoped_lock lock (client_endpoints_lock);
			client_endpoints.erase(cendpoint->GetLocalEndpoint());
		} catch(std::exception&) {}
		
		try
		{
		GetNode()->DeleteEndpoint(cendpoint);
		}
		catch (std::exception&){}

		try
		{
			
			RR_SHARED_PTR<uint32_t> endpt=RR_MAKE_SHARED<uint32_t>(cendpoint->GetLocalEndpoint());
			ServerServiceListener(shared_from_this(), ServerServiceListenerEventType_ClientDisconnected, endpt);
		}
		catch (std::exception& exp)
		{
			RobotRaconteurNode::TryHandleException(node, &exp);
		}
	}

	void ServerContext::KickUser(const std::string& username)
	{
		std::list<RR_SHARED_PTR<ServerEndpoint> > kicked_clients;
		{
			boost::mutex::scoped_lock lock(client_endpoints_lock);

			BOOST_FOREACH(RR_SHARED_PTR<ServerEndpoint>& e, client_endpoints | boost::adaptors::map_values)
			{
				try
				{
					std::string u=e->GetAuthenticatedUsername();
					if (username==u)
					{
						kicked_clients.push_back(e);
					}
				}
				catch (std::exception&) {}

			}

		}

		BOOST_FOREACH (RR_SHARED_PTR<ServerEndpoint>& ee, kicked_clients)
		{
			try
			{
				RemoveClient(ee);
			}
			catch (std::exception&) {}
		}

	}

	RR_INTRUSIVE_PTR<MessageEntry> ServerContext::ClientSessionOp(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> e)
	{
		if (user_authenticator == 0 && !boost::starts_with(m->MemberName,"Monitor"))
			throw InvalidOperationException("User authentication not activated for this service");

		RR_INTRUSIVE_PTR<MessageEntry> ret = CreateMessageEntry(MessageEntryType_ClientSessionOpRet, m->MemberName);
		ret->RequestID = m->RequestID;
		ret->ServicePath = m->ServicePath;

		std::string command = m->MemberName;

//		switch (command)
//ORIGINAL LINE: case "AuthenticateUser":
		if (command == "AuthenticateUser")
		{
					std::string username = m->FindElement("username")->CastDataToString();
					RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials = rr_cast<RRMap<std::string,RRValue> >((GetNode()->UnpackMapType<std::string, RRValue>(m->FindElement("credentials")->CastData<MessageElementMap<std::string> >())));
					e->AuthenticateUser(username, credentials->GetStorageContainer());
					ret->AddElement("return", stringToRRArray("OK"));
					return ret;
		}
//ORIGINAL LINE: case "LogoutUser":
		else if (command == "LogoutUser")
		{
					e->LogoutUser();
					ret->AddElement("return",stringToRRArray("OK"));
					return ret;

		}
//ORIGINAL LINE: case "RequestObjectLock":
		else if (command == "RequestObjectLock" || command == "ReleaseObjectLock" || command == "RequestClientObjectLock" || command == "ReleaseClientObjectLock" || command == "MonitorEnter" || command == "MonitorContinueEnter" || command == "MonitorExit")
		{

					ClientLockOp(m,ret);
					return ret;

		}
		else
		{
				throw ProtocolException("Invalid ClientSessionOp command");

		}


		throw ProtocolException("Error evaluating ClientSessionOp command");
	}

	bool ServerContext::RequireValidUser()
	{
		return m_RequireValidUser;
	}

	RR_SHARED_PTR<AuthenticatedUser> ServerContext::AuthenticateUser(const std::string &username, std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > &credentials)
	{

		if (!user_authenticator) throw AuthenticationException("Authentication not enabled");

		return user_authenticator->AuthenticateUser(username, credentials,shared_from_this());

	}

	void ServerContext::ClientLockOp(RR_INTRUSIVE_PTR<MessageEntry> m, RR_INTRUSIVE_PTR<MessageEntry> ret)
	{
		
		{
			boost::mutex::scoped_lock lock (ClientLockOp_lockobj);

		   // if (m.ServicePath != ServiceName) throw new Exception("Only locking of root object currently supported");

			std::vector<std::string> priv;
			std::string username = "";
			if (!boost::starts_with(m->MemberName,"Monitor"))
			{
				if (ServerEndpoint::GetCurrentAuthenticatedUser() == 0)
					throw PermissionDeniedException("User must be authenticated to lock object");
				priv = ServerEndpoint::GetCurrentAuthenticatedUser()->GetPrivileges();
				if (!(std::find(priv.begin(),priv.end(),("objectlock"))!=priv.end() || std::find(priv.begin(),priv.end(),"objectlockoverride")!=priv.end()))
					throw ObjectLockedException("User does not have object locking privileges");
				username = ServerEndpoint::GetCurrentAuthenticatedUser()->GetUsername();
			}



			std::string servicepath = m->ServicePath;

			RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(servicepath);

			if (m->MemberName == "RequestObjectLock")
			{
				boost::mutex::scoped_lock lock(skels_lock);
						if (skel->IsLocked())
							throw ObjectLockedException("Object already locked");
						 for (RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceSkel> >::iterator s=skels.begin(); s!=skels.end(); ++s)
						 {
							if (boost::starts_with(s->first,servicepath))
								if (s->second->IsLocked())
									throw ObjectLockedException("Object already locked");
						 }

						RR_SHARED_PTR<ServerContext_ObjectLock> o = RR_MAKE_SHARED<ServerContext_ObjectLock>(ServerEndpoint::GetCurrentAuthenticatedUser()->GetUsername(), skel);
												
						for (RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceSkel> >::iterator s=skels.begin(); s!=skels.end(); ++s)
						 {
							if (boost::starts_with(s->first,servicepath))
								o->AddSkel(s->second);
						 }

						active_object_locks.insert(make_pair(o->GetRootServicePath(), o));
						ret->AddElement("return", stringToRRArray("OK"));
			}
			else if (m->MemberName == "RequestClientObjectLock")
			{
				boost::mutex::scoped_lock lock(skels_lock);
						if (skel->IsLocked())
							throw ObjectLockedException("Object already locked");
						for (RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceSkel> >::iterator s=skels.begin(); s!=skels.end(); ++s)
						 {
							if (boost::starts_with(s->first,servicepath))
								if (s->second->IsLocked())
									throw ObjectLockedException("Object already locked");
						 }

						RR_SHARED_PTR<ServerContext_ObjectLock> o = RR_MAKE_SHARED<ServerContext_ObjectLock>(ServerEndpoint::GetCurrentAuthenticatedUser()->GetUsername(), skel,ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint());
						for (RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceSkel> >::iterator s=skels.begin(); s!=skels.end(); ++s)
						 {
							if (boost::starts_with(s->first,servicepath))
								o->AddSkel(s->second);
						 }
						active_object_locks.insert(make_pair(o->GetRootServicePath(), o));
						ret->AddElement("return", stringToRRArray("OK"));


			}
			else if (m->MemberName == "ReleaseObjectLock")
			{
					if (!skel->IsLocked())
						return;
					boost::mutex::scoped_lock lock2(skel->objectlock_lock);
					RR_SHARED_PTR<ServerContext_ObjectLock> lock=skel->objectlock.lock();
					if (!lock) return;
					if (lock->GetRootServicePath() != servicepath)
						throw ObjectLockedException("Cannot release inherited lock");
					if (username != lock->GetUsername() && std::find(priv.begin(),priv.end(),"objectlockoverride")==priv.end())
						throw ObjectLockedException("Service locked by user " + lock->GetUsername());
					if (lock->GetEndpoint() != 0)
					{
						if (ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint() != lock->GetEndpoint() && std::find(priv.begin(),priv.end(),"objectlockoverride")==priv.end())
							if (username != lock->GetUsername() && std::find(priv.begin(),priv.end(),"objectlockoverride")==priv.end())
								throw ObjectLockedException("Service locked by other session");
					}

					lock2.unlock();
					try
					{
					lock->ReleaseLock();
					}
					catch (std::exception&) {}
					lock2.lock();
					if (active_object_locks.count(skel->GetServicePath())!=0)
						active_object_locks.erase(skel->GetServicePath());
					


					ret->AddElement("return", stringToRRArray("OK"));
			}
			else if (m->MemberName == "MonitorEnter")
			{
				{
				boost::mutex::scoped_lock lock2(skel->monitorlocks_lock);
						if (skel->monitorlocks.find(ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint()) != skel->monitorlocks.end())
							throw InvalidOperationException("Already acquiring or acquired monitor lock");

				}
						RR_SHARED_PTR<ServerContext_MonitorObjectSkel> s = RR_MAKE_SHARED<ServerContext_MonitorObjectSkel>(skel);
						int32_t timeout = RRArrayToScalar(m->FindElement("timeout")->CastData<RRArray<int32_t> >());

						lock.unlock();
						std::string retcode = s->MonitorEnter(ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint(), timeout);
						ret->AddElement("return", stringToRRArray(retcode));

			}
			else if (m->MemberName == "MonitorContinueEnter")
			{
				RR_SHARED_PTR<ServerContext_MonitorObjectSkel> s;
				{
					boost::mutex::scoped_lock lock2(skel->monitorlocks_lock);
					RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerContext_MonitorObjectSkel> >::iterator e1 = skel->monitorlocks.find(ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint());
					if (e1 == skel->monitorlocks.end())
						throw InvalidOperationException("Not acquiring monitor lock");

					s = e1->second;
				}
				lock.unlock();
				std::string retcode = s->MonitorContinueEnter(ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint());
				ret->AddElement("return", stringToRRArray(retcode));

			}
			else if (m->MemberName == "MonitorExit")
			{

				RR_SHARED_PTR<ServerContext_MonitorObjectSkel> s;

				{
					boost::mutex::scoped_lock lock2(skel->monitorlocks_lock);
					if (!skel->monitorlock) return;
					RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerContext_MonitorObjectSkel> >::iterator e1 = skel->monitorlocks.find(ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint());
					if (e1 == skel->monitorlocks.end())
						throw InvalidOperationException("Not monitor locked");
					s = e1->second;
				}
					std::string retcode = s->MonitorExit(ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint());
					ret->AddElement("return", stringToRRArray(retcode));
			}
			else
			{
					throw InvalidOperationException("Invalid command");
			}
		}



	}

	void ServerContext::check_lock(RR_SHARED_PTR<ServiceSkel> skel)
	{
		check_monitor_lock(skel);
		if (skel->IsLocked())
		{
			boost::mutex::scoped_lock lock2(skel->objectlock_lock);
			RR_SHARED_PTR<ServerContext_ObjectLock> lock=skel->objectlock.lock();
			if (!lock) return;
			if (lock->GetUsername() == ServerEndpoint::GetCurrentAuthenticatedUser()->GetUsername() && lock->GetEndpoint() == 0)
				return;
			if (lock->GetUsername() == ServerEndpoint::GetCurrentAuthenticatedUser()->GetUsername() && lock->GetEndpoint() == ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint())
				return;
			throw ObjectLockedException("Object locked by " + lock->GetUsername());

		}
	}

	void ServerContext::check_monitor_lock(RR_SHARED_PTR<ServiceSkel> skel)
	{
		boost::mutex::scoped_lock lock2(skel->monitorlocks_lock);
		if (skel->IsMonitorLocked())
		{
			if (skel->monitorlock->GetLocalEndpoint() == ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint())
			{
				skel->monitorlock->MonitorRefresh(ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint());
			}
			else
			{
				throw InvalidOperationException("Object is currently monitor locked. Use MonitorEnter to obtain monitor lock");
			}

		}

	}

	void ServerContext::PeriodicCleanupTask()
	{

	}

	RR_INTRUSIVE_PTR<MessageEntry> ServerContext::CheckServiceCapability(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> c)
	{
		RR_INTRUSIVE_PTR<MessageEntry> ret = CreateMessageEntry(MessageEntryType_ServiceCheckCapabilityRet, m->MemberName);
		ret->ServicePath = m->ServicePath;
		ret->RequestID = m->RequestID;
		ret->AddElement("return", ScalarToRRArray(static_cast<uint32_t>(0)));
		return ret;

	}

	void ServerContext::ReleaseServicePath1(const std::string &path)
	{
		if (path == GetServiceName())
			throw ServiceException("Root object cannot be released");

		
		{
			boost::mutex::scoped_lock lock (skels_lock);
			std::vector<std::string> objkeys;
			BOOST_FOREACH (const std::string& k, skels | boost::adaptors::map_keys)
			{
				if (k == path || boost::starts_with(k,path + "."))
				{
					objkeys.push_back(k);
				}
			}

	
			if (objkeys.size() == 0)
				throw ServiceException("Unknown service path");

			BOOST_FOREACH (std::string& path1, objkeys)
			{
				RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceSkel> >::iterator e1 = skels.find(path1);
				if (e1 == skels.end()) continue;
				RR_SHARED_PTR<ServiceSkel> s = e1->second;

				if (s->IsLocked())
				{
					
					{
						boost::mutex::scoped_lock lock2(s->objectlock_lock);
						RR_SHARED_PTR<ServerContext_ObjectLock> lock=s->objectlock.lock();
						if (!lock) return;
						boost::mutex::scoped_lock lock3 (ClientLockOp_lockobj);
						if (lock->GetRootServicePath() == path1)
						{
							active_object_locks.erase(lock->GetUsername());
							lock->ReleaseLock();

						}
						else
						{
							lock->ReleaseSkel(s);
						}
					}
				}


				//s->ReleaseCastObject();
				skels.erase(path1);
				s->ReleaseObject();
			}
		}
		
	}

	void ServerContext::ReleaseServicePath(const std::string &path)
	{

		ReleaseServicePath1(path);

		RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ServicePathReleasedReq, "");
		m->ServicePath = path;

		SendEvent(m);
	}

	void ServerContext::ReleaseServicePath(const std::string &path, const std::vector<uint32_t>& endpoints)
	{

		ReleaseServicePath1(path);

		RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ServicePathReleasedReq, "");
		m->ServicePath = path;

		std::vector<RR_SHARED_PTR<ServerEndpoint> > cc;

		{
			{
				boost::mutex::scoped_lock lock(client_endpoints_lock);
				BOOST_FOREACH (uint32_t e, endpoints)
				{
					RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> >::iterator e1 = client_endpoints.find(e);
					if (e1 != client_endpoints.end())
					{
						cc.push_back(e1->second);
					}
				}
			}

			BOOST_FOREACH (RR_SHARED_PTR<RobotRaconteur::ServerEndpoint>& c, cc)
			{

				if (m_RequireValidUser)
				{
					try
					{
						if (c->GetAuthenticatedUsername()=="")
							continue;
					}
					catch (AuthenticationException&)
					{
						continue;
					}
				}

				try
				{
					GetNode()->CheckConnection(c->GetLocalEndpoint());
					boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) > h = boost::bind(&rr_context_emptyhandler,_1);
					AsyncSendMessage(ShallowCopyMessageEntry(m), c, h);
				}
				catch (std::exception&)
				{
					try
					{
						RemoveClient(c);
					}
					catch (std::exception&)
					{
					};
				}
			}
		}
	}


	RR_INTRUSIVE_PTR<MessageEntry> ServerContext::ProcessCallbackRequest(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t endpointid)
	{
		RR_SHARED_PTR<ServerEndpoint> e;
		{
			boost::mutex::scoped_lock lock(client_endpoints_lock);
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> >::iterator e1 = client_endpoints.find(endpointid);
			if (e1 == client_endpoints.end()) throw InvalidEndpointException("Invalid client endpoint");
			e = e1->second;
		}

		uint32_t mytransid;
		RR_SHARED_PTR<outstanding_request> t = RR_MAKE_SHARED<outstanding_request>();
		t->evt = GetNode()->CreateAutoResetEvent();
		{
			boost::mutex::scoped_lock lock(outstanding_requests_lock);

			do {
				request_number++;
				if (request_number >= std::numeric_limits<uint32_t>::max()) request_number = 0;
				m->RequestID = request_number;
				mytransid = request_number;
			} while (outstanding_requests.count(mytransid) != 0);

			outstanding_requests.insert(std::make_pair(mytransid, t));

		}




		//Console.WriteLine("Sent " + m.RequestID + " " + m.EntryType + " " + m.MemberName);
		SendMessage(m, e);


		boost::posix_time::ptime request_start = GetNode()->NowUTC();
		uint32_t request_timeout = GetNode()->GetRequestTimeout();
		while (true)
		{

			{
				boost::mutex::scoped_lock lock(outstanding_requests_lock);
				if (t->ret)
					break;
			}
			GetNode()->CheckConnection(e->GetLocalEndpoint());



			t->evt->WaitOne(10);
			if ((GetNode()->NowUTC() - request_start).total_milliseconds() > request_timeout)
			{

				{
					boost::mutex::scoped_lock lock(outstanding_requests_lock);
					outstanding_requests.erase(mytransid);
				}
				throw RequestTimeoutException("Request timeout");

			}
		}


		RR_INTRUSIVE_PTR<MessageEntry> rec_message;
		{
			boost::mutex::scoped_lock lock(outstanding_requests_lock);
			outstanding_requests.erase(mytransid);
			rec_message = t->ret;
		}

		if (rec_message->RequestID != mytransid)
			throw InternalErrorException("This should be impossible!");

		if (rec_message->Error != MessageErrorType_None)
		{
			if (rec_message->Error == MessageErrorType_RemoteError)
			{
				RR_SHARED_PTR<RobotRaconteurException> err = RobotRaconteurExceptionUtil::MessageEntryToException(rec_message);
				if (!err) RobotRaconteurExceptionUtil::ThrowMessageEntryException(rec_message);
				m_ServiceDef->DownCastAndThrowException(*err);
			}

			RobotRaconteurExceptionUtil::ThrowMessageEntryException(rec_message);
		}

		return rec_message;
	}

	RR_SHARED_PTR<ThreadPool> ServerContext::GetMonitorThreadPool()
	{
		boost::mutex::scoped_lock lock(monitor_thread_pool_lock);

		if (!monitor_thread_pool)
		{
			monitor_thread_pool=GetNode()->GetThreadPoolFactory()->NewThreadPool(GetNode());
			monitor_thread_pool->SetThreadPoolCount(5);
		
		}
		
		return monitor_thread_pool;
	}

	int32_t ServerContext::GetMonitorThreadPoolCount()
	{
		return (int32_t)GetMonitorThreadPool()->GetThreadPoolCount();
	}

	void ServerContext::SetMonitorThreadPoolCount(int32_t count)
	{
		GetMonitorThreadPool()->SetThreadPoolCount(count);
	}

	bool ServerContext::UseMessage3(uint32_t ep)
	{
		boost::mutex::scoped_lock lock(client_endpoints_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> >::iterator e = client_endpoints.find(ep);
		if (e == client_endpoints.end()) return false;
		return e->second->UseMessage3();
	}

	void ServerContext::InitializeInstanceFields()
	{
		
		m_RootObjectType = "";
		
		base_object_set = false;
		m_RequireValidUser = false;
		AllowObjectLock = false;
		
		request_number = 0;
	}

	std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > ServerContext::GetAttributes()
	{
		boost::mutex::scoped_lock lock(m_Attributes_lock);
		return m_Attributes;
	}
	void ServerContext::SetAttributes(const std::map<std::string, RR_INTRUSIVE_PTR<RRValue> >& attr)
	{
		boost::mutex::scoped_lock lock(m_Attributes_lock);

		BOOST_FOREACH (const RR_INTRUSIVE_PTR<RRValue>& e, attr | boost::adaptors::map_values)
		{
			RR_INTRUSIVE_PTR<RRBaseArray> a=RR_DYNAMIC_POINTER_CAST<RRBaseArray>(e);
			if (!a) throw InvalidArgumentException("Attributes but be numbers or strings");
		}

		m_Attributes=attr;

		try
		{
			GetNode()->UpdateServiceStateNonce();
		}
		catch (std::exception&) {}
	}


boost::thread_specific_ptr<RR_SHARED_PTR<ServerEndpoint> > ServerEndpoint::m_CurrentEndpoint;

	RR_SHARED_PTR<ServerEndpoint> ServerEndpoint::GetCurrentEndpoint()
	{
		
		if (!m_CurrentEndpoint.get()) throw InvalidOperationException("Not set");
		RR_SHARED_PTR<ServerEndpoint> out=*m_CurrentEndpoint.get();
		if (!out) throw InvalidOperationException("Not set");
		return out;
	}

boost::thread_specific_ptr<RR_SHARED_PTR<AuthenticatedUser> > ServerEndpoint::m_CurrentAuthenticatedUser;

	RR_SHARED_PTR<AuthenticatedUser> ServerEndpoint::GetCurrentAuthenticatedUser()
	{
		if (!m_CurrentAuthenticatedUser.get()) throw PermissionDeniedException("User is not authenticated");
		RR_SHARED_PTR<AuthenticatedUser> out=*m_CurrentAuthenticatedUser.get();
		if (!out) throw AuthenticationException("User is not authenticated");
		return out;
	}

	const std::string ServerEndpoint::GetAuthenticatedUsername() const
	{
		if (endpoint_authenticated_user == 0)
			return "";

		return endpoint_authenticated_user->GetUsername();
	}

	void ServerEndpoint::MessageReceived(RR_INTRUSIVE_PTR<Message> m)
	{
		if (m->entries.size() > 0)
		{
			if (m->entries.at(0)->EntryType == MessageEntryType_EndpointCheckCapability)
			{
				CheckEndpointCapabilityMessage(m);
				return;
			}
		}
		{
			
			
			SetLastMessageReceivedTime(GetNode()->NowUTC());
		}
		m_CurrentEndpoint.reset(new RR_SHARED_PTR<ServerEndpoint>(shared_from_this()));
		m_CurrentAuthenticatedUser.reset(new RR_SHARED_PTR<AuthenticatedUser>(endpoint_authenticated_user));
		if (endpoint_authenticated_user != 0)
			endpoint_authenticated_user->UpdateLastAccess();
		service->MessageReceived(m, shared_from_this());
		m_CurrentEndpoint.reset(0);
		m_CurrentAuthenticatedUser.reset(0);
	}

	void ServerEndpoint::AuthenticateUser(const std::string &username, std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > &credentials)
	{
		
		RR_SHARED_PTR<AuthenticatedUser> u = service->AuthenticateUser(username, credentials);
		endpoint_authenticated_user = u;
		m_CurrentAuthenticatedUser.reset(new RR_SHARED_PTR<AuthenticatedUser>( u));
	}

	void ServerEndpoint::LogoutUser()
	{
		endpoint_authenticated_user.reset();
		m_CurrentAuthenticatedUser.reset(0);
	}

	void ServerEndpoint::PeriodicCleanupTask()
	{
		if ((GetNode()->NowUTC() - GetLastMessageReceivedTime()).total_milliseconds() > GetNode()->GetEndpointInactivityTimeout())
		{
			service->RemoveClient(shared_from_this());
		}
	}

	bool ServerEndpoint::UseMessage3()
	{
		bool f = false;
		use_message3.compare_exchange_strong(f, false);
		return f;
	}
	void ServerEndpoint::SetTransportConnection(RR_SHARED_PTR<ITransportConnection> c)
	{
		Endpoint::SetTransportConnection(c);
		bool m = c->CheckCapabilityActive(TransportCapabilityCode_MESSAGE3_BASIC_PAGE | TransportCapabilityCode_MESSAGE3_BASIC_ENABLE);
		use_message3.store(m);
	}

	void ServerEndpoint::InitializeInstanceFields()
	{
		endpoint_authenticated_user.reset();
	}

	
}
