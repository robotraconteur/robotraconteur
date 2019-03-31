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

#include "LocalTransport_private.h"
#include "RobotRaconteur/Client.h"
#include "RobotRaconteur/Service.h"
#include "LocalTransport_discovery_private.h"

#include <boost/interprocess/sync/file_lock.hpp>

#include <boost/scope_exit.hpp>
#include <boost/algorithm/string.hpp>

#ifdef ROBOTRACONTEUR_WINDOWS
#include <Shlobj.h>
#else
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#endif 

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>

namespace RobotRaconteur
{

LocalTransport::LocalTransport(RR_SHARED_PTR<RobotRaconteurNode> node)
	: Transport(node)
{

	if (!node) throw InvalidArgumentException("Node cannot be null");
		
	transportopen=false;
	this->node=node;

	fds=RR_MAKE_SHARED<detail::LocalTransportFDs>();
#ifndef ROBOTRACONTEUR_DISABLE_MESSAGE3
	disable_message3 = false;
#else
	disable_message3 = true;
#endif
#ifndef ROBOTRACONTEUR_DISABLE_STRINGTABLE
	disable_string_table = false;
#else
	disable_string_table = true;
#endif
	disable_async_message_io = false;

	closed = false;



}

LocalTransport::~LocalTransport()
{

}


void LocalTransport::Close()
{
	{
		boost::mutex::scoped_lock lock(closed_lock);
		if (closed) return;
		closed = true;
	}

	try
	{
		boost::mutex::scoped_lock lock(acceptor_lock);
		if (acceptor) acceptor->close();
	}
	catch (std::exception&) {}

	std::vector<RR_SHARED_PTR<ITransportConnection> > t;

	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		boost::copy(TransportConnections | boost::adaptors::map_values, std::back_inserter(t));
	}

	BOOST_FOREACH(RR_SHARED_PTR<ITransportConnection>& e, t)
	{
		try
		{
			e->Close();
		}
		catch (std::exception&) {}
	}
	
	{
		boost::mutex::scoped_lock lock2(fds_lock);

		fds.reset();
		fds = RR_MAKE_SHARED<detail::LocalTransportFDs>();
	}

#ifndef ROBOTRACONTEUR_WINDOWS
	if (socket_file_name.size()>0)
	{
		unlink(socket_file_name.c_str());
	}
#endif

	DisableNodeDiscoveryListening();

	close_signal();

}

bool LocalTransport::IsServer() const
{
	return true;
}

bool LocalTransport::IsClient() const
{
	return true;
}

std::string LocalTransport::GetUrlSchemeString() const
{
	return "rr+local";
}

bool LocalTransport::CanConnectService(const std::string& url)
{
	if (boost::starts_with(url, "rr+local://"))
		return true;
	
	return false;
}

void LocalTransport::AsyncCreateTransportConnection(const std::string& url, RR_SHARED_PTR<Endpoint> ep, boost::function<void (RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException> ) >& callback)
{
	ParseConnectionURLResult url_res = ParseConnectionURL(url);

	if (url_res.nodename.empty() && url_res.nodeid.IsAnyNode())
	{
		throw ConnectionException("NodeID and/or NodeName must be specified for LocalTransport");
	}

	std::string my_username = detail::LocalTransportUtil::GetLogonUserName();
	
	boost::filesystem::path user_path=detail::LocalTransportUtil::GetTransportPrivateSocketPath();
	boost::optional<boost::filesystem::path> public_user_path=detail::LocalTransportUtil::GetTransportPublicSocketPath();
	boost::optional<boost::filesystem::path> public_search_path = detail::LocalTransportUtil::GetTransportPublicSearchPath();
	
	std::vector<boost::filesystem::path> search_paths;
	

	std::string host = url_res.host;

	if (url_res.port != -1) throw ConnectionException("Invalid url for local transport");
	if (url_res.path != "" && url_res.path != "/") throw ConnectionException("Invalid url for local transport");

	std::string username = "";

	std::vector<std::string> usernames;

	if (!boost::contains(host,"@"))
	{
		if (host!="localhost" && host!="") throw ConnectionException("Invalid host for local transport");
		search_paths.push_back(user_path);

		if (public_user_path)
		{
			search_paths.push_back(*public_user_path);
		}

		usernames.push_back(detail::LocalTransportUtil::GetLogonUserName());

		if (public_search_path)
		{
#ifdef ROBOTRACONTEUR_WINDOWS		
			const char* service_username = "LocalService";
#else
			const char* service_username = "root";
#endif
			boost::filesystem::path service_path = *public_search_path / service_username;
			if (boost::filesystem::is_directory(service_path))
			{
				search_paths.push_back(service_path);
			}

			usernames.push_back(service_username);
		}
	}
	else
	{
		std::vector<std::string> v1;
		boost::split(v1,host,boost::is_from_range('@','@'));
		if (v1.size()!=2) throw ConnectionException("Malformed URL");
		if (v1.at(1)!="localhost") throw ConnectionException("Invalid host for local transport");

		username=boost::trim_copy(v1.at(0));

		if(!boost::regex_match(username,boost::regex("^[a-zA-Z][a-zA-Z0-9_\\-]*$")))
		{
			throw ConnectionException("\"" + username + "\" is an invalid username");
		}

		if (username == my_username)
		{
			search_paths.push_back(user_path);
			if (public_user_path)
			{
				search_paths.push_back(*public_user_path);
			}
		}
		else
		{
			if (public_search_path)
			{
				boost::filesystem::path service_path = *public_search_path / username;
				if (boost::filesystem::is_directory(service_path))
				{
					search_paths.push_back(service_path);
				}
			}
		}
		usernames.push_back(username);
	}

	//TODO: test this
	RR_SHARED_PTR <LocalTransport::socket_type> socket = detail::LocalTransportUtil::FindAndConnectLocalSocket(url_res, search_paths, usernames, GetNode()->GetThreadPool()->get_io_service());
	if (!socket) throw ConnectionException("Could not connect to service");

	std::string noden;
	if (!(url_res.nodeid.IsAnyNode() && url_res.nodename != ""))
	{
		noden = url_res.nodeid.ToString();
	}
	else
	{
		noden = url_res.nodename;
	}

	boost::function<void(RR_SHARED_PTR<LocalTransport::socket_type>, RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> h 
		= boost::bind(&LocalTransport::AsyncCreateTransportConnection2, shared_from_this(), _1, noden, _2, _3, callback);
	LocalTransport_attach_transport(shared_from_this(), socket, false, ep->GetLocalEndpoint(), noden, h);
}   

void LocalTransport::AsyncCreateTransportConnection2(RR_SHARED_PTR<LocalTransport::socket_type> socket , const std::string& noden, RR_SHARED_PTR<ITransportConnection> transport, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void (RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException> ) >& callback)
{
	if (err)
	{
		try
		{
			callback(RR_SHARED_PTR<ITransportConnection>(),err);
			return;
		}
		catch (std::exception& err2)
		{
			RobotRaconteurNode::TryHandleException(node, &err2);
			return;
		}
	}

	register_transport(transport);

	callback(transport,RR_SHARED_PTR<RobotRaconteurException>());


}

RR_SHARED_PTR<ITransportConnection> LocalTransport::CreateTransportConnection(const std::string& url, RR_SHARED_PTR<Endpoint> e)
{
	RR_SHARED_PTR<detail::sync_async_handler<ITransportConnection> > d=RR_MAKE_SHARED<detail::sync_async_handler<ITransportConnection> >(RR_MAKE_SHARED<ConnectionException>("Timeout exception"));
	
	boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > h = boost::bind(&detail::sync_async_handler<ITransportConnection>::operator(), d, _1, _2);
	AsyncCreateTransportConnection(url,e,h);

	return d->end();

}

void LocalTransport::CloseTransportConnection(RR_SHARED_PTR<Endpoint> e)
{	
	RR_SHARED_PTR<ServerEndpoint> e2=boost::dynamic_pointer_cast<ServerEndpoint>(e);
	if (e2)
	{
		RR_SHARED_PTR<boost::asio::deadline_timer> timer=RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));
		timer->expires_from_now(boost::posix_time::milliseconds(1000));
		RobotRaconteurNode::asio_async_wait(node, timer, boost::bind(&LocalTransport::CloseTransportConnection_timed, shared_from_this(),boost::asio::placeholders::error,e,timer));
		return;
	}


	RR_SHARED_PTR<ITransportConnection> t;
	
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(e->GetLocalEndpoint());
		if (e1 == TransportConnections.end()) return;
		t = e1->second;
		TransportConnections.erase(e1);
	}
	
	if (t)
	{
		try
		{
			t->Close();
		}
		catch (std::exception&) {}
	}
	
}

