// Copyright 2011-2019 Wason Technology, LLC
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

#include "RobotRaconteur/Client.h"
#include "RobotRaconteur/ServiceDefinition.h"
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/Error.h"
#include <boost/algorithm/string.hpp>
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/AutoResetEvent.h"
#include "RobotRaconteur/DataTypes.h"

#include "RobotRaconteur/ErrorUtil.h"

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>

#undef SendMessage

namespace RobotRaconteur
{
	static void rr_context_emptyhandler(RR_SHARED_PTR<RobotRaconteurException>)
	{

	}

	static void rr_context_node_handler(RR_SHARED_PTR<RobotRaconteurNode> n, RR_SHARED_PTR<RobotRaconteurException> e)
	{
		n->HandleException(e.get());
	}

	ServiceStub::ServiceStub(const std::string &path, RR_SHARED_PTR<ClientContext> c)
	{
		context = c;
		ServicePath = path;
		this->node = c->GetNode();
		RRMutex = RR_MAKE_SHARED<boost::recursive_mutex>();
	}

	RR_SHARED_PTR<ClientContext> ServiceStub::GetContext()
	{
		RR_SHARED_PTR<ClientContext> out = context.lock();
		if (!out) throw InvalidOperationException("Stub has been closed");
		return out;
	}

	RR_INTRUSIVE_PTR<MessageEntry> ServiceStub::ProcessRequest(RR_INTRUSIVE_PTR<MessageEntry> m)
	{


		m->ServicePath = ServicePath;
		return GetContext()->ProcessRequest(m);
	}


	void ServiceStub::AsyncProcessRequest(RR_INTRUSIVE_PTR<MessageEntry> m, RR_MOVE_ARG(boost::function<void(RR_INTRUSIVE_PTR<MessageEntry>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		m->ServicePath = ServicePath;
		GetContext()->AsyncProcessRequest(m, RR_MOVE(handler), timeout);
	}

	RR_SHARED_PTR<RRObject> ServiceStub::FindObjRef(const std::string &n)
	{

		return GetContext()->FindObjRef(ServicePath + "." + n);
	}

	void ServiceStub::AsyncFindObjRef(const std::string &n, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		GetContext()->AsyncFindObjRef(ServicePath + "." + n, "", RR_MOVE(handler), timeout);
	}

	RR_SHARED_PTR<RRObject> ServiceStub::FindObjRef(const std::string &n, const std::string &i)
	{

		return GetContext()->FindObjRef(ServicePath + "." + n + "[" + detail::encode_index(i) + "]");
	}

	RR_SHARED_PTR<RRObject> ServiceStub::FindObjRefTyped(const std::string &n, const std::string& objecttype)
	{

		return GetContext()->FindObjRef(ServicePath + "." + n, objecttype);
	}

	RR_SHARED_PTR<RRObject> ServiceStub::FindObjRefTyped(const std::string &n, const std::string &i, const std::string& objecttype)
	{

		return GetContext()->FindObjRef(ServicePath + "." + n + "[" + detail::encode_index(i) + "]", objecttype);
	}

	void ServiceStub::AsyncFindObjRef(const std::string &n, const std::string &i, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		GetContext()->AsyncFindObjRef(ServicePath + "." + n + "[" + detail::encode_index(i) + "]", "", RR_MOVE(handler), timeout);
	}

	void ServiceStub::AsyncFindObjRefTyped(const std::string &n, const std::string& objecttype, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		GetContext()->AsyncFindObjRef(ServicePath + "." + n, objecttype, RR_MOVE(handler), timeout);
	}

	void ServiceStub::AsyncFindObjRefTyped(const std::string &n, const std::string &i, const std::string& objecttype, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		GetContext()->AsyncFindObjRef(ServicePath + "." + n + "[" + detail::encode_index(i) + "]", objecttype, RR_MOVE(handler), timeout);
	}

	std::string ServiceStub::FindObjectType(const std::string &n)
	{
		return GetContext()->FindObjectType(ServicePath + "." + n);
	}

	std::string ServiceStub::FindObjectType(const std::string &n, const std::string &i)
	{
		return GetContext()->FindObjectType(ServicePath + "." + n + "[" + detail::encode_index(i) + "]");
	}

	void ServiceStub::AsyncFindObjectType(const std::string &n, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		GetContext()->AsyncFindObjectType(ServicePath + "." + n, RR_MOVE(handler), timeout);
	}

	void ServiceStub::AsyncFindObjectType(const std::string &n, const std::string &i, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		GetContext()->AsyncFindObjectType(ServicePath + "." + n + "[" + detail::encode_index(i) + "]", RR_MOVE(handler), timeout);
	}



	void ServiceStub::AsyncSendPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, bool unreliable, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) >& handler)
	{

		m->ServicePath = ServicePath;
		GetContext()->AsyncSendPipeMessage(m, unreliable, (handler));
	}