void LocalTransport::CloseTransportConnection_timed(const boost::system::error_code& err, RR_SHARED_PTR<Endpoint> e,RR_SHARED_PTR<void> timer)
{
	if (err) return;

	RR_SHARED_PTR<ITransportConnection> t;
	
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(e->GetLocalEndpoint());
		if (e1 == TransportConnections.end()) return;
		t = e1->second;
	}
	
	if (t)
	{
		try
		{
			t->Close();
		}
		catch (std::exception&) {}
	}
}

/*void LocalTransport::StartServer(const std::string& name)
{
	boost::mutex::scoped_lock lock(acceptor_lock);

	if (acceptor) throw InvalidOperationException("Server already running");
#ifdef ROBOTRACONTEUR_WINDOWS
	acceptor=RR_MAKE_SHARED<socket_acceptor_type>("\\\\.\\pipe\\RobotRaconteur_" + name,20,GetNode());
	acceptor->listen();

	acceptor->async_accept(boost::bind(&LocalTransport::handle_accept,shared_from_this(),acceptor,_2,boost::asio::placeholders::error));
#else
	std::string fname="/tmp/RobotRaconteur_" + name;
	RR_SHARED_PTR<LocalTransport::socket_type> socket=RR_MAKE_SHARED<LocalTransport::socket_type>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));
	boost::asio::local::stream_protocol::endpoint ep(fname);
	acceptor=RR_MAKE_SHARED<socket_acceptor_type>(boost::ref(GetNode()->GetThreadPool()->get_io_service()),ep);
	acceptor->listen();
	acceptor->async_accept(*socket,boost::bind(&LocalTransport::handle_accept,shared_from_this(),acceptor,socket,boost::asio::placeholders::error));
#endif

}*/

void LocalTransport::StartClientAsNodeName(const std::string& name)
{
	if (!boost::regex_match(name, boost::regex("^[a-zA-Z][a-zA-Z0-9_\\.\\-]*$")))
	{
		throw InvalidArgumentException("\"" + name + "\" is an invalid NodeName");
	}

	boost::mutex::scoped_lock lock(acceptor_lock);
		
	boost::tuple<NodeID, RR_SHARED_PTR<detail::LocalTransportFD> > p = detail::LocalTransportUtil::GetNodeIDForNodeNameAndLock(name);

	try
	{
		GetNode()->SetNodeID(p.get<0>());
	}
	catch (std::exception&)
	{
		if (GetNode()->NodeID() != p.get<0>())
			throw;
	}

	{
		boost::mutex::scoped_lock lock(fds_lock);
		fds->h_nodename_s = p.get<1>();		
	}
}


void LocalTransport::StartServerAsNodeName(const std::string& name, bool public_)
{
	boost::mutex::scoped_lock lock(acceptor_lock);

	RR_SHARED_PTR<detail::LocalTransportNodeNameLock> nodename_lock = detail::LocalTransportNodeNameLock::Lock(name);
	if (!nodename_lock) throw NodeNameAlreadyInUse();

	boost::tuple<NodeID, RR_SHARED_PTR<detail::LocalTransportFD> > nodeid1 = detail::LocalTransportUtil::GetNodeIDForNodeNameAndLock(name);
	NodeID& nodeid = nodeid1.get<0>();

	RR_SHARED_PTR<detail::LocalTransportNodeIDLock> nodeid_lock = detail::LocalTransportNodeIDLock::Lock(nodeid);
	if (!nodeid_lock) throw NodeIDAlreadyInUse();
	
	int32_t tries=0;

	boost::filesystem::path socket_path;
	
	if (!public_)
	{
		socket_path = detail::LocalTransportUtil::GetTransportPrivateSocketPath();
	}
	else
	{
		boost::optional<boost::filesystem::path> socket_path1 = detail::LocalTransportUtil::GetTransportPublicSocketPath();
		if (!socket_path1) throw ConnectionException("Computer not initialized for public node server");
		socket_path = *socket_path1;
	}
	std::string pipename;
		
	while (true)
	{
#ifdef ROBOTRACONTEUR_WINDOWS
		pipename="\\\\.\\pipe\\RobotRaconteur-";
		pipename += GetNode()->GetRandomString(32);
		
#elif defined(ROBOTRACONTEUR_APPLE)
		pipename=socket_path.string() + "/socket/";
		pipename += GetNode()->GetRandomString(16);
#else

		pipename=socket_path.string() + "/socket/RobotRaconteur-";
		pipename += GetNode()->GetRandomString(16) + ".sock";
#endif


		try
		{
			#ifdef ROBOTRACONTEUR_WINDOWS
				acceptor=RR_MAKE_SHARED<socket_acceptor_type>(pipename.c_str(),20,public_,GetNode());
				acceptor->listen();

				
			#else

				RR_SHARED_PTR<LocalTransport::socket_type> socket=RR_MAKE_SHARED<LocalTransport::socket_type>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));
				boost::asio::local::stream_protocol::endpoint ep(pipename);
				acceptor=RR_MAKE_SHARED<socket_acceptor_type>(boost::ref(GetNode()->GetThreadPool()->get_io_service()),ep);
				acceptor->listen();
				chmod(pipename.c_str(),S_IRWXU | S_IRWXG);
				
			#endif

			break;
		}
		catch (std::exception&)
		{
			tries++;
			if (tries > 3)
				throw;			
		}
	}
	
	boost::filesystem::path pid_id_fname=socket_path / "by-nodeid" / (nodeid.ToString("D") + ".pid");
	boost::filesystem::path info_id_fname=socket_path / "by-nodeid" / (nodeid.ToString("D") + ".info");
	boost::filesystem::path pid_name_fname=socket_path / "by-nodename" / (name + ".pid");
	boost::filesystem::path info_name_fname = socket_path / "by-nodename" / (name + ".info");

	std::map<std::string, std::string> info;
	info.insert(std::make_pair("nodename", name));
	info.insert(std::make_pair("nodeid", nodeid.ToString()));
	info.insert(std::make_pair("socket", pipename));
	info.insert(std::make_pair("ServiceStateNonce", GetNode()->GetServiceStateNonce()));
	
	RR_SHARED_PTR<detail::LocalTransportFD> h_pid_id_s=detail::LocalTransportUtil::CreatePidFile(pid_id_fname);
	RR_SHARED_PTR<detail::LocalTransportFD> h_pid_name_s = detail::LocalTransportUtil::CreatePidFile(pid_name_fname, true);
	RR_SHARED_PTR<detail::LocalTransportFD> h_info_id_s = detail::LocalTransportUtil::CreateInfoFile(info_id_fname, info);
	RR_SHARED_PTR<detail::LocalTransportFD> h_info_name_s = detail::LocalTransportUtil::CreateInfoFile(info_name_fname, info, true);

	try
	{
		GetNode()->SetNodeID(nodeid);
	}
	catch (std::exception&)
	{
		if (GetNode()->NodeID()!=nodeid)
			throw;
	}

	try
	{
		GetNode()->SetNodeName(name);
	}
	catch (std::exception&)
	{
		if (GetNode()->NodeName()!=name)
			throw;
	}
#ifdef ROBOTRACONTEUR_WINDOWS
	acceptor->async_accept(boost::bind(&LocalTransport::handle_accept,shared_from_this(),acceptor,_2,boost::asio::placeholders::error));
#else
	RR_SHARED_PTR<LocalTransport::socket_type> socket=RR_MAKE_SHARED<LocalTransport::socket_type>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));
	acceptor->async_accept(*socket,boost::bind(&LocalTransport::handle_accept,shared_from_this(),acceptor,socket,boost::asio::placeholders::error));
#endif

	{
		boost::mutex::scoped_lock lock(fds_lock);
		fds->h_nodename_s = nodeid1.get<1>();
		fds->h_pid_id_s=h_pid_id_s;
		fds->h_info_id_s=h_info_id_s;
		fds->h_pid_name_s=h_pid_name_s;
		fds->h_info_name_s=h_info_name_s;
		fds->nodeid_lock = nodeid_lock;
		fds->nodename_lock = nodename_lock;
	}

	socket_file_name=pipename;
}

void LocalTransport::StartServerAsNodeID(const NodeID& nodeid1, bool public_)
{
	NodeID nodeid=nodeid1;
	if (nodeid.IsAnyNode()) throw InvalidArgumentException("NodeID must not be zero node");

	boost::mutex::scoped_lock lock(acceptor_lock);
	
	RR_SHARED_PTR<detail::LocalTransportNodeIDLock> nodeid_lock = detail::LocalTransportNodeIDLock::Lock(nodeid);
	if (!nodeid_lock) throw NodeIDAlreadyInUse();

	int32_t tries=0;

	std::string pipename;

	boost::filesystem::path socket_path;

	if (!public_)
	{
		socket_path = detail::LocalTransportUtil::GetTransportPrivateSocketPath();
	}
	else
	{
		boost::optional<boost::filesystem::path> socket_path1 = detail::LocalTransportUtil::GetTransportPublicSocketPath();
		if (!socket_path1) throw ConnectionException("Computer not initialized for public node server");
		socket_path = *socket_path1;
	}

	while (true)
	{
#ifdef ROBOTRACONTEUR_WINDOWS
		pipename="\\\\.\\pipe\\RobotRaconteur-";
		pipename += GetNode()->GetRandomString(32);
#else
		pipename=socket_path.string() + "/socket/RobotRaconteur-";
		pipename += GetNode()->GetRandomString(32) + ".sock";
#endif
		
		


		try
		{
			#ifdef ROBOTRACONTEUR_WINDOWS
				acceptor=RR_MAKE_SHARED<socket_acceptor_type>(pipename.c_str(),20,public_,GetNode());
				acceptor->listen();

				
			#else

				RR_SHARED_PTR<LocalTransport::socket_type> socket=RR_MAKE_SHARED<LocalTransport::socket_type>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));
				boost::asio::local::stream_protocol::endpoint ep(pipename);
				acceptor=RR_MAKE_SHARED<socket_acceptor_type>(boost::ref(GetNode()->GetThreadPool()->get_io_service()),ep);
				acceptor->listen();
				chmod(pipename.c_str(),S_IRWXU | S_IRWXG);
				
			#endif

			break;
		}
		catch (std::exception&)
		{
			tries++;
			if (tries > 3)
				throw;			
		}
	}

	boost::filesystem::path pid_id_fname = socket_path / "by-nodeid" / (nodeid.ToString("D") + ".pid");
	boost::filesystem::path info_id_fname = socket_path / "by-nodeid" / (nodeid.ToString("D") + ".info");

	std::map<std::string, std::string> info;
	info.insert(std::make_pair("nodename", GetNode()->NodeName()));
	info.insert(std::make_pair("nodeid", nodeid.ToString()));
	info.insert(std::make_pair("socket", pipename));
	info.insert(std::make_pair("ServiceStateNonce", GetNode()->GetServiceStateNonce()));
	
	RR_SHARED_PTR<detail::LocalTransportFD> h_pid_id_s = detail::LocalTransportUtil::CreatePidFile(pid_id_fname);
	RR_SHARED_PTR<detail::LocalTransportFD> h_info_id_s = detail::LocalTransportUtil::CreateInfoFile(info_id_fname, info);

	try
	{
		GetNode()->SetNodeID(nodeid);
	}
	catch (std::exception&)
	{
		if (GetNode()->NodeID()!=nodeid)
			throw;
	}
	
#ifdef ROBOTRACONTEUR_WINDOWS
	acceptor->async_accept(boost::bind(&LocalTransport::handle_accept,shared_from_this(),acceptor,_2,boost::asio::placeholders::error));
#else
	RR_SHARED_PTR<LocalTransport::socket_type> socket=RR_MAKE_SHARED<LocalTransport::socket_type>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));
	acceptor->async_accept(*socket,boost::bind(&LocalTransport::handle_accept,shared_from_this(),acceptor,socket,boost::asio::placeholders::error));
#endif

	{
		boost::mutex::scoped_lock lock(fds_lock);
		fds->h_pid_id_s=h_pid_id_s;
		fds->h_info_id_s=h_info_id_s;
		fds->nodeid_lock = nodeid_lock;
		
	}

	socket_file_name=pipename;
}

void LocalTransport::SendMessage(RR_INTRUSIVE_PTR<Message> m)
{
	

	RR_SHARED_PTR<ITransportConnection> t;
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(m->header->SenderEndpoint);
		if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e1->second;
	}
	t->SendMessage(m);
}

uint32_t LocalTransport::TransportCapability(const std::string& name)
{
	return 0;
}

void LocalTransport::PeriodicCleanupTask()
{
	boost::mutex::scoped_lock lock(TransportConnections_lock);
	for (RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e=TransportConnections.begin(); e!=TransportConnections.end(); )
	{
		try
		{
			RR_SHARED_PTR<LocalTransportConnection> e2=rr_cast<LocalTransportConnection>(e->second);
			if (!e2->IsConnected())
			{
				e=TransportConnections.erase(e);
			}
			else
			{
				e++;
			}
		}
		catch (std::exception&) {}
	}
}

void LocalTransport::AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )>& handler)
{
	RR_SHARED_PTR<ITransportConnection> t;
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(m->header->SenderEndpoint);
		if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e1->second;
	}

	t->AsyncSendMessage(m,handler);
}

void LocalTransport::handle_accept(RR_SHARED_PTR<LocalTransport> parent,RR_SHARED_PTR<socket_acceptor_type> acceptor, RR_SHARED_PTR<socket_type> socket, const boost::system::error_code& error)
{
	if (error) 
		return;

	try
	{		
		boost::function<void(RR_SHARED_PTR<LocalTransport::socket_type>, RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> h
			= boost::bind(&LocalTransport_connected_callback2, parent, _1, _2, _3);
		LocalTransport_attach_transport(parent,socket,true,0,"{0}",h);
	}
	catch (std::exception& exp) 
	{
		RobotRaconteurNode::TryHandleException(parent->node, &exp);
	}

	boost::mutex::scoped_lock lock(parent->acceptor_lock);
	
#ifdef ROBOTRACONTEUR_WINDOWS
	acceptor->async_accept(boost::bind(&LocalTransport::handle_accept,parent,acceptor,_2,boost::asio::placeholders::error));
#else
	RR_SHARED_PTR<socket_type> socket2=RR_MAKE_SHARED<socket_type>(boost::ref(parent->GetNode()->GetThreadPool()->get_io_service()));
	acceptor->async_accept(*socket2,boost::bind(&LocalTransport::handle_accept,parent,acceptor,socket2,boost::asio::placeholders::error));
#endif


}

void LocalTransport::register_transport(RR_SHARED_PTR<ITransportConnection> connection)
{
	boost::mutex::scoped_lock lock(TransportConnections_lock);
	TransportConnections.insert(std::make_pair(connection->GetLocalEndpoint(),connection));
}

void LocalTransport::erase_transport(RR_SHARED_PTR<ITransportConnection> connection)
{
	try
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);		
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(connection->GetLocalEndpoint());
		if (e1 == TransportConnections.end()) return;
		if (e1->second == connection)
		{
			TransportConnections.erase(e1);
		}
	}
	catch (std::exception&) {}

	TransportConnectionClosed(connection->GetLocalEndpoint());

}


void LocalTransport::MessageReceived(RR_INTRUSIVE_PTR<Message> m)
{
	GetNode()->MessageReceived(m);
}

void LocalTransport::AsyncGetDetectedNodes(const std::vector<std::string>& schemes, boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)>& handler, int32_t timeout)
{ 
	if (boost::range::find(schemes, "rr+local") == schemes.end() || schemes.size() == 0)
	{
		RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > n = RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >();
		detail::PostHandler(node, handler, n, true);
		return;
	}


	RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > o=RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >();

	boost::filesystem::path private_search_dir=detail::LocalTransportUtil::GetTransportPrivateSocketPath();

	std::string my_username = detail::LocalTransportUtil::GetLogonUserName();
	detail::LocalTransportUtil::FindNodesInDirectory(*o, private_search_dir, "rr+local", GetNode()->NowUTC(), my_username);

	//TODO: search other users
	boost::optional<boost::filesystem::path> search_path = detail::LocalTransportUtil::GetTransportPublicSearchPath();
	if (search_path)
	{
		try
		{
			boost::filesystem::directory_iterator end_iter;
			for (boost::filesystem::directory_iterator dir_itr(*search_path); dir_itr != end_iter; dir_itr++)
			{
				try
				{
					if (!boost::filesystem::is_directory(*dir_itr))
					{
						continue;
					}

					boost::filesystem::path path1 = dir_itr->path();

					std::string username1 = path1.filename().string();
					std::vector<NodeDiscoveryInfo> o1;
					detail::LocalTransportUtil::FindNodesInDirectory(o1, *dir_itr, "rr+local", GetNode()->NowUTC(), username1);
					boost::range::copy(o1, std::back_inserter(*o));
				}
				catch (std::exception&) {}
			}
		}
		catch (std::exception&) {}
	}


	detail::PostHandler(node, handler, o, true);

}

bool LocalTransport::GetDisableMessage3()
{
	boost::mutex::scoped_lock lock(parameter_lock);
	return disable_message3;
}
void LocalTransport::SetDisableMessage3(bool d)
{
	boost::mutex::scoped_lock lock(parameter_lock);
	disable_message3 = d;
}