	void ServiceStub::SendWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m)
	{

		m->ServicePath = ServicePath;
		GetContext()->SendWireMessage(m);
	}

	void ServiceStub::DispatchPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m)
	{
	}

	void ServiceStub::DispatchWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m)
	{
	}

	RR_INTRUSIVE_PTR<MessageEntry> ServiceStub::CallbackCall(RR_INTRUSIVE_PTR<MessageEntry> m)
	{
		throw MemberNotFoundException("Member not found");
	}

	void ServiceStub::RRClose()
	{

	}

	RR_SHARED_PTR<PipeClientBase> ServiceStub::RRGetPipeClient(const std::string& membername)
	{
		throw MemberNotFoundException("PipeClient \"" + membername + "\" not found");
	}

	RR_SHARED_PTR<WireClientBase> ServiceStub::RRGetWireClient(const std::string& membername)
	{
		throw MemberNotFoundException("WireClient \"" + membername + "\" not found");
	}



	RR_SHARED_PTR<RobotRaconteurNode> ServiceStub::RRGetNode()
	{
		RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
		if (!n) throw InvalidOperationException("Node has been released");
		return n;
	}

	RR_WEAK_PTR<RobotRaconteurNode> ServiceStub::RRGetNodeWeak()
	{
		return node;
	}

	RR_SHARED_PTR<ServiceFactory> ClientContext::GetServiceDef() const
	{
		return m_ServiceDef;
	}

	ClientContext::ClientContext(RR_SHARED_PTR<RobotRaconteurNode> node) : Endpoint(node)
	{


		//rec_event = new AutoResetEvent(false);
		InitializeInstanceFields();

	}

	ClientContext::ClientContext(RR_SHARED_PTR<ServiceFactory> service_def, RR_SHARED_PTR<RobotRaconteurNode> node) : Endpoint(node)
	{
		InitializeInstanceFields();
		m_ServiceDef = service_def;


		//rec_event = new AutoResetEvent(false);

	}

	RR_SHARED_PTR<RRObject> ClientContext::FindObjRef(const std::string &path, const std::string& objecttype2)
	{
		RR_SHARED_PTR<detail::sync_async_handler<RRObject> >h = RR_MAKE_SHARED<detail::sync_async_handler<RRObject> >();
		AsyncFindObjRef(path, objecttype2, boost::bind(&detail::sync_async_handler<RRObject>::operator(), h, _1, _2), GetNode()->GetRequestTimeout());
		return h->end();
	}

	void ClientContext::AsyncFindObjRef(const std::string &path, const std::string& objecttype2, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{

		if (!GetConnected()) throw ConnectionException("Service client not connected");

		boost::mutex::scoped_lock lock2(FindObjRef_lock);
		boost::mutex::scoped_lock lock(stubs_lock);
		RR_SHARED_PTR<ServiceStub> stub;
		{
			RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceStub> >::iterator e1 = stubs.find(path);
			if (e1 != stubs.end())
			{
				stub = e1->second;
			}
			else
			{
				if (boost::range::find(active_stub_searches, path) != active_stub_searches.end())
				{
					active_stub_searches_handlers.push_back(boost::make_tuple(path, handler));
					return;
				}
			}

		}

		if (stub)
		{
			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, rr_cast<RRObject>(stub), RR_SHARED_PTR<RobotRaconteurException>()), true);
			return;
		}
		else
		{
			RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_ObjectTypeName, "");
			e->AddElement("clientversion", stringToRRArray(ROBOTRACONTEUR_VERSION_TEXT));
			//MessageElement m = e.AddElement("ObjectPath", path);
			e->ServicePath = path;
			RR_SHARED_PTR<detail::async_timeout_wrapper<RRObject> > t = RR_MAKE_SHARED<detail::async_timeout_wrapper<RRObject> >(GetNode(), boost::bind(&ClientContext::AsyncFindObjRef3, shared_from_this(), _1, _2, path, handler));

			boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> h1(boost::bind(&detail::async_timeout_wrapper<RRObject>::operator(), t, _1, _2));
			AsyncProcessRequest(e, boost::bind(&ClientContext::AsyncFindObjRef1, shared_from_this(), _1, _2, path, objecttype2, h1, timeout), timeout);
			t->start_timer(timeout, RR_MAKE_SHARED<RequestTimeoutException>("Request timeout"));
			active_stub_searches.push_back(path);

		}
	}

	void ClientContext::AsyncFindObjRef1(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, const std::string& path, const std::string& objecttype2, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler, int32_t timeout)
	{
		if (err)
		{
			detail::InvokeHandlerWithException(node, handler, err);
			return;
		}
		else
		{
			try
			{
				std::string objecttype = ret->FindElement("objecttype")->CastDataToString();

				if (objecttype == "")
					throw ObjectNotFoundException("Object type was not returned.");


				std::string objectdef = SplitQualifiedName(objecttype).get<0>();

				if (use_pulled_types)
				{

					std::vector<std::string> servicetypes = GetPulledServiceTypes();
					if (std::find(servicetypes.begin(), servicetypes.end(), objectdef) == servicetypes.end())
					{
						AsyncPullServiceDefinitionAndImports(objectdef, boost::bind(&ClientContext::AsyncFindObjRef2, shared_from_this(), _1, _2, objecttype, objectdef, path, objecttype2, handler, timeout), timeout);
						return;
					}

				}

				AsyncFindObjRef2(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > >(), RR_SHARED_PTR<RobotRaconteurException>(), objecttype, objectdef, path, objecttype2, (handler), timeout);
			}			
			catch (std::exception& err)
			{
				detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
			}
		}
	}

	void ClientContext::AsyncFindObjRef2(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > ret, RR_SHARED_PTR<RobotRaconteurException> err, const std::string& objecttype1, const std::string& objectdef, const std::string& path, const std::string& objecttype2, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler, int32_t timeout)
	{
		std::string objecttype = objecttype1;
		if (err)
		{
			detail::InvokeHandlerWithException(node, handler, err);
			return;
		}
		else
		{

			try
			{

				RR_SHARED_PTR<ServiceStub> stub;

				if (use_pulled_types)
				{
					if (ret)
					{
						std::vector<std::string> servicetypes = GetPulledServiceTypes();

						if (std::find(servicetypes.begin(), servicetypes.end(), objectdef) == servicetypes.end())
						{
							std::vector<RR_SHARED_PTR<ServiceDefinition> > d = *ret;

							std::vector<RR_SHARED_PTR<ServiceDefinition> > missingdefs = std::vector<RR_SHARED_PTR<ServiceDefinition> >();



							BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition>& di, d)
							{

								std::vector<std::string> servicetypes2 = GetPulledServiceTypes();
								if (std::find(servicetypes2.begin(), servicetypes2.end(), (di->Name)) == servicetypes2.end())
								{
									missingdefs.push_back(di);
								}

							}
							if (missingdefs.size() > 0)
							{

								std::vector<std::string> missingdefs_str;
								BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition>ee, missingdefs)
								{
									missingdefs_str.push_back(ee->ToString());
								}

								std::vector<RR_SHARED_PTR<ServiceFactory> > di2 = GetNode()->GetDynamicServiceFactory()->CreateServiceFactories(missingdefs_str);

								for (size_t i = 0; i < missingdefs.size(); i++)
								{
									boost::mutex::scoped_lock lock(pulled_service_types_lock);
									pulled_service_types.insert(std::make_pair(di2.at(i)->GetServiceName(), di2.at(i)));
								}
							}

						}
					}

					if (objecttype2 != "")
					{
						VerifyObjectImplements(objecttype, objecttype2);
						objecttype = objecttype2;
					}

					RR_SHARED_PTR<ServiceFactory> f = GetPulledServiceType(objectdef);

					stub = f->CreateStub(objecttype, path, shared_from_this());
				}

				else
				{
					if (objecttype2 != "")
					{
						VerifyObjectImplements(objecttype, objecttype2);
						objecttype = objecttype2;
					}

					stub = GetServiceDef()->CreateStub(objecttype, path, shared_from_this());
				}


				{
					boost::mutex::scoped_lock lock(stubs_lock);
					RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceStub> >::iterator e1 = stubs.find(path);
					if (e1 == stubs.end())
					{
						stubs.insert(std::make_pair(path, stub));
					}
					else
					{
						stub = e1->second;
					}
				}

				RR_SHARED_PTR<RRObject> ret = RR_STATIC_POINTER_CAST<RRObject>(stub);
				detail::InvokeHandler(node, handler, ret);
				
			}			
			catch (std::exception& err2)
			{
				detail::InvokeHandlerWithException(node, handler, err2, MessageErrorType_ServiceError);
			}
		}


	}

	void ClientContext::AsyncFindObjRef3(RR_SHARED_PTR<RRObject> ret, RR_SHARED_PTR<RobotRaconteurException> err, const std::string& path, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{

		try
		{
			boost::mutex::scoped_lock lock(stubs_lock);
			active_stub_searches.remove(path);
			for (std::list<boost::tuple<std::string, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> > >::iterator ee = active_stub_searches_handlers.begin(); ee != active_stub_searches_handlers.end();)
			{
				if (ee->get<0>() == path)
				{
					try
					{
						RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(ee->get<1>(), ret, err), true);
					}
					catch (std::exception&) {}

					ee = active_stub_searches_handlers.erase(ee);
				}
				else
				{
					ee++;
				}
			}
		}
		catch (std::exception&) {}

		try
		{
			handler(ret, err);
		}
		catch (std::exception& exp)
		{
			RobotRaconteurNode::TryHandleException(node, &exp);
		}
	}

	std::string ClientContext::FindObjectType(const std::string &path)
	{
		RR_SHARED_PTR<detail::sync_async_handler<std::string> > h = RR_MAKE_SHARED<detail::sync_async_handler<std::string> >(RR_MAKE_SHARED<ServiceException>("Error retrieving object type"));
		AsyncFindObjectType(path, boost::bind(&detail::sync_async_handler<std::string>::operator(), h, _1, _2), GetNode()->GetRequestTimeout());
		return *h->end();
	}

	void ClientContext::AsyncFindObjectType(const std::string &path, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_ObjectTypeName, "");
		e->AddElement("clientversion", stringToRRArray(ROBOTRACONTEUR_VERSION_TEXT));
		//MessageElement m = e.AddElement("ObjectPath", path);
		e->ServicePath = path;
		AsyncProcessRequest(e, boost::bind(&ClientContext::AsyncFindObjectType1, shared_from_this(), _1, _2, handler), timeout);
	}

	void ClientContext::AsyncFindObjectType1(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		if (err)
		{
			detail::InvokeHandlerWithException(node, handler, err);			
			return;
		}
		else
		{
			try
			{
				std::string objecttype = ret->FindElement("objecttype")->CastDataToString();

				try
				{
					RR_SHARED_PTR<std::string> ret = RR_MAKE_SHARED<std::string>(objecttype);
					detail::InvokeHandler(node, handler, ret);
				}
				catch (std::exception& exp)
				{
					RobotRaconteurNode::TryHandleException(node, &exp);
				}
			}			
			catch (std::exception& err)
			{
				detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
			}
		}
	}


	RR_INTRUSIVE_PTR<MessageEntry> ClientContext::ProcessRequest(RR_INTRUSIVE_PTR<MessageEntry> m)
	{
		
		if (!GetConnected()) throw ConnectionException("Service client not connected");

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
		SendMessage(m);


		boost::posix_time::ptime request_start = GetNode()->NowUTC();
		uint32_t request_timeout = GetNode()->GetRequestTimeout();
		while (true)
		{

			{
				boost::mutex::scoped_lock lock(outstanding_requests_lock);
				if (t->ret)
					break;
			}
			GetNode()->CheckConnection(GetLocalEndpoint());



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

		if (!GetConnected()) throw ConnectionException("Service client not connected");



		RR_INTRUSIVE_PTR<MessageEntry> rec_message;
		{
			boost::mutex::scoped_lock lock(outstanding_requests_lock);
			outstanding_requests.erase(mytransid);
			if (!t->ret) throw ConnectionException("Service client not connected");
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


	void ClientContext::AsyncProcessRequest(RR_INTRUSIVE_PTR<MessageEntry> m, RR_MOVE_ARG(boost::function<void(RR_INTRUSIVE_PTR<MessageEntry>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		if (!GetConnected()) throw ConnectionException("Service client not connected");

		uint32_t mytransid;

		RR_SHARED_PTR<outstanding_request> t = RR_MAKE_SHARED<outstanding_request>();
		t->evt = GetNode()->CreateAutoResetEvent();
		t->handler.swap(handler);

		{
			boost::mutex::scoped_lock lock(outstanding_requests_lock);

			do {
				request_number++;
				if (request_number >= std::numeric_limits<uint32_t>::max()) request_number = 0;
				m->RequestID = request_number;
				mytransid = request_number;
			} while (outstanding_requests.count(mytransid) != 0);

			if (timeout != RR_TIMEOUT_INFINITE)
			{
				RR_SHARED_PTR<Timer> timer = GetNode()->CreateTimer(boost::posix_time::milliseconds(timeout), boost::bind(&ClientContext::AsyncProcessRequest_timeout, shared_from_this(), _1, mytransid), true);
				timer->Start();
				t->timer = timer;
			}

			outstanding_requests.insert(std::make_pair(mytransid, t));

			try
			{
				if (!connection_test_timer)
				{
					connection_test_timer = RobotRaconteurNode::s()->CreateTimer(boost::posix_time::milliseconds(50), boost::bind(&ClientContext::connection_test, shared_from_this(), _1));
					connection_test_timer->Start();

				}
			}
			catch (std::exception&) {}

		}

		//Console.WriteLine("Sent " + m.RequestID + " " + m.EntryType + " " + m.MemberName);
		boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&ClientContext::AsyncProcessRequest_err, shared_from_this(), _1, mytransid);
		AsyncSendMessage(m, h);



	}

	void ClientContext::connection_test(const TimerEvent& ev)
	{
		try
		{
			GetNode()->CheckConnection(GetLocalEndpoint());
		}
		catch (RobotRaconteurException& exp)
		{
			std::list<boost::function<void(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> error)> > handlers;

			{
				boost::mutex::scoped_lock lock(outstanding_requests_lock);
				BOOST_FOREACH(RR_SHARED_PTR<outstanding_request>& e, outstanding_requests | boost::adaptors::map_values)
				{
					e->evt->Set();

					if (e->handler) handlers.push_back(e->handler);
				}

				outstanding_requests.clear();

				if (connection_test_timer)
				{
					try
					{
						connection_test_timer->Stop();
					}

					catch (std::exception&) {}
					connection_test_timer.reset();
				}
			}


			BOOST_FOREACH(boost::function<void(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> error)>& e, handlers)
			{				
				detail::InvokeHandlerWithException(node,e,exp);				
			}
		}
	}

	void ClientContext::AsyncProcessRequest_err(RR_SHARED_PTR<RobotRaconteurException> error, uint32_t requestid)
	{
		if (!error) return;
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



	void ClientContext::AsyncProcessRequest_timeout(const TimerEvent& error, uint32_t requestid)
	{
		if (!error.stopped)
		{
			RR_SHARED_PTR<outstanding_request> t;
			try
			{


				{
					boost::mutex::scoped_lock lock(outstanding_requests_lock);
					RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_request> >::iterator e1 = outstanding_requests.find(requestid);
					if (e1 == outstanding_requests.end()) return;
					t = e1->second;
					outstanding_requests.erase(e1);
				}

				detail::InvokeHandlerWithException(node, t->handler, RR_MAKE_SHARED<RequestTimeoutException>("Request timeout"));
			}
			catch (std::exception& exp)
			{
				RobotRaconteurNode::TryHandleException(node, &exp);
			}
		}
	}

	void ClientContext::SendMessage(RR_INTRUSIVE_PTR<MessageEntry> m)
	{
		//m.ServiceName = ServiceName;

		if (!GetConnected())
			throw ConnectionException("Client has been disconnected");
		/*boost::shared_lock<boost::shared_mutex> lock(message_lock);

		if (!GetConnected())
		throw ConnectionException("Client has been disconnected");*/

		RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
		mm->header = CreateMessageHeader();

		mm->entries.push_back(m);

		std::vector<std::string> v;
		boost::split(v, m->MetaData, boost::is_from_range('\n', '\n'), boost::algorithm::token_compress_on);

		if (std::find(v.begin(), v.end(), "unreliable") != v.end())
		{
			mm->header->MetaData = "unreliable\n";
		}

		//LastMessageSentTime = GetNode()->NowUTC();

		Endpoint::SendMessage(mm);


	}

	void ClientContext::AsyncSendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
	{
		//m.ServiceName = ServiceName;

		if (!GetConnected())
			throw ConnectionException("Client has been disconnected");
		/*boost::shared_lock<boost::shared_mutex> lock(message_lock);

		if (!GetConnected())
		throw ConnectionException("Client has been disconnected");*/

		RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
		mm->header = CreateMessageHeader();

		mm->entries.push_back(m);

		//LastMessageSentTime = GetNode()->NowUTC();

		std::vector<std::string> v;
		boost::split(v, m->MetaData, boost::is_from_range('\n', '\n'), boost::algorithm::token_compress_on);

		if (std::find(v.begin(), v.end(), "unreliable") != v.end())
		{
			mm->header->MetaData = "unreliable\n";
		}

		Endpoint::AsyncSendMessage(mm, callback);
	}

	void ClientContext_empty_handler() {}

	void ClientContext::MessageReceived(RR_INTRUSIVE_PTR<Message> m)
	{


		if (!GetConnected()) return;


		SetLastMessageReceivedTime(GetNode()->NowUTC());

		if (m->entries.size() >= 1)
		{
			if (m->entries[0]->EntryType == MessageEntryType_ConnectClientRet || m->entries[0]->EntryType == MessageEntryType_ConnectClientCombinedRet)
			{
				SetRemoteEndpoint(m->header->SenderEndpoint);
				SetRemoteNodeID(m->header->SenderNodeID);
				SetRemoteNodeName(m->header->SenderNodeName);
			}

			if (m->entries[0]->EntryType == MessageEntryType_EndpointCheckCapability)
			{
				CheckEndpointCapabilityMessage(m);
				return;
			}
		}

		BOOST_FOREACH(RR_INTRUSIVE_PTR<MessageEntry>& mm, m->entries)
		{
			if (mm->Error == MessageErrorType_InvalidEndpoint)
			{
				AsyncClose(&ClientContext_empty_handler);
				return;
			}
			MessageEntryReceived(mm);
		}
	}

	void DispatchEvent_func(RR_SHARED_PTR<ServiceStub> stub, RR_INTRUSIVE_PTR<MessageEntry> m)
	{

		try
		{
			stub->DispatchEvent(m);
		}
		catch (std::exception& exp)
		{
			try { stub->RRGetNode()->HandleException(&exp); }
			catch (std::exception) {};
		}
	}

	void ClientContext::MessageEntryReceived(RR_INTRUSIVE_PTR<MessageEntry> m)
	{

		{
			//boost::recursive_mutex::scoped_lock lock (rec_loc);
			if (m->EntryType == MessageEntryType_EventReq)
			{
				RR_SHARED_PTR<ServiceStub> stub;
				{
					boost::mutex::scoped_lock lock(stubs_lock);
					RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceStub> >::iterator e1 = stubs.find(m->ServicePath);
					if (e1 == stubs.end()) return;
					stub = e1->second;
				}
				if (stub) RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&DispatchEvent_func, stub, m));
				//stub.DispatchEvent(m);

			}
			else if (m->EntryType == MessageEntryType_PropertyGetRes || m->EntryType == MessageEntryType_PropertySetRes 
				|| m->EntryType == MessageEntryType_FunctionCallRes || m->EntryType == MessageEntryType_ObjectTypeNameRet 
				|| m->EntryType == MessageEntryType_ConnectClientRet || m->EntryType == MessageEntryType_ConnectClientCombinedRet 
				|| m->EntryType == MessageEntryType_DisconnectClientRet || m->EntryType == MessageEntryType_GetServiceDescRet 
				|| (m->EntryType >= MessageEntryType_PipeConnectReq && m->EntryType <= MessageEntryType_PipeDisconnectRet) 
				|| m->EntryType == MessageEntryType_ClientSessionOpRet || m->EntryType == MessageEntryType_WireConnectRet 
				|| m->EntryType == MessageEntryType_WireDisconnectRet || m->EntryType == MessageEntryType_WirePeekInValueRet
				|| m->EntryType == MessageEntryType_WirePeekOutValueRet || m->EntryType == MessageEntryType_WirePokeOutValueRet
				|| m->EntryType == MessageEntryType_MemoryReadRet 
				|| m->EntryType == MessageEntryType_MemoryWriteRet || m->EntryType == MessageEntryType_MemoryGetParamRet
				|| m->EntryType == MessageEntryType_GeneratorNextRes)
			{
				// Console.WriteLine("Got " + m.RequestID + " " + m.EntryType + " " + m.MemberName);
				RR_SHARED_PTR<outstanding_request> t;
				uint32_t transid = m->RequestID;
				try
				{
					boost::mutex::scoped_lock lock(outstanding_requests_lock);
					RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_request> >::iterator e1 = outstanding_requests.find(transid);
					if (e1 == outstanding_requests.end()) return;
					t = e1->second;
					t->ret = m;
					try
					{
						if (t->timer) t->timer->Stop();
					}
					catch (std::exception&) {}

					t->evt->Set();

				}
				catch (std::exception&) {
					return;
				}

				if (t)
				{
					try
					{
						if (t->handler)
						{

							{
								boost::mutex::scoped_lock lock(outstanding_requests_lock);
								outstanding_requests.erase(transid);

								try
								{
									if (connection_test_timer)
									{
										bool async_running = false;
										BOOST_FOREACH(RR_SHARED_PTR<outstanding_request>& ee, outstanding_requests | boost::adaptors::map_values)
										{
											if (ee->handler) async_running = true;
										}

										if (!async_running)
										{
											try
											{
												connection_test_timer->Stop();
											}
											catch (std::exception&) {}

											connection_test_timer.reset();
										}
									}
								}
								catch (std::exception&) {}


							}

							if (m->Error == MessageErrorType_None)
							{
								t->handler(m, RR_SHARED_PTR<RobotRaconteurException>());
							}
							else if (m->Error == MessageErrorType_RemoteError)
							{
								RR_SHARED_PTR<RobotRaconteurException> err = RobotRaconteurExceptionUtil::MessageEntryToException(m);
								RR_SHARED_PTR<RobotRaconteurException> err2 = m_ServiceDef->DownCastException(err);
								t->handler(RR_INTRUSIVE_PTR<MessageEntry>(), err2);
							}
							else
							{
								t->handler(RR_INTRUSIVE_PTR<MessageEntry>(), RobotRaconteurExceptionUtil::MessageEntryToException(m));
							}
						}
					}
					catch (std::exception& exp)
					{
						RobotRaconteurNode::TryHandleException(node, &exp);
					}
				}

			}
			else if (m->EntryType == MessageEntryType_ServiceClosed)
			{


				try
				{
					AsyncClose(&ClientContext_empty_handler);
				}
				catch (std::exception&)
				{
					//rec_loc.lock();

					throw;
				}

				//rec_loc.lock();

			}
			else if (m->EntryType == MessageEntryType_ClientKeepAliveRet)
			{
			}
			else if (m->EntryType == MessageEntryType_PipePacket || m->EntryType == MessageEntryType_PipeClosed || m->EntryType == MessageEntryType_PipePacketRet)
			{
				RR_SHARED_PTR<ServiceStub> stub;
				{
					boost::mutex::scoped_lock lock(stubs_lock);
					RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceStub> >::iterator e1 = stubs.find(m->ServicePath);
					if (e1 == stubs.end()) return;
					stub = e1->second;
				}
				if (stub) stub->DispatchPipeMessage(m);
				return;
			}
			else if (m->EntryType == MessageEntryType_WirePacket || m->EntryType == MessageEntryType_WireClosed)
			{
				RR_SHARED_PTR<ServiceStub> stub;
				{
					boost::mutex::scoped_lock lock(stubs_lock);
					RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceStub> >::iterator e1 = stubs.find(m->ServicePath);
					if (e1 == stubs.end()) return;
					stub = e1->second;
				}
				if (stub) stub->DispatchWireMessage(m);
			}
			else if (m->EntryType == MessageEntryType_ServicePathReleasedReq)
			{
				boost::mutex::scoped_lock lock(stubs_lock);
				std::string path = m->ServicePath;
				std::vector<std::string> objkeys;
				BOOST_FOREACH(const std::string& ee, stubs | boost::adaptors::map_keys)
				{
					if (ee == path || boost::starts_with(ee, path + "."))
					{
						objkeys.push_back(ee);
					}
				}


				//= stubs.Keys->Where(x => (x->Length >= path.length()) && (x->substr(0, path.length()) == path))->ToArray();
				//if (objkeys.Count() == 0) throw new ServiceException("Unknown service path");

				BOOST_FOREACH(std::string& path1, objkeys)
				{
					try
					{
						RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceStub> >::iterator e1 = stubs.find(path1);
						if (e1 == stubs.end()) continue;
						e1->second->RRClose();
						stubs.erase(e1);
					}
					catch (std::exception&)
					{
					}

				}

			}
			else if (m->EntryType == MessageEntryType_CallbackCallReq)
			{
				ProcessCallbackCall(m);
			}
			else
			{
				throw ServiceException("Unknown service command");
			}

		}
	}

	/*RR_INTRUSIVE_PTR<MessageElementStructure> ClientContext::PackStructure(const RR_SHARED_PTR<void> &s)
	{
	return GetServiceDef()->PackStructure(s);
	;
	}

	template<typename T>
	T ClientContext::UnpackStructure(const RR_INTRUSIVE_PTR<MessageElementStructure> &l)
	{
	return GetServiceDef()->UnpackStructure<T>(l);
	}*/

	std::string ClientContext::GetServiceName() const
	{
		return m_ServiceName;
	}

	bool ClientContext::GetConnected() const
	{
		return m_Connected;
	}

	RR_SHARED_PTR<RRObject> ClientContext::ConnectService(RR_SHARED_PTR<Transport> c, const std::string &url, const std::string &username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype)
	{
		RR_SHARED_PTR<detail::sync_async_handler<RRObject> > d = RR_MAKE_SHARED<detail::sync_async_handler<RRObject> >(RR_MAKE_SHARED<ConnectionException>("Connection timed out"));
		AsyncConnectService(c, std::string(url), std::string(username), credentials, std::string(objecttype), boost::bind(&detail::sync_async_handler<RRObject>::operator(), d, _1, _2), GetNode()->GetRequestTimeout());
		return d->end();
	}

	RR_SHARED_PTR<RRObject> ClientContext::ConnectService(RR_SHARED_PTR<Transport> c, RR_SHARED_PTR<ITransportConnection> tc, const std::string &url, const std::string &username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype)
	{
		RR_SHARED_PTR<detail::sync_async_handler<RRObject> > d = RR_MAKE_SHARED<detail::sync_async_handler<RRObject> >(RR_MAKE_SHARED<ConnectionException>("Connection timed out"));
		AsyncConnectService(c, tc, std::string(url), std::string(username), credentials, std::string(objecttype), boost::bind(&detail::sync_async_handler<RRObject>::operator(), d, _1, _2), GetNode()->GetRequestTimeout());
		return d->end();
	}

	void ClientContext::AsyncConnectService(RR_SHARED_PTR<Transport> c, const std::string &url, const std::string &username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		boost::recursive_mutex::scoped_lock cl_lock(connect_lock);

		ParseConnectionURLResult url_res = ParseConnectionURL(url);

		m_ServiceName = url_res.service;


		if (!c->CanConnectService(url))
			throw ServiceException("Invalid transport");


		RR_SHARED_PTR<detail::async_timeout_wrapper<RRObject> > d = RR_MAKE_SHARED<detail::async_timeout_wrapper<RRObject> >(GetNode(), handler, boost::bind(&detail::async_timeout_wrapper_closer<RRObject, ClientContext>, _1));

		boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>) > h1(boost::bind(&detail::async_timeout_wrapper<RRObject>::operator(), d, _1, _2));
		boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> h(boost::bind(&ClientContext::AsyncConnectService1, shared_from_this(), c, _1, _2, std::string(url), std::string(username), credentials, std::string(objecttype), h1));

		c->AsyncCreateTransportConnection(url, RR_STATIC_POINTER_CAST<Endpoint>(shared_from_this()), h);
		d->start_timer(timeout, RR_MAKE_SHARED<ConnectionException>("Timeout during operation"));

	}

	
	void ClientContext::AsyncConnectService1(RR_SHARED_PTR<Transport> c, RR_SHARED_PTR<ITransportConnection> tc, RR_SHARED_PTR<RobotRaconteurException> e, const std::string& url, const std::string& username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		boost::recursive_mutex::scoped_lock cl_lock(connect_lock);
		if (e)
		{
			detail::InvokeHandlerWithException(node, handler, e);
			
			AsyncClose(&ClientContext_empty_handler);
			return;
		}
		else
		{
			try
			{
				AsyncConnectService(c, tc, std::string(url), std::string(username), credentials, objecttype, RR_MOVE(handler), 0);
			}
			
			catch (std::exception& err)
			{
				detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
				AsyncClose(&ClientContext_empty_handler);
			}
		}
	}


	void ClientContext::AsyncConnectService(RR_SHARED_PTR<Transport> c, RR_SHARED_PTR<ITransportConnection> tc, const std::string &url, const std::string &username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		boost::recursive_mutex::scoped_lock cl_lock(connect_lock);

		this->connecttransport = c;
		this->connecturl = url;

		bool use_message3_ = tc->CheckCapabilityActive(TransportCapabilityCode_MESSAGE3_BASIC_PAGE | 
			TransportCapabilityCode_MESSAGE3_BASIC_ENABLE);

		bool use_combined_connection_;
		
		if (!use_message3_)
		{
			use_combined_connection_ = tc->CheckCapabilityActive(TransportCapabilityCode_MESSAGE2_BASIC_PAGE
				| TransportCapabilityCode_MESSAGE2_BASIC_CONNECTCOMBINED);
		}
		else
		{
			use_combined_connection_ = tc->CheckCapabilityActive(TransportCapabilityCode_MESSAGE3_BASIC_PAGE
				| TransportCapabilityCode_MESSAGE3_BASIC_CONNECTCOMBINED);
		}

		use_message3.store(use_message3_);
		use_combined_connection.store(use_combined_connection_);
		ParseConnectionURLResult url_res = ParseConnectionURL(url);

		m_ServiceName = url_res.service;

		if (!(url_res.nodeid.IsAnyNode() && url_res.nodename != ""))
		{
			//RR_SHARED_PTR<NodeID> remid = RR_MAKE_SHARED<NodeID>(s[0]);
			SetRemoteNodeID(url_res.nodeid);
		}
		else
		{
			SetRemoteNodeName(url_res.nodename);
		}



		TransportConnection = tc;

		SetTransport(c->TransportID);
		SetTransportConnection(tc);
		SetRemoteEndpoint(0);

		m_Connected = true;

		//std::cout << "AsyncConnectService" << std::endl;

		try
		{
			ClientServiceListener(shared_from_this(), ClientServiceListenerEventType_TransportConnectionConnected, RR_SHARED_PTR<RRObject>());
		}
		catch (std::exception& exp)
		{
			RobotRaconteurNode::TryHandleException(node, &exp);
		}

		if (use_combined_connection.load())
		{
			RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry();
			m->ServicePath = GetServiceName();
			m->MemberName = "connectclientcombined";
			m->EntryType = MessageEntryType_ConnectClientCombined;

			m->AddElement("clientversion", stringToRRArray(ROBOTRACONTEUR_VERSION_TEXT));
			m->AddElement("returnservicedefs", stringToRRArray("true"));
			if (username.size() != 0)
			{
				m->AddElement("username", stringToRRArray(username));
			}

			if (credentials)
			{
				m->AddElement("credentials", GetNode()->PackMapType<std::string, RRValue>(credentials));
			}

			AsyncProcessRequest(m, boost::bind(&ClientContext::AsyncConnectService7, shared_from_this(), _1, _2, objecttype, handler), timeout);

			return;
		}


		if (timeout == RR_TIMEOUT_INFINITE)
		{
			AsyncPullServiceDefinitionAndImports("", boost::bind(&ClientContext::AsyncConnectService2, shared_from_this(), _1, _2, std::string(username), credentials, std::string(objecttype), handler), GetNode()->GetRequestTimeout());
		}
		else
		{

			RR_SHARED_PTR<detail::async_timeout_wrapper<RRObject> > d = RR_MAKE_SHARED<detail::async_timeout_wrapper<RRObject> >(GetNode(), handler, boost::bind(&detail::async_timeout_wrapper_closer<RRObject, ClientContext>, _1));
			boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> h1(boost::bind(&detail::async_timeout_wrapper<RRObject>::operator(), d, _1, _2));
			AsyncPullServiceDefinitionAndImports("", boost::bind(&ClientContext::AsyncConnectService2, shared_from_this(), _1, _2, std::string(username), credentials, std::string(objecttype), h1), GetNode()->GetRequestTimeout());
			d->start_timer(timeout, RR_MAKE_SHARED<ConnectionException>("Timeout during operation"));
		}
	}


	void ClientContext::AsyncConnectService2(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, RR_SHARED_PTR<RobotRaconteurException> e, const std::string& username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		boost::recursive_mutex::scoped_lock cl_lock(connect_lock);
		//std::cout << "AsyncConnectService2" << std::endl;
		if (e)
		{
			detail::InvokeHandlerWithException(node, handler, e);
			
			AsyncClose(&ClientContext_empty_handler);
			return;
		}
		else
		{
			try
			{
				{
					boost::mutex::scoped_lock lock(pulled_service_defs_lock);
					BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition>& e, *d)
					{
						if (pulled_service_defs.find(e->Name) == pulled_service_defs.end())
						{
							pulled_service_defs.insert(std::make_pair(e->Name, e));
						}
					}
				}


				//Determine the type of the root object

				RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_ObjectTypeName, "");
				//e.AddElement("servicepath", ServiceName);
				e->ServicePath = GetServiceName();
				e->AddElement("clientversion", stringToRRArray(ROBOTRACONTEUR_VERSION_TEXT));

				//std::cout << "AsyncConnectService2_1" << std::endl;

				AsyncProcessRequest(e, boost::bind(&ClientContext::AsyncConnectService3, shared_from_this(), _1, _2, username, credentials, std::string(objecttype), d, handler), GetNode()->GetRequestTimeout());
			}
			catch (std::exception& err)
			{
				detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);

				AsyncClose(&ClientContext_empty_handler);
			}
		}
	}

	void ClientContext::AsyncConnectService3(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> e, const std::string& username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype, RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		boost::recursive_mutex::scoped_lock cl_lock(connect_lock);
		//std::cout << "AsyncConnectService3" << std::endl;
		if (e)
		{
			detail::InvokeHandlerWithException(node, handler, e);
			AsyncClose(&ClientContext_empty_handler);
			return;
		}
		else
		{
			try
			{

				std::string type = ret->FindElement("objecttype")->CastDataToString();
				if (type == "")
					throw ObjectNotFoundException("Could not find object type");
				;

				//If we want a desired type, try to figure out if the upcast is valid
				if (objecttype != "")
				{
					VerifyObjectImplements(type, objecttype);
					type = objecttype;


					AsyncPullServiceDefinitionAndImports(SplitQualifiedName(type).get<0>(), boost::bind(&ClientContext::AsyncConnectService4, shared_from_this(), _1, _2, username, credentials, objecttype, type, handler), GetNode()->GetRequestTimeout());
					return;
				}

				//std::cout << "AsyncConnectService3_1" << std::endl;
				AsyncConnectService4(d, RR_SHARED_PTR<RobotRaconteurException>(), username, credentials, objecttype, type, (handler));

			}
			catch (RobotRaconteurException& err)
			{
				detail::InvokeHandlerWithException(node, handler, err);				

				AsyncClose(&ClientContext_empty_handler);
			}
			catch (std::exception& err)
			{
				detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
				AsyncClose(&ClientContext_empty_handler);
			}
		}

	}

	void ClientContext::AsyncConnectService4(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, RR_SHARED_PTR<RobotRaconteurException> e, const std::string& username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype, const std::string& type, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		boost::recursive_mutex::scoped_lock cl_lock(connect_lock);
		//std::cout << "AsyncConnectService4" << std::endl;
		if (e)
		{
			detail::InvokeHandlerWithException(node, handler, e);			
			AsyncClose(&ClientContext_empty_handler);
			return;
		}
		else
		{
			try
			{
				if (GetNode()->GetDynamicServiceFactory())
				{
					use_pulled_types = true;

					std::vector<RR_SHARED_PTR<ServiceDefinition> > missingdefs = std::vector<RR_SHARED_PTR<ServiceDefinition> >();

					BOOST_FOREACH(RR_SHARED_PTR<RobotRaconteur::ServiceDefinition>& di, *d)
					{

						std::vector<std::string> stypes = GetPulledServiceTypes();
						if (std::find(stypes.begin(), stypes.end(), (di->Name)) == stypes.end())
						{
							missingdefs.push_back(di);
						}

					}
					if (missingdefs.size() > 0)
					{
						std::vector<std::string> missingdefs_vec;
						BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition>& e, missingdefs)
						{
							missingdefs_vec.push_back(e->ToString());
						}
						std::vector<RR_SHARED_PTR<ServiceFactory> > di2 = GetNode()->GetDynamicServiceFactory()->CreateServiceFactories(missingdefs_vec);

						for (size_t i = 0; i < missingdefs.size(); i++)
						{
							boost::mutex::scoped_lock lock(pulled_service_types_lock);
							pulled_service_types.insert(std::make_pair(di2.at(i)->GetServiceName(), di2.at(i)));
						}
					}

					m_ServiceDef = GetPulledServiceType(d->at(0)->Name);
				}
				else
				{
					try
					{

						m_ServiceDef = GetNode()->GetServiceType(d->at(0)->Name);
					}
					catch (std::exception&)
					{
						throw ServiceException("Could not find correct service factory for remote service");
					}
				}



				//std::cout << "AsyncConnectService4_1" << std::endl;
				RR_INTRUSIVE_PTR<MessageEntry> e2 = CreateMessageEntry();
				e2->ServicePath = GetServiceName();
				e2->MemberName = "registerclient";
				e2->EntryType = MessageEntryType_ConnectClient;
				AsyncProcessRequest(e2, boost::bind(&ClientContext::AsyncConnectService5, shared_from_this(), _1, _2, username, credentials, objecttype, type, d, handler), GetNode()->GetRequestTimeout());
			}			
			catch (std::exception& err)
			{
				detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
				AsyncClose(&ClientContext_empty_handler);
			}
		}
	}

	void ClientContext::AsyncConnectService5(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> e, const std::string& username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, const std::string& objecttype, const std::string& type, RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		boost::recursive_mutex::scoped_lock cl_lock(connect_lock);
		//std::cout << "AsyncConnectService5" << std::endl;
		if (e)
		{
			detail::InvokeHandlerWithException(node, handler, e);
			AsyncClose(&ClientContext_empty_handler);
			return;
		}
		else
		{
			try
			{

				if (username != "")
				{
					try
					{
						//std::cout << "AsyncConnectService5_1" << std::endl;
						AsyncAuthenticateUser(username, credentials, boost::bind(&ClientContext::AsyncConnectService6, shared_from_this(), _1, _2, type, d, handler), GetNode()->GetRequestTimeout());
					}
					catch (AuthenticationException& ex)
					{
						try
						{
							AsyncClose(&ClientContext_empty_handler);
						}
						catch (std::exception&) {}
						throw ex;
					}

				}
				else
				{
					//std::cout << "AsyncConnectService5_1" << std::endl;
					AsyncConnectService6(RR_MAKE_SHARED<std::string>("OK"), RR_SHARED_PTR<RobotRaconteurException>(), type, d, (handler));
				}


			}			
			catch (std::exception& err)
			{
				detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);

				AsyncClose(&ClientContext_empty_handler);
			}
		}
	}

	void ClientContext::AsyncConnectService6(RR_SHARED_PTR<std::string> ret, RR_SHARED_PTR<RobotRaconteurException> e, const std::string& type, RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		boost::recursive_mutex::scoped_lock cl_lock(connect_lock);
		//std::cout << "AsyncConnectService6" << std::endl;
		if (e)
		{
			detail::InvokeHandlerWithException(node, handler, e);			
			AsyncClose(&ClientContext_empty_handler);
			return;
		}
		else if (*ret != "OK")
		{
			
			detail::InvokeHandlerWithException(node, handler, RR_MAKE_SHARED<AuthenticationException>("Invalid username or credentials"));
			
			AsyncClose(&ClientContext_empty_handler);
			return;

		}
		else
		{
			try
			{
				RR_SHARED_PTR<ServiceStub> stub = GetServiceDef()->CreateStub(type, GetServiceName(), shared_from_this());
				{
					boost::mutex::scoped_lock lock(stubs_lock);
					stubs.insert(make_pair(GetServiceName(), stub));
				}

				//std::cout << "handler" << std::endl;
				RR_SHARED_PTR<RRObject> ret = RR_STATIC_POINTER_CAST<RRObject>(stub);
				detail::InvokeHandler(node, handler, ret);				

			}			
			catch (std::exception& err)
			{
				detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
				AsyncClose(&ClientContext_empty_handler);
			}
		}
	}

	void ClientContext::AsyncConnectService7(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> e, const std::string& objecttype, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		boost::recursive_mutex::scoped_lock cl_lock(connect_lock);

		if (e)
		{
			detail::InvokeHandlerWithException(node, handler, e);
			AsyncClose(&ClientContext_empty_handler);
			return;
		}

		try
		{
			{
				boost::mutex::scoped_lock lock(pulled_service_defs_lock);
				RR_INTRUSIVE_PTR<MessageElementList> l = ret->FindElement("servicedefs")->CastData<MessageElementList>();
				if (!l) throw ServiceException("servicedefs cannot be null on connect");
				BOOST_FOREACH(RR_INTRUSIVE_PTR<MessageElement> l1, l->Elements)
				{
					RR_SHARED_PTR<ServiceDefinition> d1 = RR_MAKE_SHARED<ServiceDefinition>();
					std::vector<RobotRaconteurParseException> warnings;
					d1->FromString(l1->CastDataToString(), warnings);
					pulled_service_defs.insert(std::make_pair(d1->Name, d1));
				}
			}

			std::string type = ret->FindElement("objecttype")->CastDataToString();

			if (type == "")
				throw ObjectNotFoundException("Could not find object type");


			//If we want a desired type, try to figure out if the upcast is valid
			if (objecttype != "")
			{
				VerifyObjectImplements(type, objecttype);
				type = objecttype;
			}

			try
			{
				if (GetNode()->GetDynamicServiceFactory())
				{
					boost::mutex::scoped_lock lock(pulled_service_defs_lock);
					use_pulled_types = true;

					std::vector<std::string> pulleddefs_str;
					BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition>ee, pulled_service_defs | boost::adaptors::map_values)
					{
						pulleddefs_str.push_back(ee->ToString());
					}

					std::vector<RR_SHARED_PTR<ServiceFactory> > di2 = GetNode()->GetDynamicServiceFactory()->CreateServiceFactories(pulleddefs_str);

					for (size_t i = 0; i < pulleddefs_str.size(); i++)
					{
						boost::mutex::scoped_lock lock(pulled_service_types_lock);
						pulled_service_types.insert(std::make_pair(di2.at(i)->GetServiceName(), di2.at(i)));
					}

					m_ServiceDef = GetPulledServiceType(SplitQualifiedName(type).get<0>());
				}
				else
				{
					m_ServiceDef = GetNode()->GetServiceType(SplitQualifiedName(type).get<0>());
				}
			}
			catch (std::exception&)
			{
				throw ServiceException("Could not find correct service factory for remote service");
			}

			RR_SHARED_PTR<ServiceStub> stub = GetServiceDef()->CreateStub(type, GetServiceName(), shared_from_this());
			{
				boost::mutex::scoped_lock lock(stubs_lock);
				stubs.insert(make_pair(GetServiceName(), stub));
			}

			RR_SHARED_PTR<RRObject> ret = RR_STATIC_POINTER_CAST<RRObject>(stub);
			detail::InvokeHandler(node, handler, ret);
			
		}		
		catch (std::exception& err)
		{
			detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
			AsyncClose(&ClientContext_empty_handler);
		}
	}


	bool ClientContext::VerifyObjectImplements2(const std::string& objecttype, const std::string& implementstype)
	{
		if (objecttype == implementstype) return true;

		boost::mutex::scoped_lock lock(pulled_service_defs_lock);

		boost::tuple<std::string, std::string> s1 = SplitQualifiedName(objecttype);


		if (pulled_service_defs.find(s1.get<0>()) == pulled_service_defs.end())
		{
			RR_SHARED_PTR<ServiceDefinition> d1 = PullServiceDefinition(s1.get<0>());
			pulled_service_defs.insert(std::make_pair(d1->Name, d1));
		}

		std::map<std::string, RR_SHARED_PTR<ServiceDefinition> >::iterator e1 = pulled_service_defs.find(s1.get<0>());
		if (e1 == pulled_service_defs.end()) return false;

		RR_SHARED_PTR<ServiceDefinition> d = e1->second;

		RR_SHARED_PTR<ServiceEntryDefinition> o;

		BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition>& e, d->Objects)
		{
			if (e->Name == s1.get<1>()) o = e;
		}

		if (!o) return false;

		BOOST_FOREACH(std::string& e, o->Implements)
		{
			std::string deftype = d->Name;
			std::string objtype = "";

			if (!boost::contains(e, "."))
			{
				objtype = e;
			}
			else
			{
				boost::tuple<std::string, std::string> s2 = SplitQualifiedName(e);
				deftype = s2.get<0>();
				objtype = s2.get<1>();
			}


			if ((deftype + "." + objtype) == implementstype) return true;

			if (VerifyObjectImplements2(deftype + "." + objtype, implementstype)) return true;

		}

		return false;

	}

	bool ClientContext::VerifyObjectImplements(const std::string& objecttype, const std::string& implementstype)
	{
		if (!VerifyObjectImplements2(objecttype, implementstype))
			throw ServiceException("Invalid object type");
		return true;
	}

	void ClientContext::Close()
	{
		RR_SHARED_PTR<detail::sync_async_handler<void> > h = RR_MAKE_SHARED<detail::sync_async_handler<void> >(RR_MAKE_SHARED<RequestTimeoutException>(""));
		AsyncClose(boost::bind(&detail::sync_async_handler<void>::operator(), h));

		try
		{
			h->end_void();
		}
		catch (std::exception&)
		{

		}
	}

	void ClientContext::AsyncClose(RR_MOVE_ARG(boost::function<void()>) handler)
	{


		boost::mutex::scoped_lock lock3(close_lock);
		{
			boost::mutex::scoped_lock lock(m_Connected_lock);
			if (!m_Connected) return;
		}
		try
		{
			RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_DisconnectClient, "");
			e->AddElement("servicename", stringToRRArray(GetServiceName()));

			AsyncProcessRequest(e, boost::bind(&ClientContext::AsyncClose1, shared_from_this(), _1, _2, handler), 500);
		}
		catch (std::exception&)
		{
			{
				boost::mutex::scoped_lock lock(m_Connected_lock);
				m_Connected = false;
			}
			
			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&ClientContext::AsyncClose1, shared_from_this(), RR_INTRUSIVE_PTR<MessageEntry>(), RR_SHARED_PTR<RobotRaconteurException>(), handler), true);
			
		}
	}

	void ClientContext::AsyncClose1(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void()>& handler)
	{

		{

			{
				boost::mutex::scoped_lock lock(m_Connected_lock);
				m_Connected = false;
			}

			std::list<boost::function<void(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> error)> > handlers;

			{
				boost::mutex::scoped_lock lock(outstanding_requests_lock);
				BOOST_FOREACH(RR_SHARED_PTR<outstanding_request>& e, outstanding_requests | boost::adaptors::map_values)
				{
					e->evt->Set();

					if (e->handler) handlers.push_back(e->handler);
				}

				outstanding_requests.clear();

				if (connection_test_timer)
				{
					try
					{
						connection_test_timer->Stop();
					}
					catch (std::exception&) {}
					connection_test_timer.reset();
				}
			}


			BOOST_FOREACH(boost::function<void(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> error)>& e, handlers)
			{
				try
				{
					e(RR_INTRUSIVE_PTR<MessageEntry>(), RR_MAKE_SHARED<ConnectionException>("Connection closed"));
				}
				catch (std::exception&) {}
			}


			//TODO fix this...

			{
				boost::mutex::scoped_lock lock(stubs_lock);
				BOOST_FOREACH(RR_SHARED_PTR<ServiceStub>& s, stubs | boost::adaptors::map_values)
				{
					try
					{
						s->RRClose();
					}
					catch (std::exception&) {}
				}
			}

			{
				boost::mutex::scoped_lock lock(stubs_lock);
				stubs.clear();
			}
			{
				boost::mutex::scoped_lock lock(outstanding_requests_lock);
				outstanding_requests.clear();
			}
			//m_Connected = false;


			try
			{

				ClientServiceListener(shared_from_this(), ClientServiceListenerEventType_ClientClosed, RR_SHARED_PTR<RRObject>());
			}
			catch (std::exception& exp)
			{
				RobotRaconteurNode::TryHandleException(node, &exp);
			}
			catch (...) {}

			detail::InvokeHandler(node, handler);

		}

		//boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		GetNode()->DeleteEndpoint(shared_from_this());

	}

	void ClientContext::AsyncSendPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, bool unreliable, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) >& handler)
	{
		if (!unreliable)
		{
			AsyncSendMessage(m, (handler));
		}
		else
		{
			if (!GetConnected())
				throw ConnectionException("Client has been disconnected");

			if (UseMessage3())
			{
				RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
				mm->header = CreateMessageHeader();
				mm->header->MessageFlags |= MessageFlags_UNRELIABLE;

				mm->entries.push_back(m);
				//m.EntryType= MessageEntryType.PipePacket;
				boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&rr_context_emptyhandler, _1);
				Endpoint::AsyncSendMessage(mm, h);
			}
			else
			{

				RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
				mm->header = CreateMessageHeader();
				mm->header->MetaData = "unreliable\n";

				mm->entries.push_back(m);
				//m.EntryType= MessageEntryType.PipePacket;
				boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&rr_context_emptyhandler, _1);
				Endpoint::AsyncSendMessage(mm, h);
			}
		}

	}

	void ClientContext::SendWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m)
	{
		if (!GetConnected())
			throw ConnectionException("Client has been disconnected");

		if (UseMessage3())
		{
			RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
			mm->header = CreateMessageHeader();
			mm->header->MessageFlags |= MessageFlags_UNRELIABLE;

			mm->entries.push_back(m);
			//m.EntryType= MessageEntryType.PipePacket;
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&rr_context_emptyhandler, _1);
			Endpoint::AsyncSendMessage(mm, h);
		}
		else
		{
			RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
			mm->header = CreateMessageHeader();
			mm->header->MetaData = "unreliable\n";

			mm->entries.push_back(m);
			//m.EntryType= MessageEntryType.PipePacket;
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&rr_context_emptyhandler, _1);
			Endpoint::AsyncSendMessage(mm, h);
		}
	}

	RR_SHARED_PTR<ServiceDefinition> ClientContext::PullServiceDefinition(const std::string &ServiceType)
	{
		RR_SHARED_PTR<detail::sync_async_handler<ServiceDefinition> > d = RR_MAKE_SHARED<detail::sync_async_handler<ServiceDefinition> >(RR_MAKE_SHARED<ServiceException>("Could not pull service definition"));
		AsyncPullServiceDefinition(std::string(ServiceType), boost::bind(&detail::sync_async_handler<ServiceDefinition>::operator(), d, _1, _2), GetNode()->GetRequestTimeout());
		return d->end();

	}

	void ClientContext::AsyncPullServiceDefinition(const std::string &ServiceType, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<ServiceDefinition>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		RR_INTRUSIVE_PTR<MessageEntry> e3 = CreateMessageEntry(MessageEntryType_GetServiceDesc, "");
		//e.AddElement("servicepath", ServiceName);

		if (ServiceType != "")
			e3->AddElement("ServiceType", stringToRRArray(ServiceType));

		e3->ServicePath = GetServiceName();
		e3->AddElement("clientversion", stringToRRArray(ROBOTRACONTEUR_VERSION_TEXT));

		AsyncProcessRequest(e3, boost::bind(&ClientContext::AsyncPullServiceDefinition1, shared_from_this(), _1, _2, std::string(ServiceType),handler), timeout);
	}

	void ClientContext::AsyncPullServiceDefinition1(RR_INTRUSIVE_PTR<MessageEntry> ret3, RR_SHARED_PTR<RobotRaconteurException> err, const std::string& ServiceType, boost::function<void(RR_SHARED_PTR<ServiceDefinition>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		if (err)
		{
			detail::InvokeHandlerWithException(node, handler, err);
			return;
		}
		else
		{
			try
			{

				if (ret3->Error != MessageErrorType_None)
					throw RobotRaconteurExceptionUtil::MessageEntryToException(ret3);
				std::string def = ret3->FindElement("servicedef")->CastDataToString();
				if (def == "")
					throw ServiceNotFoundException("Could not find service definition");

				RR_SHARED_PTR<ServiceDefinition> d = RR_MAKE_SHARED<ServiceDefinition>();
				std::vector<RobotRaconteurParseException> w;
				d->FromString(def, w);

				if (ServiceType == "")
				{
					bool attrib_found = false;
					BOOST_FOREACH(RR_INTRUSIVE_PTR<MessageElement>& ee, ret3->elements)
					{
						if (ee->ElementName == "attributes")
							attrib_found = true;
					}


					if (attrib_found)
					{
						boost::mutex::scoped_lock lock(m_Attributes_lock);
						m_Attributes = rr_cast<RRMap<std::string, RRValue> >((GetNode()->UnpackMapType<std::string, RRValue>(ret3->FindElement("attributes")->CastData<MessageElementMap<std::string> >())))->GetStorageContainer();

					}

				}

				detail::InvokeHandler(node, handler, d);
								
			}
			catch (std::exception& err)
			{
				detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ConnectionError);
			}
		}
	}


	std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > ClientContext::GetAttributes()
	{
		boost::mutex m_Attributes_lock;
		return m_Attributes;
	}

	void ClientContext::AsyncPullServiceDefinitionAndImports(const std::string &servicetype, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > >, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		boost::posix_time::ptime timeout_time = GetNode()->NowUTC() + boost::posix_time::milliseconds(timeout);

		AsyncPullServiceDefinition(servicetype, boost::bind(&ClientContext::AsyncPullServiceDefinitionAndImports1, shared_from_this(), _1, _2, std::string(servicetype), handler, timeout_time), boost::numeric_cast<uint32_t>((timeout_time - GetNode()->NowUTC()).total_milliseconds()));
	}

	void ClientContext::AsyncPullServiceDefinitionAndImports1(RR_SHARED_PTR<ServiceDefinition> root, RR_SHARED_PTR<RobotRaconteurException> err, const std::string& servicetype, boost::function<void(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > >, RR_SHARED_PTR<RobotRaconteurException>)>& handler, boost::posix_time::ptime timeout_time)
	{
		if (err)
		{
			detail::InvokeHandlerWithException(node, handler, err);			
			return;
		}
		else
		{
			try
			{
				RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > defs = RR_MAKE_SHARED<std::vector<RR_SHARED_PTR<ServiceDefinition> > >();

				defs->push_back(root);

				if (root->Imports.empty())
				{					
					detail::InvokeHandler(node, handler, defs);
					return;					
				}
				else
				{
					std::vector<std::string>::const_iterator s = root->Imports.begin();
					std::vector<std::string>::const_iterator e = root->Imports.end();

					while (e != s)
					{
						bool defs_f = false;
						BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition>& ee, *defs)
						{
							if (ee->Name == *s)
								defs_f = true;
						}
						if (!defs_f)
						{
							boost::function<void(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > >, RR_SHARED_PTR<RobotRaconteurException>)> handler2;
							handler2.swap(handler);
							AsyncPullServiceDefinitionAndImports(*s, boost::bind(&ClientContext::AsyncPullServiceDefinitionAndImports2, shared_from_this(), _1, _2, boost::make_tuple(std::string(servicetype), root, defs, s, e, handler2, timeout_time)), boost::numeric_cast<uint32_t>((timeout_time - GetNode()->NowUTC()).total_milliseconds()));
							return;
						}
						s++;
					}
				}

				
				detail::InvokeHandler(node, handler, defs);
				return;
				

			}			
			catch (std::exception& err)
			{
				detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ConnectionError);				
			}
		}
	}

	void ClientContext::AsyncPullServiceDefinitionAndImports2(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > imported, RR_SHARED_PTR<RobotRaconteurException> err, boost::tuple<std::string, RR_SHARED_PTR<ServiceDefinition>, RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > >, std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator, boost::function<void(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > >, RR_SHARED_PTR<RobotRaconteurException>)>, boost::posix_time::ptime> args)
	{

		std::string servicetype = args.get<0>();
		RR_SHARED_PTR<ServiceDefinition> root = args.get<1>();
		RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > defs = args.get<2>();
		std::vector<std::string>::const_iterator s = args.get<3>();
		std::vector<std::string>::const_iterator e = args.get<4>();
		boost::function<void(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > >, RR_SHARED_PTR<RobotRaconteurException>)> handler = args.get<5>();
		boost::posix_time::ptime timeout_time = args.get<6>();

		if (err)
		{			
			detail::InvokeHandlerWithException(node, handler, err);
		}
		else
		{
			try
			{

				BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition>& ee2, *imported)
				{
					bool defs_f = false;
					BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition>& ee, *defs)
					{
						if (ee->Name == ee2->Name)
							defs_f = true;
					}
					if (!defs_f)
					{
						defs->push_back(ee2);
					}
				}

				s++;
				while (e != s)
				{
					bool defs_f = false;
					BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition>& ee, *defs)
					{
						if (ee->Name == *s)
							defs_f = true;
					}
					if (!defs_f)
					{
						AsyncPullServiceDefinitionAndImports(*s, boost::bind(&ClientContext::AsyncPullServiceDefinitionAndImports2, shared_from_this(), _1, _2, boost::make_tuple(std::string(servicetype), root, defs, s, e, handler, timeout_time)), boost::numeric_cast<uint32_t>((timeout_time - GetNode()->NowUTC()).total_milliseconds()));
						return;
					}
					s++;
				}


				
				detail::InvokeHandler(node, handler, defs);
				return;				

			}			
			catch (std::exception& err)
			{
				detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ConnectionError);
			}
		}
	}



	std::vector<RR_SHARED_PTR<ServiceDefinition> > ClientContext::PullServiceDefinitionAndImports(const std::string &servicetype)
	{

		RR_SHARED_PTR<detail::sync_async_handler<std::vector<RR_SHARED_PTR<ServiceDefinition> > > > d = RR_MAKE_SHARED<detail::sync_async_handler<std::vector<RR_SHARED_PTR<ServiceDefinition> > > >(RR_MAKE_SHARED<ServiceException>("Could not pull service definition"));
		AsyncPullServiceDefinitionAndImports(std::string(servicetype), boost::bind(&detail::sync_async_handler<std::vector<RR_SHARED_PTR<ServiceDefinition> > >::operator(), d, _1, _2), GetNode()->GetRequestTimeout());
		return *d->end();		
	}

	std::vector<std::string> ClientContext::GetPulledServiceTypes()
	{
		boost::mutex::scoped_lock lock(pulled_service_types_lock);

		std::vector<std::string> o;
		boost::copy(pulled_service_types | boost::adaptors::map_keys, std::back_inserter(o));

		return o;
	}

	RR_SHARED_PTR<ServiceFactory> ClientContext::GetPulledServiceType(const std::string& type)
	{
		boost::mutex::scoped_lock lock(pulled_service_types_lock);
		RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceFactory> >::iterator e1 = pulled_service_types.find(type);
		if (e1 == pulled_service_types.end())
		{
			throw ServiceException("Unknown service type");
		}
		return e1->second;
	}


	const bool ClientContext::GetUserAuthenticated()
	{
		boost::mutex::scoped_lock lock(m_Authentication_lock);
		return m_UserAuthenticated;
	}

	const std::string ClientContext::GetAuthenticatedUsername()
	{
		boost::mutex::scoped_lock lock(m_Authentication_lock);
		return m_AuthenticatedUsername;
	}

	std::string ClientContext::AuthenticateUser(const std::string &username, RR_INTRUSIVE_PTR<RRValue> credentials)
	{
		RR_SHARED_PTR<detail::sync_async_handler<std::string> > d = RR_MAKE_SHARED<detail::sync_async_handler<std::string> >(RR_MAKE_SHARED<AuthenticationException>("Timed out"));
		AsyncAuthenticateUser(username, credentials, boost::bind(&detail::sync_async_handler<std::string>::operator(), d, _1, _2));
		return *d->end();
	}

	void ClientContext::AsyncAuthenticateUser(const std::string &username, RR_INTRUSIVE_PTR<RRValue> credentials, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{

		boost::mutex::scoped_lock lock(m_Authentication_lock);
		if (!GetConnected()) throw ConnectionException("Service client not connected");

		RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ClientSessionOpReq, "AuthenticateUser");
		m->ServicePath = GetServiceName();
		m->AddElement("username", stringToRRArray(username));
		if (dynamic_cast<RRMap<std::string, RRValue>*>(credentials.get()) != 0)
		{
			m->AddElement("credentials", GetNode()->PackMapType<std::string, RRValue>(credentials));
		}
		else if (dynamic_cast<MessageElement*>(credentials.get()) != 0)
		{
			RR_INTRUSIVE_PTR<MessageElement> mcredentials = rr_cast<MessageElement>(credentials);
			mcredentials->ElementName = "credentials";
			m->AddElement(mcredentials);
		}
		AsyncProcessRequest(m, boost::bind(&ClientContext::AsyncAuthenticateUser2, shared_from_this(), _1, _2, std::string(username), handler), timeout);

	}

	void ClientContext::AsyncAuthenticateUser2(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, const std::string& username, boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		if (err)
		{
			detail::InvokeHandlerWithException(node, handler, err);
			return;
		}

		m_AuthenticatedUsername = username;
		m_UserAuthenticated = true;
		std::string res = ret->FindElement("return")->CastDataToString();
		RR_SHARED_PTR<std::string> res2 = RR_MAKE_SHARED<std::string>(res);
		detail::InvokeHandler(node, handler, res2);

	}

	std::string ClientContext::LogoutUser()
	{
		boost::mutex::scoped_lock lock(m_Authentication_lock);
		if (!GetUserAuthenticated())
			throw InvalidOperationException("User is not authenticated");

		m_UserAuthenticated = false;
		m_AuthenticatedUsername = "";

		RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ClientSessionOpReq, "LogoutUser");
		m->ServicePath = GetServiceName();
		m->AddElement("username", stringToRRArray(GetAuthenticatedUsername()));
		RR_INTRUSIVE_PTR<MessageEntry> ret = ProcessRequest(m);
		return ret->FindElement("return")->CastDataToString();

	}

	std::string ClientContext::RequestObjectLock(RR_SHARED_PTR<RRObject> obj, RobotRaconteurObjectLockFlags flags)
	{
		RR_SHARED_PTR<detail::sync_async_handler<std::string> > t = RR_MAKE_SHARED<detail::sync_async_handler<std::string> >();
		AsyncRequestObjectLock(obj, flags, boost::bind(&detail::sync_async_handler<std::string>::operator(), t, _1, _2), GetNode()->GetRequestTimeout());
		return *t->end();
	}

	void ClientContext::AsyncRequestObjectLock(RR_SHARED_PTR<RRObject> obj, RobotRaconteurObjectLockFlags flags, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		if (!GetConnected()) throw ConnectionException("Service client not connected");


		if (!(dynamic_cast<ServiceStub*>(obj.get()) != 0))
			throw InvalidArgumentException("Can only lock object opened through Robot Raconteur");
		RR_SHARED_PTR<ServiceStub> s = rr_cast<ServiceStub>(obj);

		std::string command = "";
		if (flags == RobotRaconteurObjectLockFlags_USER_LOCK)
		{
			command = "RequestObjectLock";
		}
		else if (flags == RobotRaconteurObjectLockFlags_CLIENT_LOCK)
		{
			command = "RequestClientObjectLock";
		}
		else
			throw InvalidArgumentException("Unknown flags");

		RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ClientSessionOpReq, command);
		m->ServicePath = s->ServicePath;

		AsyncProcessRequest(m, boost::bind(&ClientContext::EndAsyncLockOp, shared_from_this(), _1, _2, handler), timeout);


	}

	void ClientContext::EndAsyncLockOp(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		if (err)
		{
			detail::InvokeHandlerWithException(node, handler, err);
			return;
		}
		try
		{
			std::string rets = ret->FindElement("return")->CastDataToString();
			RR_SHARED_PTR<std::string> rets2 = RR_MAKE_SHARED<std::string>(rets);
			detail::InvokeHandler(node, handler, rets2);
			
		}		
		catch (std::exception& err)
		{
			detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
		}
	}

	std::string ClientContext::ReleaseObjectLock(RR_SHARED_PTR<RRObject> obj)
	{
		RR_SHARED_PTR<detail::sync_async_handler<std::string> > t = RR_MAKE_SHARED<detail::sync_async_handler<std::string> >();
		AsyncReleaseObjectLock(obj, boost::bind(&detail::sync_async_handler<std::string>::operator(), t, _1, _2), GetNode()->GetRequestTimeout());
		return *t->end();
	}

	void ClientContext::AsyncReleaseObjectLock(RR_SHARED_PTR<RRObject> obj, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::string>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)
	{
		if (!GetConnected()) throw ConnectionException("Service client not connected");


		if (!(dynamic_cast<ServiceStub*>(obj.get()) != 0))
			throw InvalidArgumentException("Can only unlock object opened through Robot Raconteur");
		RR_SHARED_PTR<ServiceStub> s = rr_cast<ServiceStub>(obj);

		RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ClientSessionOpReq, "ReleaseObjectLock");
		m->ServicePath = s->ServicePath;

		AsyncProcessRequest(m, boost::bind(&ClientContext::EndAsyncLockOp, shared_from_this(), _1, _2, handler), timeout);
		
	}

	std::string ClientContext::MonitorEnter(RR_SHARED_PTR<RRObject> obj, int32_t timeout)
	{
		if (!GetConnected()) throw ConnectionException("Service client not connected");

		RR_SHARED_PTR<ServiceStub> stub2 = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
		if (!stub2)
			throw InvalidArgumentException("Can only unlock object opened through Robot Raconteur");

		bool iserror = true;

		{
			
			RR_SHARED_PTR<ServiceStub> s = rr_cast<ServiceStub>(obj);
			s->RRMutex->lock();

			bool keep_trying = true;
			RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ClientSessionOpReq, "MonitorEnter");
			m->ServicePath = s->ServicePath;
			m->AddElement("timeout", ScalarToRRArray<int32_t>(timeout));

			RR_INTRUSIVE_PTR<MessageEntry> ret = ProcessRequest(m);
			std::string retcode = ret->FindElement("return")->CastDataToString();

			if (retcode == "OK")
			{
				iserror = false;
				return "OK";

			}
			if (retcode == "Continue")
			{
				while (keep_trying)
				{
					RR_INTRUSIVE_PTR<MessageEntry> m1 = CreateMessageEntry(MessageEntryType_ClientSessionOpReq, "MonitorContinueEnter");
					m1->ServicePath = s->ServicePath;

					RR_INTRUSIVE_PTR<MessageEntry> ret1 = ProcessRequest(m1);
					std::string retcode1 = ret1->FindElement("return")->CastDataToString();
					if (retcode1 == "OK")
					{
						iserror = false;
						return "OK";
					}
					if (retcode1 != "Continue")
					{
						s->RRMutex->unlock();
						throw ProtocolException("Unknown return code");
					}
				}
			}
			else
			{
				s->RRMutex->unlock();
				throw ProtocolException("Unknown return code");
			}
			s->RRMutex->unlock();

		}



		throw ProtocolException("Unknown return code");
	}

	void ClientContext::MonitorExit(RR_SHARED_PTR<RRObject> obj)
	{
		if (!GetConnected()) throw ConnectionException("Service client not connected");
		RR_SHARED_PTR<ServiceStub> stub2 = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
		if (!stub2)
		{
			throw InvalidArgumentException("Can only unlock object opened through Robot Raconteur");
		}
		try
		{			
			RR_SHARED_PTR<ServiceStub> s = rr_cast<ServiceStub>(obj);

			boost::recursive_mutex::scoped_lock lock2(*s->RRMutex);

			RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ClientSessionOpReq, "MonitorExit");
			m->ServicePath = s->ServicePath;

			RR_INTRUSIVE_PTR<MessageEntry> ret = ProcessRequest(m);
			std::string retcode = ret->FindElement("return")->CastDataToString();
			if (retcode != "OK")
			{
				s->RRMutex->unlock();
				throw ProtocolException("Unknown return code");
			}
		}
		catch (std::exception&) {}
		try
		{			
			if (stub2)
			{
				stub2->RRMutex->unlock();
			}			
		}
		catch (std::exception&) {}
	}

	void ClientContext::PeriodicCleanupTask()
	{
		if (!GetConnected()) return;

		boost::posix_time::ptime t;
		{

			t = GetLastMessageReceivedTime();
		}

		if ((GetNode()->NowUTC() - t).total_milliseconds() > GetNode()->GetEndpointInactivityTimeout())
		{
			//This may result in a rare segfault so we can't automatically delete
			//Close();
			//Instead send a message to the listener that the connection has timed out
			try
			{
				ClientServiceListener(shared_from_this(), ClientServiceListenerEventType_ClientConnectionTimeout, RR_SHARED_PTR<RRObject>());
			}
			catch (std::exception& exp)
			{
				RobotRaconteurNode::TryHandleException(node, &exp);
			}
		}

		if (GetRemoteEndpoint() != 0)
		{
			if ((GetNode()->NowUTC() - GetLastMessageSentTime()).total_milliseconds() > 60000)
			{
				RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ClientKeepAliveReq, "");
				m->ServicePath = m_ServiceName;
				m->RequestID = 0;
				boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&rr_context_emptyhandler, _1);
				AsyncSendMessage(m, h);
			}
		}
	}

	uint32_t ClientContext::CheckServiceCapability(const std::string &name)
	{
		RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ServiceCheckCapabilityReq, name);
		m->ServicePath = m_ServiceName;
		RR_INTRUSIVE_PTR<MessageEntry> ret = ProcessRequest(m);
		uint32_t res = RRArrayToScalar(ret->FindElement("return")->CastData<RRArray<uint32_t> >());
		return res;
	}

	void ClientContext::ProcessCallbackCall(RR_INTRUSIVE_PTR<MessageEntry> m)
	{


		RR_INTRUSIVE_PTR<MessageEntry> ret;
		try
		{
			RR_SHARED_PTR<ServiceStub> stub;
			{
				boost::mutex::scoped_lock lock(stubs_lock);
				RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceStub> >::iterator e1 = stubs.find(m->ServicePath);
				if (e1 == stubs.end())
					throw ServiceException("Stub not found");
				stub = e1->second;
			}

			ret = stub->CallbackCall(m);
			if (!ret) throw InternalErrorException("Null pointer exception");
		}
		catch (std::exception& e)
		{
			ret = CreateMessageEntry((MessageEntryType(m->EntryType + 1)), m->MemberName);
			ret->ServicePath = m->ServicePath;
			ret->RequestID = m->RequestID;
			RobotRaconteurExceptionUtil::ExceptionToMessageEntry(e, ret);

		}

		boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&rr_context_node_handler, GetNode(), _1);
		AsyncSendMessage(ret, h);

	}

	bool ClientContext::UseMessage3()
	{
		bool f = false;
		use_message3.compare_exchange_strong(f, false);
		return f;
	}

	void ClientContext::InitializeInstanceFields()
	{

		request_number = 0;
		m_Connected = false;

		m_UserAuthenticated = false;
		use_pulled_types = false;
		//LastMessageSentTime = GetNode()->NowUTC();
		use_message3.store(false);
	}

	void ClientContext::TransportConnectionClosed(uint32_t endpoint)
	{
		if (endpoint != GetLocalEndpoint())
		{
			return;
		}

		try
		{
			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&ClientContext::TransportConnectionClosed1, shared_from_this()), true);
		}
		catch (std::exception&) {}
		
	}

	void ClientContext::TransportConnectionClosed1()
	{
		try
		{
			ClientServiceListener(shared_from_this(), ClientServiceListenerEventType_TransportConnectionClosed, RR_SHARED_PTR<RRObject>());
		}
		catch (std::exception& exp)
		{
			RobotRaconteurNode::TryHandleException(node, &exp);
		}
	}

}