bool LocalTransport::GetDisableStringTable()
{
	boost::mutex::scoped_lock lock(parameter_lock);
	return disable_string_table;
}
void LocalTransport::SetDisableStringTable(bool d)
{
	boost::mutex::scoped_lock lock(parameter_lock);
	disable_string_table = d;
}

bool LocalTransport::GetDisableAsyncMessageIO()
{
	boost::mutex::scoped_lock lock(parameter_lock);
	return disable_async_message_io;
}
void LocalTransport::SetDisableAsyncMessageIO(bool d)
{
	boost::mutex::scoped_lock lock(parameter_lock);
	disable_async_message_io = d;
}

void LocalTransport::EnableNodeDiscoveryListening()
{
	boost::mutex::scoped_lock lock(discovery_lock);

	if (discovery)
	{
		throw InvalidOperationException("LocalTransport discovery already running");
	}

#ifdef ROBOTRACONTEUR_WINDOWS
	discovery = RR_MAKE_SHARED<detail::WinLocalTransportDiscovery>(GetNode());
	discovery->Init();
	try { discovery->Refresh(); } catch (std::exception&) {}
#endif

#ifdef ROBOTRACONTEUR_LINUX
	discovery = RR_MAKE_SHARED<detail::LinuxLocalTransportDiscovery>(GetNode());
	discovery->Init();
	try { discovery->Refresh(); } catch (std::exception&) {}
#endif

#ifdef ROBOTRACONTEUR_OSX
	discovery = RR_MAKE_SHARED<detail::DarwinLocalTransportDiscovery>(GetNode());
	discovery->Init();
	try { discovery->Refresh(); } catch (std::exception&) {}
#endif
}

void LocalTransport::DisableNodeDiscoveryListening()
{
	boost::mutex::scoped_lock lock(discovery_lock);
	if (discovery)
	{
		discovery->Shutdown();
	}

	discovery.reset();
}

void LocalTransport::LocalNodeServicesChanged()
{
	boost::mutex::scoped_lock lock(fds_lock);
	if (fds && fds->h_info_id_s)
	{
		std::string service_nonce = GetNode()->GetServiceStateNonce();
		
		detail::LocalTransportUtil::RefreshInfoFile(fds->h_info_id_s, service_nonce);
		if (fds->h_info_name_s)
		{
			detail::LocalTransportUtil::RefreshInfoFile(fds->h_info_name_s, service_nonce);
		}
	}
}

LocalTransportConnection::LocalTransportConnection(RR_SHARED_PTR<LocalTransport> parent, bool server, uint32_t local_endpoint) : ASIOStreamBaseTransport(parent->GetNode())
{
	this->parent=parent;
	this->server=server;
	this->m_LocalEndpoint=local_endpoint;
	this->m_RemoteEndpoint=0;

	//The heartbeat isn't as necessary in local connections because the socket
	//will be closed immediately if the other side closes or is terminated
	this->HeartbeatPeriod=30000;
	this->ReceiveTimeout=600000;

	this->disable_message3 = parent->GetDisableMessage3();
	this->disable_string_table = parent->GetDisableStringTable();
	this->disable_async_io = parent->GetDisableAsyncMessageIO();
}


void LocalTransportConnection::AsyncAttachSocket(RR_SHARED_PTR<LocalTransport::socket_type> socket, std::string noden, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	this->socket=socket;
	
	
	NodeID target_nodeid=NodeID::GetAny();
	std::string target_nodename;
	if (noden.find("{")!=std::string::npos)
	{
		target_nodeid=NodeID(noden);
	}
	else
	{
		target_nodename=noden;
	}

	ASIOStreamBaseTransport::AsyncAttachStream(server,target_nodeid, target_nodename, callback);
}

void LocalTransportConnection::MessageReceived(RR_INTRUSIVE_PTR<Message> m)
{

	RR_SHARED_PTR<LocalTransport> p=parent.lock();
	if (!p) return;

	RR_INTRUSIVE_PTR<Message> ret = p->SpecialRequest(m, shared_from_this());
	if (ret != 0)
	{
		try
		{
			if ((m->entries.at(0)->EntryType == MessageEntryType_ConnectionTest || m->entries.at(0)->EntryType == MessageEntryType_ConnectionTestRet))
			{
				if (m->entries.at(0)->Error != MessageErrorType_None)
				{
					Close();
					return;
				}
			}

			if ((ret->entries.at(0)->EntryType == MessageEntryType_ConnectClientRet || ret->entries.at(0)->EntryType == MessageEntryType_ReconnectClient || ret->entries.at(0)->EntryType == MessageEntryType_ConnectClientCombinedRet) && ret->entries.at(0)->Error == MessageErrorType_None)
			{
				if (ret->entries.at(0)->Error == MessageErrorType_None)
				{
					if (ret->header->SenderNodeID == GetNode()->NodeID())
					{
						{
							boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
							if (m_LocalEndpoint != 0)
							{
								throw InvalidOperationException("Already connected");
							}

							m_RemoteEndpoint = ret->header->ReceiverEndpoint;
							m_LocalEndpoint = ret->header->SenderEndpoint;
						}


						p->register_transport(RR_STATIC_POINTER_CAST<LocalTransportConnection>(shared_from_this()));
					}
				}

			}
			
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&LocalTransportConnection::SimpleAsyncEndSendMessage, RR_STATIC_POINTER_CAST<LocalTransportConnection>(shared_from_this()), _1);
			AsyncSendMessage(ret, h);
		}
		catch (std::exception&)
		{
			Close();
		}

		return;
	}



	
	try
	{

		//TODO: fix this (maybe??)...

		/*boost::asio::ip::address addr=socket->local_endpoint().address();
		uint16_t port=socket->local_endpoint().port();

		std::string connecturl;
		if (addr.is_v4())
		{
			connecturl="local://" + addr.to_string() + ":" + boost::lexical_cast<std::string>(port) + "/";
		}
		else
		{
			boost::asio::ip::address_v6 addr2=addr.to_v6();
			addr2.scope_id(0);
			connecturl="tcp://[" + addr2.to_string() + "]:" + boost::lexical_cast<std::string>(port) + "/";
		}
		*/

		std::string connecturl="rr+local:///";
		Transport::m_CurrentThreadTransportConnectionURL.reset(new std::string(connecturl));
		Transport::m_CurrentThreadTransport.reset(  new RR_SHARED_PTR<ITransportConnection>(RR_STATIC_POINTER_CAST<LocalTransportConnection>(shared_from_this())));
		p->MessageReceived(m);
	}
	catch (std::exception& exp)
	{
		RobotRaconteurNode::TryHandleException(node, &exp);
		Close();
	}

		
	Transport::m_CurrentThreadTransportConnectionURL.reset(0);
	Transport::m_CurrentThreadTransport.reset(0);


	
}

void LocalTransportConnection::async_write_some(const_buffers& b, boost::function<void (const boost::system::error_code& error, size_t bytes_transferred)>& handler)
{
	boost::mutex::scoped_lock lock(socket_lock);
	RobotRaconteurNode::asio_async_write_some(node, socket, b,handler);
}


void LocalTransportConnection::async_read_some(mutable_buffers& b, boost::function<void (const boost::system::error_code& error, size_t bytes_transferred)>& handler)
{
	boost::mutex::scoped_lock lock(socket_lock);
	RobotRaconteurNode::asio_async_read_some(node, socket, b,handler);
}

size_t LocalTransportConnection::available()
{
#ifdef ROBOTRACONTEUR_WINDOWS
	boost::mutex::scoped_lock lock(socket_lock);
	DWORD lpTotalBytesAvail;
	::PeekNamedPipe(socket->native_handle(),NULL,NULL,NULL,&lpTotalBytesAvail,NULL);
	return (size_t)lpTotalBytesAvail;
#else
	return 0;
#endif
}

void LocalTransportConnection::Close()
{
	boost::recursive_mutex::scoped_lock lock(close_lock);
	
	{
		boost::mutex::scoped_lock lock(socket_lock);

#ifndef ROBOTRACONTEUR_WINDOWS
		try
		{
		if (socket->is_open())
		{
		socket->shutdown(LocalTransport::socket_type::shutdown_both);
		}
	
		}
		catch (std::exception&) {}
#endif
		try
		{
			socket->close();
		}
		catch (std::exception&) {}
	}

	try
	{
		RR_SHARED_PTR<LocalTransport> p=parent.lock();
		if (p) p->erase_transport(RR_STATIC_POINTER_CAST<LocalTransportConnection>(shared_from_this()));
	}
	catch (std::exception&) {}

	ASIOStreamBaseTransport::Close();

}

void LocalTransport::CheckConnection(uint32_t endpoint)
{
	RR_SHARED_PTR<ITransportConnection> t;
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e = TransportConnections.find(endpoint);
		if (e == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e->second;
	}
	t->CheckConnection(endpoint);
}

uint32_t LocalTransportConnection::GetLocalEndpoint() 
{
	return m_LocalEndpoint;
}

uint32_t LocalTransportConnection::GetRemoteEndpoint() 
{
	return m_RemoteEndpoint;
}

void LocalTransportConnection::CheckConnection(uint32_t endpoint)
{
	if (endpoint!=m_LocalEndpoint || !connected.load()) throw ConnectionException("Connection lost");
}

void LocalTransport_connected_callback2(RR_SHARED_PTR<LocalTransport> parent,RR_SHARED_PTR<LocalTransport::socket_type> socket, RR_SHARED_PTR<ITransportConnection> connection, RR_SHARED_PTR<RobotRaconteurException> err)
{
	//This is just an empty method.  The connected transport will register when it has a local endpoint.
	
}

void LocalTransport_attach_transport(RR_SHARED_PTR<LocalTransport> parent, RR_SHARED_PTR<LocalTransport::socket_type> socket,  bool server, uint32_t endpoint, std::string noden, boost::function<void( RR_SHARED_PTR<LocalTransport::socket_type> , RR_SHARED_PTR<ITransportConnection> , RR_SHARED_PTR<RobotRaconteurException> )>& callback)
{
	try
	{
		RR_SHARED_PTR<LocalTransportConnection> t=RR_MAKE_SHARED<LocalTransportConnection>(parent,server,endpoint);
		boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(callback, socket, t, _1);
		t->AsyncAttachSocket(socket,noden,h);
		parent->AddCloseListener(t, &LocalTransportConnection::Close);
	}
	catch (std::exception& )
	{
		RobotRaconteurNode::TryPostToThreadPool(parent->GetNode(), boost::bind(callback,RR_SHARED_PTR<LocalTransport::socket_type>(), RR_SHARED_PTR<LocalTransportConnection>(),RR_MAKE_SHARED<ConnectionException>("Could not connect to service")));
	}

}

namespace detail
{
	namespace LocalTransportUtil
	{
		std::string GetLogonUserName()
		{
#ifdef ROBOTRACONTEUR_WINDOWS
			DWORD size=0;
			GetUserName(NULL,&size);

			boost::scoped_array<char> n(new char[size]);

			BOOL res=GetUserName(n.get(),&size);
			if (!res) throw ConnectionException("Could not determine current user");

			return std::string(n.get());
#else
			struct passwd* passwd;
			passwd=getpwuid(getuid());
			if (passwd==NULL) throw ConnectionException("Could not determine current user");

			std::string o(passwd->pw_name);
			if (o.size()==0) throw ConnectionException("Could not determine current user");

			return o;

#endif
		}

		boost::filesystem::path GetUserDataPath()
		{
#ifdef ROBOTRACONTEUR_WINDOWS
			boost::scoped_array<char> sysdata_path1(new char[MAX_PATH]);
			if (FAILED(SHGetFolderPath(NULL,CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,NULL,0,sysdata_path1.get())))
			{
				throw SystemResourceException("Could not get system information");
			}

			boost::filesystem::path sysdata_path(sysdata_path1.get());
						
			boost::filesystem::path path = sysdata_path / "RobotRaconteur";
#else
			char* path1=std::getenv("HOME");
			if (!path1) throw SystemResourceException("Home directory not set");
			boost::filesystem::path path = boost::filesystem::path(path1) / ".config" / "RobotRaconteur";
#endif
			boost::system::error_code ec1;
			boost::filesystem::create_directories(path, ec1);
			if (ec1) throw SystemResourceException("Could not active system for local transport");

			return path;
		}

		boost::filesystem::path GetUserRunPath()
		{
#ifdef ROBOTRACONTEUR_WINDOWS
			boost::scoped_array<char> sysdata_path1(new char[MAX_PATH]);
			if (FAILED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, sysdata_path1.get())))
			{
				throw SystemResourceException("Could not get system information");
			}

			boost::filesystem::path sysdata_path(sysdata_path1.get());

			boost::filesystem::path path = sysdata_path / "RobotRaconteur" / "run";
			boost::system::error_code ec1;
			boost::filesystem::create_directories(path, ec1);
			if (ec1) throw SystemResourceException("Could not active system for local transport");
			
#elif defined(ROBOTRACONTEUR_APPLE)
			
			int u= getuid();
			
			boost::filesystem::path path;
			if (u==0)
			{
				path="/var/run/robotraconteur/root/";
				boost::filesystem::create_directories(path);
				chmod(path.c_str(),  S_IRUSR |  S_IWUSR |  S_IXUSR);
				//path /= "robotraconteur";
				boost::system::error_code ec1;
				boost::filesystem::create_directories(path, ec1);
				if (ec1) throw SystemResourceException("Could not active system for local transport");
				
			}
			else
			{
				char* path1 = std::getenv("TMPDIR");
				if (!path1) throw SystemResourceException("Could not active system for local transport");
				
				path = path1;
				path = path.remove_trailing_separator().parent_path();
				path /= "C";
				if (!boost::filesystem::is_directory(path)) throw SystemResourceException("Could not active system for local transport");
				path /= "robotraconteur";
				boost::system::error_code ec1;
				boost::filesystem::create_directories(path, ec1);
				if (ec1) throw SystemResourceException("Could not active system for local transport");
			}

#else
			int u= getuid();

			//TODO: fix mac version
			boost::filesystem::path path;
			if (u==0)
			{
				path="/var/run/robotraconteur/root/";
				boost::filesystem::create_directories(path);
				chmod(path.c_str(),  S_IRUSR |  S_IWUSR |  S_IXUSR);
				//path /= "robotraconteur";
				boost::system::error_code ec1;
				boost::filesystem::create_directories(path, ec1);
				if (ec1) throw SystemResourceException("Could not active system for local transport");

			}
			else
			{
				char* path1 = std::getenv("XDG_RUNTIME_DIR");
				if (path1!=NULL)
				{
					path = path1;
					path /= "robotraconteur";
					boost::system::error_code ec1;
					boost::filesystem::create_directories(path, ec1);
					if (ec1) throw SystemResourceException("Could not active system for local transport");
				}
				else
				{

					//path = boost::filesystem::path("/tmp/.robotraconteur-user-" + boost::lexical_cast<std::string>(u));

					path = boost::filesystem::path("/var/run/user") / boost::lexical_cast<std::string>(u) / "robotraconteur";
					boost::system::error_code ec1;
					boost::filesystem::create_directories(path, ec1);
					if (ec1) throw SystemResourceException("Could not active system for local transport: $XDG_RUNTIME_DIR not set");
					/*chmod(path.c_str(),  S_IRUSR |  S_IWUSR |  S_IXUSR);
					struct stat s;
					if (stat(path.c_str(), &s) < 0)
					{
						throw SystemResourceException("Could not active system for local transport: permission error");
					}

					if (s.st_uid != u)
					{
						throw SystemResourceException("Could not active system for local transport");
					}*/
				}


			}
#endif
			return path;
		}

		boost::filesystem::path GetUserNodeIDPath()
		{
			boost::filesystem::path path = GetUserDataPath() / "nodeids";
			boost::system::error_code ec1;
			boost::filesystem::create_directories(path, ec1);
			if (ec1) throw SystemResourceException("Could not active system for local transport");

			return path;

		}

		boost::filesystem::path GetTransportPrivateSocketPath()
		{			
			boost::filesystem::path path = GetUserRunPath() / "transport" / "local";
			
			boost::filesystem::path bynodeid_path = path / "by-nodeid";
			boost::filesystem::path bynodename_path = path / "by-nodename";
			boost::filesystem::path socket_path = path / "socket";

			boost::system::error_code ec1;
			boost::filesystem::create_directories(bynodeid_path, ec1);
			if (ec1) throw SystemResourceException("Could not active system for local transport");

			boost::system::error_code ec2;
			boost::filesystem::create_directories(bynodename_path, ec2);
			if (ec2) throw SystemResourceException("Could not active system for local transport");

			boost::system::error_code ec3;
			boost::filesystem::create_directories(socket_path, ec3);
			if (ec3) throw SystemResourceException("Could not active system for local transport");

			return path;
		}

		boost::optional<boost::filesystem::path> GetTransportPublicSocketPath()
		{
			boost::filesystem::path path1;
			if (boost::optional < boost::filesystem::path> path1_1 = GetTransportPublicSearchPath())
			{
				path1 = *path1_1;
			}
			else
			{
				return boost::optional<boost::filesystem::path>();
			}
						
			std::string username=GetLogonUserName();

			boost::filesystem::path path = path1 / username;

			if (!boost::filesystem::is_directory(path))
			{
				return boost::optional<boost::filesystem::path>();
			}

			boost::filesystem::path bynodeid_path = path / "by-nodeid";
			boost::filesystem::path bynodename_path = path / "by-nodename";
			boost::filesystem::path socket_path = path / "socket";

			boost::system::error_code ec1;
			boost::filesystem::create_directories(bynodeid_path, ec1);
			if (ec1) return boost::optional<boost::filesystem::path>();

			boost::system::error_code ec2;
			boost::filesystem::create_directories(bynodename_path, ec2);
			if (ec2) return boost::optional<boost::filesystem::path>();

			boost::system::error_code ec3;
			boost::filesystem::create_directories(socket_path, ec3);
			if (ec3) return boost::optional<boost::filesystem::path>();

#ifdef ROBOTRACONTEUR_WINDOWS
			//TODO: file permissions?
#else
			struct stat info;
			if (stat(path.string().c_str(), &info) < 0)
			{
				return boost::optional<boost::filesystem::path>();
			}

			uid_t my_uid=getuid();

			chmod(bynodeid_path.string().c_str(), S_ISGID |S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP);
			chown(bynodeid_path.string().c_str(), my_uid, info.st_gid);
			chmod(bynodename_path.string().c_str(), S_ISGID | S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP);
			chown(bynodename_path.string().c_str(), my_uid, info.st_gid);
			chmod(socket_path.string().c_str(), S_ISGID | S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP);
			chown(socket_path.string().c_str(), my_uid, info.st_gid);
#endif

			return path;
		}

		boost::optional<boost::filesystem::path> GetTransportPublicSearchPath()
		{
#ifdef ROBOTRACONTEUR_WINDOWS
			boost::scoped_array<wchar_t> sysdata_path1(new wchar_t[MAX_PATH]);
			if (FAILED(SHGetFolderPathW(NULL,CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE,NULL,0,sysdata_path1.get())))
			{
				return boost::optional<boost::filesystem::path>();
			}

			boost::filesystem::path sysdata_path(sysdata_path1.get());

			std::string username=GetLogonUserName();

			boost::filesystem::path path1 = sysdata_path / "RobotRaconteur";
			if (!boost::filesystem::is_directory(path1))
			{
				return boost::optional<boost::filesystem::path>();
			}

			DWORD security_size = 0;
			if (!GetFileSecurityW(path1.c_str(), OWNER_SECURITY_INFORMATION, NULL, 0, &security_size))
			{
				DWORD err = GetLastError();
				if (err != ERROR_INSUFFICIENT_BUFFER)
				{
					return boost::optional<boost::filesystem::path>();
				}
			}

			boost::shared_array<uint8_t> security(new uint8_t[security_size]);
			if (!GetFileSecurityW(path1.c_str(), OWNER_SECURITY_INFORMATION, (PSECURITY_DESCRIPTOR)security.get(), security_size, &security_size))
			{
				return boost::optional<boost::filesystem::path>();
			}

			PSID owner_sid;
			BOOL owner_defaulted;
			if (!GetSecurityDescriptorOwner(security.get(), &owner_sid, &owner_defaulted))
			{
				return boost::optional<boost::filesystem::path>();
			}

			LPWSTR sid1;
			if (!ConvertSidToStringSidW(owner_sid, &sid1))
			{
				FreeSid(owner_sid);
				return boost::optional<boost::filesystem::path>();
			}

			std::wstring sid(sid1);
			LocalFree(sid1);			
			
			if (sid != GetSIDStringForProcessId(-1) && sid != L"S-1-5-19")
			{
				return boost::optional<boost::filesystem::path>();
			}
			
			path1 /= "run";
			path1 /= "transport";
			path1 /= "local";
						
#else
			boost::filesystem::path path1="/var/run/robotraconteur/transport/local";			
#endif

			if (!boost::filesystem::is_directory(path1))
			{
				return boost::optional<boost::filesystem::path>();
			}

			return path1;

		}

		bool ReadInfoFile(const boost::filesystem::path& fname, std::map<std::string,std::string>& data)
		{
			detail::LocalTransportFD fd;

			boost::system::error_code open_err;
			fd.open_read(fname, open_err);
			if (open_err) return false;

			if (!fd.read_info()) return false;

			data = fd.info;
			return true;
		}
		
		boost::tuple<NodeID, RR_SHARED_PTR<LocalTransportFD> > GetNodeIDForNodeNameAndLock(const std::string& nodename)
		{
			NodeID nodeid;

			if (!boost::regex_match(nodename, boost::regex("^[a-zA-Z][a-zA-Z0-9_\\.\\-]*$")))
			{
				throw InvalidArgumentException("\"" + nodename + "\" is an invalid NodeName");
			}

			boost::filesystem::path p = GetUserNodeIDPath() / nodename;

#ifdef ROBOTRACONTEUR_WINDOWS

			RR_SHARED_PTR<LocalTransportFD> fd = RR_MAKE_SHARED<LocalTransportFD>();

			boost::system::error_code open_err;
			fd->open_lock_write(p, false, open_err);
			if (open_err)
			{
				if (open_err.value() == ERROR_SHARING_VIOLATION)
				{
					throw NodeNameAlreadyInUse();
				}

				throw SystemResourceException("Could not initialize LocalTransport server");
			}


#else
			boost::filesystem::path p_lock = detail::LocalTransportUtil::GetUserRunPath() / "nodeids";
			boost::filesystem::create_directories(p_lock);
			p_lock /= nodename + ".pid";

			RR_SHARED_PTR<LocalTransportFD> fd_run = RR_MAKE_SHARED<LocalTransportFD>();

			boost::system::error_code open_run_err;
			fd_run->open_lock_write(p_lock, false, open_run_err);
			if (open_run_err)
			{
				if (open_run_err.value() == boost::system::errc::no_lock_available)
				{
					throw NodeNameAlreadyInUse();
				}
				throw SystemResourceException("Could not initialize LocalTransport server");
			}

			std::string pid_str = boost::lexical_cast<std::string>(getpid());
			if (!fd_run->write(pid_str)) throw SystemResourceException("Could not initialize LocalTransport server");

			RR_SHARED_PTR<LocalTransportFD> fd = RR_MAKE_SHARED<LocalTransportFD>();

			boost::system::error_code open_err;
			fd->open_lock_write(p, false, open_err);
			if (open_err)
			{
				if (open_err.value() == boost::system::errc::read_only_file_system)
				{
					open_err = boost::system::error_code();
					fd->open_read(p, open_err);
					if (open_err)
					{
						throw InvalidOperationException("LocalTransport NodeID not set on read only filesystem");
					}
				}
				else
				{
					throw SystemResourceException("Could not initialize LocalTransport server");
				}
			}			

			
#endif
			size_t len = fd->file_len();
			
			if (len == 0 || len == -1 || len > 16*1024)
			{
				nodeid = NodeID::NewUniqueID();
				std::string dat = nodeid.ToString();
				fd->write(dat);
			}
			else
			{				
				std::string nodeid_str;
				fd->read(nodeid_str);
				try
				{
					boost::trim(nodeid_str);
					nodeid = NodeID(nodeid_str);
				}
				catch (std::exception&)
				{
					throw IOException("Error in NodeID mapping settings file");
				}
			}
#ifdef ROBOTRACONTEUR_WINDOWS
			return boost::make_tuple(nodeid,fd);
#else
			return boost::make_tuple(nodeid, fd_run);
#endif
		}

		RR_SHARED_PTR<LocalTransportFD> CreatePidFile(const boost::filesystem::path& path, bool for_name)
		{

#ifdef ROBOTRACONTEUR_WINDOWS
			std::string pid_str = boost::lexical_cast<std::string>(GetCurrentProcessId());
			RR_SHARED_PTR<LocalTransportFD> fd = RR_MAKE_SHARED<LocalTransportFD>();
			boost::system::error_code open_err;
			fd->open_lock_write(path, true, open_err);
			if (open_err)
			{				
				if (open_err.value() == ERROR_SHARING_VIOLATION)
				{
					if (!for_name)
					{
						throw NodeIDAlreadyInUse();
					}
					else
					{
						throw NodeNameAlreadyInUse();
					}
				}
				throw SystemResourcePermissionDeniedException("Could not initialize server");
			}			
#else

			mode_t old_mode=umask(~(S_IRUSR | S_IWUSR | S_IRGRP));

			BOOST_SCOPE_EXIT(old_mode) {
				umask(old_mode);
			} BOOST_SCOPE_EXIT_END

			RR_SHARED_PTR<LocalTransportFD> fd = RR_MAKE_SHARED<LocalTransportFD>();

			boost::system::error_code open_err;
			fd->open_lock_write(path, true, open_err);
			if (open_err)
			{
				if (open_err.value() == boost::system::errc::no_lock_available)
				{
					if (!for_name)
					{
						throw NodeIDAlreadyInUse();
					}
					else
					{
						throw NodeNameAlreadyInUse();
					}
				}
				throw SystemResourceException("Could not initialize LocalTransport server");
			}

			std::string pid_str = boost::lexical_cast<std::string>(getpid());			
#endif
			fd->write(pid_str);			

			return fd;
		}
		RR_SHARED_PTR<LocalTransportFD> CreateInfoFile(const boost::filesystem::path& path, std::map<std::string, std::string> info, bool for_name)
		{

			std::string username = detail::LocalTransportUtil::GetLogonUserName();


#ifdef ROBOTRACONTEUR_WINDOWS
			std::string pid_str = boost::lexical_cast<std::string>(GetCurrentProcessId()) + "\n";
			RR_SHARED_PTR<LocalTransportFD> fd = RR_MAKE_SHARED<LocalTransportFD>();
			boost::system::error_code open_err;
			fd->open_lock_write(path, true, open_err);
			if (open_err)
			{
				if (open_err.value() == ERROR_SHARING_VIOLATION)
				{
					if (!for_name)
					{
						throw NodeIDAlreadyInUse();
					}
					else
					{
						throw NodeNameAlreadyInUse();
					}
				}
				throw SystemResourcePermissionDeniedException("Could not initialize server");
			}
#else
			mode_t old_mode=umask(~(S_IRUSR | S_IWUSR | S_IRGRP));

			BOOST_SCOPE_EXIT(old_mode) {
				umask(old_mode);
			} BOOST_SCOPE_EXIT_END

			RR_SHARED_PTR<LocalTransportFD> fd = RR_MAKE_SHARED<LocalTransportFD>();

			boost::system::error_code open_err;
			fd->open_lock_write(path, true, open_err);
			if (open_err)
			{
				if (open_err.value() == boost::system::errc::no_lock_available)
				{
					if (!for_name)
					{
						throw NodeIDAlreadyInUse();
					}
					else
					{
						throw NodeNameAlreadyInUse();
					}
				}
				throw SystemResourceException("Could not initialize LocalTransport server");
			}

			std::string pid_str = boost::lexical_cast<std::string>(getpid());			
#endif
			info.insert(std::make_pair("pid", pid_str));
			info.insert(std::make_pair("username", username));

			fd->info = info;
			if (!fd->write_info()) throw SystemResourceException("Could not initialize server");

			return fd;
		}

		void RefreshInfoFile(RR_SHARED_PTR<LocalTransportFD> h_info, const std::string& service_nonce)
		{

			if (!h_info) return;

			boost::mutex::scoped_lock lock(h_info->this_lock);

			std::map<std::string, std::string>::iterator e = h_info->info.find("ServiceStateNonce");
			if (e==h_info->info.end())
			{
				h_info->info.insert(std::make_pair("ServiceStateNonce", service_nonce));
			}
			else
			{
				e->second = service_nonce;
			}

			h_info->reset();
			h_info->write_info();			
		}

		void FindNodesInDirectory(std::vector<NodeDiscoveryInfo>& nodeinfo, const boost::filesystem::path& path, const std::string& scheme, const boost::posix_time::ptime& now, boost::optional<std::string> username)
		{			
			boost::filesystem::path search_id = path / "by-nodeid";
			boost::filesystem::path search_name = path / "by-nodename";
			boost::filesystem::directory_iterator end_iter;
			for (boost::filesystem::directory_iterator dir_itr(search_id); dir_itr != end_iter; dir_itr++)
			{
				try
				{
					if (!boost::filesystem::is_regular_file(*dir_itr))
					{
						continue;
					}

					if (dir_itr->path().extension().string() != ".info")
					{
						continue;
					}

					std::map<std::string, std::string> info;
					if (!detail::LocalTransportUtil::ReadInfoFile(*dir_itr, info))
					{
						continue;
					}

					typedef std::map<std::string, std::string>::iterator e_type;
					e_type info_end = info.end();

					e_type nodeid1 = info.find("nodeid");
					e_type username1 = info.find("username");
					if (nodeid1 == info_end || username1 == info_end)
					{
						continue;
					}

					NodeID nodeid = nodeid1->second;
					std::string& username2 = username1->second;
					

					std::string url;
					if (username)
					{
						if (username2 != *username) continue;
						url = scheme + "://" + *username + "@localhost/?nodeid=" + nodeid.ToString("D") + "&service=RobotRaconteurServiceIndex";
					}
					else
					{
						url = scheme + ":///?nodeid=" + nodeid.ToString("D") + "&service=RobotRaconteurServiceIndex";
					}

					NodeDiscoveryInfo i;
					i.NodeID = nodeid;
					i.NodeName = "";
					NodeDiscoveryInfoURL iurl;
					iurl.URL = url;
					iurl.LastAnnounceTime = now;
					i.URLs.push_back(iurl);

					e_type service_nonce = info.find("ServiceStateNonce");
					if (service_nonce != info.end())
					{
						i.ServiceStateNonce = service_nonce->second;
					}

					nodeinfo.push_back(i);
				}
				catch (std::exception&) {}
			}

			for (boost::filesystem::directory_iterator dir_itr(search_name); dir_itr != end_iter; dir_itr++)
			{
				try
				{
					if (!boost::filesystem::is_regular_file(*dir_itr))
					{
						continue;
					}

					if (!boost::ends_with(dir_itr->path().filename().string(), ".info"))
					{
						continue;
					}

					std::map<std::string, std::string> info;
					if (!detail::LocalTransportUtil::ReadInfoFile(*dir_itr, info))
					{
						continue;
					}

					typedef std::map<std::string, std::string>::iterator e_type;
					e_type info_end = info.end();

					e_type nodeid1 = info.find("nodeid");
					e_type nodename1 = info.find("nodename");					
					if (nodeid1 == info_end || nodename1 == info_end)
					{
						continue;
					}

					NodeID nodeid = nodeid1->second;
					
					if (nodename1->second != boost::filesystem::change_extension(dir_itr->path().filename(), ""))
					{
						//TODO: Test?
						continue;
					}

					BOOST_FOREACH(NodeDiscoveryInfo& e1, nodeinfo)
					{
						if (e1.NodeID == nodeid)
						{
							if (e1.NodeName == "")
							{
								e1.NodeName = nodename1->second;
							}
						}
					}					
				}
				catch (std::exception&) {}
			}
		}

		RR_SHARED_PTR<LocalTransport::socket_type> FindAndConnectLocalSocket(ParseConnectionURLResult url, const std::vector<boost::filesystem::path>& search_paths, const std::vector<std::string>& usernames, boost::asio::io_service& io_service_)
		{
#ifdef ROBOTRACONTEUR_WINDOWS
			HANDLE h = INVALID_HANDLE_VALUE;
#else
			RR_SHARED_PTR<LocalTransport::socket_type> socket;
#endif	

			BOOST_FOREACH(const boost::filesystem::path& e, search_paths)
			{
				std::map<std::string, std::string> info_data;
				if (!url.nodeid.IsAnyNode())
				{
					boost::filesystem::path e2 = e / "by-nodeid";
					e2 /= url.nodeid.ToString("D") + ".info";

					if (!detail::LocalTransportUtil::ReadInfoFile(e2, info_data))
					{
						continue;
					}

					if (!url.nodename.empty())
					{
						std::map<std::string, std::string>::iterator name1 = info_data.find("nodename");
						if (name1 == info_data.end())
						{
							continue;
						}

						if (name1->second != url.nodename)
						{
							continue;
						}

						boost::filesystem::path e3 = e / "by-nodename";
						e3 /= url.nodename + ".info";

						std::map<std::string, std::string> info_data2;
						if (!detail::LocalTransportUtil::ReadInfoFile(e3, info_data2))
						{
							continue;
						}

						std::map<std::string, std::string>::iterator socket1 = info_data.find("socket");
						std::map<std::string, std::string>::iterator socket2 = info_data2.find("socket");

						if (socket1 == info_data.end() || socket2 == info_data2.end())
						{
							continue;
						}

						if (socket1->second != socket2->second)
						{
							continue;
						}
					}

				}
				else
				{
					boost::filesystem::path e2 = e / "by-nodename";
					e2 /= url.nodename + ".info";

					if (!detail::LocalTransportUtil::ReadInfoFile(e2, info_data))
					{
						continue;
					}
				}


				std::map<std::string, std::string>::iterator e2 = info_data.find("socket");
				if (e2 == info_data.end()) continue;

				std::string pipename = e2->second;
#ifdef ROBOTRACONTEUR_WINDOWS

				h = CreateFileA(pipename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION, NULL);
				if (h == INVALID_HANDLE_VALUE)
					continue;
				else
					break;
#else

				socket = RR_MAKE_SHARED<LocalTransport::socket_type>(boost::ref(io_service_));
				boost::asio::local::stream_protocol::endpoint ep(pipename);
				boost::system::error_code ec;
				socket->connect(ep, ec);
				if (ec)
				{
					socket.reset();
					continue;
				}
				break;

#endif
			}

#ifdef ROBOTRACONTEUR_WINDOWS
			if (h == INVALID_HANDLE_VALUE)
			{
				return RR_SHARED_PTR <LocalTransport::socket_type>();
			}
			RR_SHARED_PTR<LocalTransport::socket_type> socket = RR_MAKE_SHARED<LocalTransport::socket_type>(boost::ref(io_service_), h);
						
			if (usernames.size() > 0)
			{
				bool good_user = false;
				BOOST_FOREACH(const std::string& username, usernames)
				{
					if (detail::LocalTransportUtil::IsPipeUser(h, username))
					{
						good_user = true;
					}
				}

				if (!good_user) throw ConnectionException("Local socket user mismatch");
				
			}

#else
			//TODO: Check user on unix
			if (!socket) return RR_SHARED_PTR <LocalTransport::socket_type>();
#endif

			return socket;

		}
	}

	LocalTransportDiscovery::LocalTransportDiscovery(RR_SHARED_PTR<RobotRaconteurNode> node)
	{
		this->node = node;
	}

	void LocalTransportDiscovery::Refresh()
	{
		RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
		if (!node1) return;

		boost::posix_time::ptime now = node1->NowUTC();

		
		
		if (private_path)
		{
			std::vector<NodeDiscoveryInfo> nodeinfo;
			LocalTransportUtil::FindNodesInDirectory(nodeinfo, *private_path, "rr+local", now, boost::optional<std::string>());
			BOOST_FOREACH(NodeDiscoveryInfo& n, nodeinfo)
			{
				node1->NodeDetected(n);
			}
		}

		if (public_path)
		{
			try
			{
				boost::filesystem::directory_iterator end_iter;
				for (boost::filesystem::directory_iterator dir_itr(*public_path); dir_itr != end_iter; dir_itr++)
				{
					try
					{
						if (!boost::filesystem::is_directory(*dir_itr))
						{
							continue;
						}

						boost::filesystem::path path1 = dir_itr->path();

						std::string username1 = path1.filename().string();
						std::vector<NodeDiscoveryInfo> nodeinfo;
						detail::LocalTransportUtil::FindNodesInDirectory(nodeinfo, *dir_itr, "rr+local", now, username1);
						BOOST_FOREACH(NodeDiscoveryInfo& n, nodeinfo)
						{
							node1->NodeDetected(n);
						}
					}
					catch (std::exception&) {}
				}
			}
			catch (std::exception&) {}
		}		
	}

	LocalTransportFD::LocalTransportFD()
	{
#ifdef ROBOTRACONTEUR_WINDOWS
		fd = NULL;
#else
		fd = -1;
#endif
	}

	LocalTransportFD::~LocalTransportFD()
	{
#ifdef ROBOTRACONTEUR_WINDOWS
		CloseHandle(fd);
#else
		close(fd);
#endif
	}

	void LocalTransportFD::open_read(const boost::filesystem::path& path, boost::system::error_code& err)
	{
#ifdef BOOST_WINDOWS
		HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (h == INVALID_HANDLE_VALUE)
		{
			err = boost::system::error_code(GetLastError(), boost::system::system_category());
			return;
		}

		fd = h;
#else
		int fd1 = open(path.c_str(), O_CLOEXEC | O_RDONLY);
		if (fd1 < 0)
		{
			err = boost::system::error_code(errno, boost::system::system_category());
			return;
		}
		fd = fd1;
#endif
	}
	void LocalTransportFD::open_lock_write(const boost::filesystem::path& path, bool delete_on_close, boost::system::error_code& err)
	{
#ifdef BOOST_WINDOWS
		DWORD flags = FILE_ATTRIBUTE_NORMAL;
		if (delete_on_close)
		{
			flags |= FILE_FLAG_DELETE_ON_CLOSE;
		}
		HANDLE h = CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, flags, NULL);
		if (h == INVALID_HANDLE_VALUE)
		{
			err = boost::system::error_code(GetLastError(), boost::system::system_category());
			return;
		}

		fd = h;
#else
		int fd1 = open(path.c_str(), O_CLOEXEC | O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
		if (fd1 < 0)
		{
			err = boost::system::error_code(errno, boost::system::system_category());
			return;
		}

		struct ::flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = 0;
		lock.l_len = 0;
		if (::fcntl(fd1, F_SETLK, &lock) < 0)
		{
			close(fd1);
			err = boost::system::error_code(boost::system::errc::no_lock_available, boost::system::system_category());
			return;
		}

		fd = fd1;
#endif
	}

	/*void LocalTransportFD::reopen_lock_write(bool delete_on_close, boost::system::error_code& err)
	{
		DWORD flags = FILE_ATTRIBUTE_NORMAL;
		if (delete_on_close)
		{
			flags |= FILE_FLAG_DELETE_ON_CLOSE;
		}
		HANDLE h = ReOpenFile(fd, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, flags);
		if (h == INVALID_HANDLE_VALUE)
		{
			err = boost::system::error_code(GetLastError(), boost::system::system_category());
			return;
		}

		fd = h;
	}*/

	bool LocalTransportFD::read(std::string& data)
	{
#ifdef ROBOTRACONTEUR_WINDOWS
		if (::SetFilePointer(fd, 0, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER) return false;
		DWORD len = GetFileSize(fd, NULL);
		if (len == INVALID_FILE_SIZE) return false;
		if (len > 16 * 1024) return false;
		std::string ret;
		ret.resize(len);
		DWORD bytes_read;
		if (!::ReadFile(fd, &ret[0], len, &bytes_read, NULL))
		{
			return false;
		}

		if (bytes_read != len) return false;
		data = ret;
		return true;
#else
		if (lseek(fd, 0, SEEK_END) < 0) return false;
		int len = lseek(fd, 0, SEEK_CUR);
		if (len < 0) return false;
		if (lseek(fd, 0, SEEK_SET) < 0) return false;

		std::string ret;
		ret.resize(len);
		
		int retval = ::read(fd, &ret[0], len);
		if (retval < 0)
		{
			return false;
		}

		if (retval != len) return false;
		data = ret;
		return true;
#endif
	}

	bool LocalTransportFD::read_info()
	{
		std::string in;
		if (!read(in)) return false;

		std::vector<std::string> lines;
		boost::split(lines, in, boost::is_any_of("\n"), boost::algorithm::token_compress_on);
		info.clear();
		BOOST_FOREACH(std::string& l, lines)
		{
			boost::regex r("^\\s*([\\w+\\.\\-]+)\\s*\\:\\s*(.*)\\s*$");
			boost::smatch r_match;
			if (!boost::regex_match(l, r_match, r))
			{
				continue;
			}			
			info.insert(std::make_pair(boost::trim_copy(r_match[1].str()), boost::trim_copy(r_match[2].str())));
		}
		return true;
	}

	bool LocalTransportFD::write(const std::string& data)
	{
#ifdef ROBOTRACONTEUR_WINDOWS
		DWORD bytes_written = 0;
		if (!::WriteFile(fd, &data[0], data.size(), &bytes_written, NULL)) return false;
		if (bytes_written != data.size()) return false;
		if(!FlushFileBuffers(fd)) return false;
#else		
		int ret = ::write(fd, &data[0], data.size());
		if (ret != data.size()) return false;
		if (fsync(fd) < 0) return false;
#endif
		return true;
	}

	bool LocalTransportFD::write_info()
	{
		std::string data;
		for (std::map<std::string, std::string>::iterator e = info.begin(); e != info.end(); e++)
		{
			data += e->first + ": " + e->second + "\n";
		}

		return write(data);
	}

	bool LocalTransportFD::reset()
	{
#ifdef ROBOTRACONTEUR_WINDOWS
		if(::SetFilePointer(fd, 0, 0, FILE_BEGIN)== INVALID_SET_FILE_POINTER) return false;
		if(!::SetEndOfFile(fd)) return false;
#else
		if (lseek(fd, 0, SEEK_SET) < 0) return false;
		if (ftruncate(fd, 0) < 0) return false;
#endif
		return true;
	}

	size_t LocalTransportFD::file_len()
	{
#ifdef ROBOTRACONTEUR_WINDOWS
		return ::GetFileSize(fd, NULL);
#else
		int init_pos = lseek(fd, 0, SEEK_CUR);
		if (init_pos < 0) return -1;
		if (lseek(fd, 0, SEEK_END) < 0) return -1;
		int len = lseek(fd, 0, SEEK_CUR);
		if (len < 0) return -1;
		if (lseek(fd, init_pos, SEEK_SET) < 0) return -1;
		return len;
#endif
	}

}

}
