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

#include "boost_asio_win_unix_sockets_backport.h"

#include <boost/bind/placeholders.hpp>
#include <boost/asio.hpp>
#include <boost/assign/list_of.hpp>

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

namespace detail
{
class LocalTransport_socket
{
  public:
    LocalTransport_socket(RR_BOOST_ASIO_IO_CONTEXT& context)
    {
        socket = RR_SHARED_PTR<boost::asio::local::stream_protocol::socket>(
            new boost::asio::local::stream_protocol::socket(context));
    }
    RR_SHARED_PTR<boost::asio::local::stream_protocol::socket> socket;
};
class LocalTransport_acceptor
{
  public:
    LocalTransport_acceptor(RR_BOOST_ASIO_IO_CONTEXT& context, const boost::asio::local::stream_protocol::endpoint& ep)
        : acceptor(context, ep)
    {}
    LocalTransport_acceptor(RR_BOOST_ASIO_IO_CONTEXT& context) : acceptor(context) {}
    boost::asio::local::stream_protocol::acceptor acceptor;
};
} // namespace detail

static std::string LocalTransport_socket_remote_endpoint(const RR_SHARED_PTR<detail::LocalTransport_socket>& socket)
{
    boost::system::error_code ec;
    boost::asio::local::stream_protocol::endpoint ep = socket->socket->remote_endpoint(ec);
    if (ec)
    {
        return "[unknown]";
    }
    std::stringstream ss;
    ss << ep;
    return ss.str();
}

LocalTransport::LocalTransport(const RR_SHARED_PTR<RobotRaconteurNode>& node) : Transport(node)
{

    if (!node)
        throw InvalidArgumentException("Node cannot be null");

    transportopen = false;
    this->node = node;

    fds = RR_MAKE_SHARED<detail::LocalTransportFDs>();

    max_message_size = 12 * 1024 * 1024;

#ifndef ROBOTRACONTEUR_DISABLE_MESSAGE4
    disable_message4 = false;
#else
    disable_message4 = true;
#endif
#ifndef ROBOTRACONTEUR_DISABLE_STRINGTABLE
    disable_string_table = false;
#else
    disable_string_table = true;
#endif
    disable_async_message_io = false;

    closed = false;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "LocalTransport created");
}

LocalTransport::~LocalTransport() {}

void LocalTransport::Close()
{
    {
        boost::mutex::scoped_lock lock(closed_lock);
        if (closed)
            return;
        closed = true;
    }

    try
    {
        boost::mutex::scoped_lock lock(acceptor_lock);
        if (acceptor)
        {
            acceptor->acceptor.close();
            acceptor.reset();
        }
    }
    catch (std::exception&)
    {}

    std::vector<RR_SHARED_PTR<ITransportConnection> > t;

    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        boost::copy(TransportConnections | boost::adaptors::map_values, std::back_inserter(t));
    }

    BOOST_FOREACH (RR_SHARED_PTR<ITransportConnection>& e, t)
    {
        try
        {
            e->Close();
        }
        catch (std::exception&)
        {}
    }

    {
        boost::mutex::scoped_lock lock2(fds_lock);

        fds.reset();
        fds = RR_MAKE_SHARED<detail::LocalTransportFDs>();
    }

#ifndef ROBOTRACONTEUR_WINDOWS
    if (!socket_file_name.empty())
    {
        unlink(socket_file_name.c_str());
    }
#else
    if (!socket_file_name.empty())
    {
        DeleteFileA(socket_file_name.c_str());
    }
#endif

    DisableNodeDiscoveryListening();

    close_signal();

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1, "LocalTransport closed");
}

bool LocalTransport::IsServer() const { return true; }

bool LocalTransport::IsClient() const { return true; }

std::string LocalTransport::GetUrlSchemeString() const { return "rr+local"; }

std::vector<std::string> LocalTransport::GetServerListenUrls()
{
    std::vector<std::string> o;
    if (acceptor)
    {
        NodeID nodeid = GetNode()->NodeID();
        o.push_back("rr+local:///?nodeid=" + nodeid.ToString("D"));
    }
    return o;
}

bool LocalTransport::CanConnectService(boost::string_ref url) { return (boost::starts_with(url, "rr+local://")); }

void LocalTransport::AsyncCreateTransportConnection(
    boost::string_ref url, const RR_SHARED_PTR<Endpoint>& ep,
    boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>&
        callback)
{
    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                      "LocalTransport begin create transport connection with URL: " << url);

    ParseConnectionURLResult url_res = ParseConnectionURL(url);

    if (url_res.nodename.empty() && url_res.nodeid.IsAnyNode())
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                           "LocalTransport NodeID and/or NodeName not specified in URL: " << url);
        throw ConnectionException("NodeID and/or NodeName must be specified for LocalTransport");
    }

    std::string my_username = NodeDirectoriesUtil::GetLogonUserName();
    NodeDirectories node_dirs = GetNode()->GetNodeDirectories();

    boost::filesystem::path user_path = detail::LocalTransportUtil::GetTransportPrivateSocketPath(node_dirs);
    boost::optional<boost::filesystem::path> public_user_path =
        detail::LocalTransportUtil::GetTransportPublicSocketPath(node_dirs);
    boost::optional<boost::filesystem::path> public_search_path =
        detail::LocalTransportUtil::GetTransportPublicSearchPath(node_dirs);

    std::vector<boost::filesystem::path> search_paths;

    std::string host = url_res.host;

    if (url_res.port != -1)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                           "LocalTransport must not contain port, invalid URL: " << url);
        throw ConnectionException("Invalid url for local transport");
    }
    if (!url_res.path.empty() && url_res.path != "/")
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                           "LocalTransport must not contain a path, invalid URL: " << url);
        throw ConnectionException("Invalid url for local transport");
    }

    std::string username;

    std::vector<std::string> usernames;

    if (!boost::contains(host, "@"))
    {
        if (host != "localhost" && !host.empty())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                node, Transport, ep->GetLocalEndpoint(),
                "LocalTransport host must be empty or \"localhost\", invalid URL: " << url);
            throw ConnectionException("Invalid host for local transport");
        }
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                           "LocalTransport searching \"" << user_path << "\" for URL: " << url);
        search_paths.push_back(user_path);

        if (public_user_path)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                               "LocalTransport searching \"" << *public_user_path
                                                                             << "\" for URL: " << url);
            search_paths.push_back(*public_user_path);
        }

        usernames.push_back(NodeDirectoriesUtil::GetLogonUserName());

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
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                                   "LocalTransport searching \"" << service_path
                                                                                 << "\" for URL: " << url);
                search_paths.push_back(service_path);
            }

            usernames.push_back(service_username);
        }
    }
    else
    {
        std::vector<std::string> v1;
        boost::split(v1, host, boost::is_from_range('@', '@'));
        if (v1.size() != 2)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                               "LocalTransport malformed username@localhost URL: " << url);
            throw ConnectionException("Malformed URL");
        }
        if (v1.at(1) != "localhost")
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                               "LocalTransport malformed username@localhost URL: " << url);
            throw ConnectionException("Invalid host for local transport");
        }

        username = boost::trim_copy(v1.at(0));

        if (!boost::regex_match(username, boost::regex("^[a-zA-Z][a-zA-Z0-9_\\-]*$")))
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                               "LocalTransport invalid username in URL: " << url);
            throw ConnectionException("\"" + username + "\" is an invalid username");
        }

        if (username == my_username)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                               "LocalTransport searching \"" << user_path << "\" for URL: " << url);
            search_paths.push_back(user_path);
            if (public_user_path)
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                                   "LocalTransport searching \"" << *public_user_path
                                                                                 << "\" public path for URL: " << url);
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
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                                       "LocalTransport searching \""
                                                           << service_path << "\" public path for URL: " << url);
                    search_paths.push_back(service_path);
                }
            }
        }
        usernames.push_back(username);
    }

    // TODO: test this
    RR_SHARED_PTR<detail::LocalTransport_socket> socket = detail::LocalTransportUtil::FindAndConnectLocalSocket(
        url_res, search_paths, usernames, GetNode()->GetThreadPool()->get_io_context());
    if (!socket)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                           "LocalTransport could not connect to URL: " << url);
        throw ConnectionException("Could not connect to service");
    }

    std::string noden;
    if (!(url_res.nodeid.IsAnyNode() && !url_res.nodename.empty()))
    {
        noden = url_res.nodeid.ToString();
    }
    else
    {
        noden = url_res.nodename;
    }

    boost::function<void(const RR_SHARED_PTR<detail::LocalTransport_socket>&,
                         const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
        h = boost::bind(&LocalTransport::AsyncCreateTransportConnection2, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                        noden, RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3), callback);
    LocalTransport_attach_transport(shared_from_this(), socket, false, ep->GetLocalEndpoint(), noden, h);
}

void LocalTransport::AsyncCreateTransportConnection2(
    const RR_SHARED_PTR<detail::LocalTransport_socket>& socket, const std::string& noden,
    const RR_SHARED_PTR<ITransportConnection>& transport, const RR_SHARED_PTR<RobotRaconteurException>& err,
    boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>&
        callback)
{
    RR_UNUSED(noden);
    if (err)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, transport->GetLocalEndpoint(),
                                          "LocalTransport failed to connect: " << err->what());
        try
        {
            callback(RR_SHARED_PTR<ITransportConnection>(), err);
            return;
        }
        catch (std::exception& err2)
        {
            RobotRaconteurNode::TryHandleException(node, &err2);
            return;
        }
    }

    register_transport(transport);

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, transport->GetLocalEndpoint(),
                                      "LocalTransport connected transport to \""
                                          << LocalTransport_socket_remote_endpoint(socket) << "\"");
    callback(transport, RR_SHARED_PTR<RobotRaconteurException>());
}

RR_SHARED_PTR<ITransportConnection> LocalTransport::CreateTransportConnection(boost::string_ref url,
                                                                              const RR_SHARED_PTR<Endpoint>& e)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<ITransportConnection> > d =
        RR_MAKE_SHARED<detail::sync_async_handler<ITransportConnection> >(
            RR_MAKE_SHARED<ConnectionException>("Timeout exception"));

    boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)> h =
        boost::bind(&detail::sync_async_handler<ITransportConnection>::operator(), d, RR_BOOST_PLACEHOLDERS(_1),
                    RR_BOOST_PLACEHOLDERS(_2));
    AsyncCreateTransportConnection(url, e, h);

    return d->end();
}

void LocalTransport::CloseTransportConnection(const RR_SHARED_PTR<Endpoint>& e)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, e->GetLocalEndpoint(),
                                       "LocalTransport request close transport connection");

    RR_SHARED_PTR<ServerEndpoint> e2 = boost::dynamic_pointer_cast<ServerEndpoint>(e);
    if (e2)
    {
        RR_SHARED_PTR<boost::asio::deadline_timer> timer(
            new boost::asio::deadline_timer(GetNode()->GetThreadPool()->get_io_context()));
        timer->expires_from_now(boost::posix_time::milliseconds(1000));
        RobotRaconteurNode::asio_async_wait(node, timer,
                                            boost::bind(&LocalTransport::CloseTransportConnection_timed,
                                                        shared_from_this(), boost::asio::placeholders::error, e,
                                                        timer));
        return;
    }

    RR_SHARED_PTR<ITransportConnection> t;

    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(e->GetLocalEndpoint());
        if (e1 == TransportConnections.end())
            return;
        t = e1->second;
        TransportConnections.erase(e1);
    }

    if (t)
    {
        try
        {
            t->Close();
        }
        catch (std::exception&)
        {}
    }
}

void LocalTransport::CloseTransportConnection_timed(const boost::system::error_code& err,
                                                    const RR_SHARED_PTR<Endpoint>& e, const RR_SHARED_PTR<void>& timer)
{
    RR_UNUSED(timer);
    if (err)
        return;

    RR_SHARED_PTR<ITransportConnection> t;

    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(e->GetLocalEndpoint());
        if (e1 == TransportConnections.end())
            return;
        t = e1->second;
    }

    if (t)
    {
        try
        {
            t->Close();
        }
        catch (std::exception&)
        {}
    }
}

/*void LocalTransport::StartServer(boost::string_ref name)
{
    boost::mutex::scoped_lock lock(acceptor_lock);

    if (acceptor) throw InvalidOperationException("Server already running");
#ifdef ROBOTRACONTEUR_WINDOWS
    acceptor=RR_MAKE_SHARED<socket_acceptor_type>("\\\\.\\pipe\\RobotRaconteur_" + name,20,GetNode());
    acceptor->listen();

    acceptor->async_accept(boost::bind(&LocalTransport::handle_accept,shared_from_this(),acceptor,RR_BOOST_PLACEHOLDERS(_2),boost::asio::placeholders::error));
#else
    std::string fname="/tmp/RobotRaconteur_" + name;
    RR_SHARED_PTR<detail::LocalTransport_socket>
socket=RR_MAKE_SHARED<detail::LocalTransport_socket>(boost::ref(GetNode()->GetThreadPool()->get_io_context()));
    boost::asio::local::stream_protocol::endpoint ep(fname);
    acceptor=RR_MAKE_SHARED<socket_acceptor_type>(boost::ref(GetNode()->GetThreadPool()->get_io_context()),ep);
    acceptor->listen();
    acceptor->async_accept(*socket,boost::bind(&LocalTransport::handle_accept,shared_from_this(),acceptor,socket,boost::asio::placeholders::error));
#endif

}*/

void LocalTransport::StartClientAsNodeName(boost::string_ref name)
{
    if (!boost::regex_match(name.begin(), name.end(), boost::regex("^[a-zA-Z][a-zA-Z0-9_\\.\\-]*$")))
    {
        throw InvalidArgumentException("\"" + name + "\" is an invalid NodeName");
    }

    NodeDirectories node_dirs = GetNode()->GetNodeDirectories();

    try
    {
        boost::mutex::scoped_lock lock(acceptor_lock);

        GetUuidForNameAndLockResult p = GetUuidForNameAndLock(node_dirs, name, boost::assign::list_of("nodeids"));

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                           "LocalTransport loaded NodeID " << p.uuid.ToString() << "for NodeName \""
                                                                           << name << "\"");

        try
        {
            GetNode()->SetNodeID(p.uuid);
        }
        catch (std::exception&)
        {
            if (GetNode()->NodeID() != p.uuid)
                throw;
        }

        try
        {
            GetNode()->SetNodeName(name);
        }
        catch (std::exception&)
        {
            if (GetNode()->NodeName() != name)
                throw;
        }

        {
            boost::mutex::scoped_lock lock(fds_lock);
            fds->h_nodename_s = p.fd;
        }
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, Transport, -1,
                                           "LocalTransport could not start client with NodeName \""
                                               << name << "\": " << exp.what());
        throw;
    }
}

bool LocalTransport::IsLocalTransportSupported()
{
#ifdef ROBOTRACONTEUR_WINDOWS
    // Test for AF_UNIX support by trying to create socket
    SOCKET sock = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock != INVALID_SOCKET)
    {
        boost::system::error_code ec;
        boost::filesystem::path tmpfile =
            boost::filesystem::temp_directory_path() / boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%", ec);
        if (!ec)
        {
            sockaddr_un tmpaddr;
            tmpaddr.sun_family = AF_UNIX;
            std::string tmpfile_str = tmpfile.string();
            if (tmpfile_str.size() - 1 < UNIX_PATH_MAX)
            {
                strcpy(tmpaddr.sun_path, tmpfile_str.c_str());
                if (::bind(sock, (SOCKADDR*)&tmpaddr, sizeof(tmpaddr)) == SOCKET_ERROR)
                {
                    ::closesocket(sock);
                    return false;
                }
                DeleteFileA(tmpfile_str.c_str());
            }
        }

        ::closesocket(sock);
        return true;
    }
    return false;
#else
    return true;
#endif
}

void LocalTransport::StartServerAsNodeName(boost::string_ref name, bool public_)
{
    if (!IsLocalTransportSupported())
    {
        ROBOTRACONTEUR_LOG_WARNING_COMPONENT(
            node, Transport, -1,
            "LocalTransport not supported on this operating system. Other transports will operate normally");
        StartClientAsNodeName(name);
        return;
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                       "LocalTransport starting server with NodeName \"" << name << "\"");

    NodeDirectories node_dirs = GetNode()->GetNodeDirectories();

    try
    {
        boost::mutex::scoped_lock lock(acceptor_lock);

        RR_SHARED_PTR<detail::LocalTransportNodeNameLock> nodename_lock =
            detail::LocalTransportNodeNameLock::Lock(name.to_string());
        if (!nodename_lock)
            throw NodeNameAlreadyInUse();

        GetUuidForNameAndLockResult nodeid1 = GetUuidForNameAndLock(node_dirs, name, boost::assign::list_of("nodeids"));
        NodeID& nodeid = nodeid1.uuid;

        RR_SHARED_PTR<detail::LocalTransportNodeIDLock> nodeid_lock = detail::LocalTransportNodeIDLock::Lock(nodeid);
        if (!nodeid_lock)
            throw NodeIDAlreadyInUse();

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                           "LocalTransport loaded NodeID " << nodeid.ToString() << "for NodeName \""
                                                                           << name << "\"");

        int32_t tries = 0;

        boost::filesystem::path socket_path;

        if (!public_)
        {
            socket_path = detail::LocalTransportUtil::GetTransportPrivateSocketPath(node_dirs);
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(
                node, Transport, -1, "LocalTransport server using private socket_path: \"" << socket_path << "\"");
        }
        else
        {
            boost::optional<boost::filesystem::path> socket_path1 =
                detail::LocalTransportUtil::GetTransportPublicSocketPath(node_dirs);
            if (!socket_path1)
                throw ConnectionException("Computer not initialized for public node server");
            socket_path = *socket_path1;
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(
                node, Transport, -1, "LocalTransport server using public socket_path: \"" << socket_path << "\"");
        }

        std::string pipename;

        while (true)
        {

            if (public_)
            {
                pipename = socket_path.string() + ROBOTRACONTEUR_PATHSEP "socket" ROBOTRACONTEUR_PATHSEP;
            }
            else
            {
                pipename = node_dirs.user_run_dir.string() + ROBOTRACONTEUR_PATHSEP "socket" ROBOTRACONTEUR_PATHSEP;
            }
            pipename += GetNode()->GetRandomString(16) + ".sock";

            sockaddr_un empty_sockaddr = {};
            if (pipename.size() > (sizeof(empty_sockaddr.sun_path) - 1))
            {
                throw RobotRaconteur::SystemResourceException("Local socket path name exceeds UNIX_PATH_MAX");
            }

            try
            {

                RR_SHARED_PTR<detail::LocalTransport_socket> socket(
                    new detail::LocalTransport_socket(GetNode()->GetThreadPool()->get_io_context()));
                boost::asio::local::stream_protocol::endpoint ep(pipename);
                acceptor = RR_SHARED_PTR<detail::LocalTransport_acceptor>(
                    new detail::LocalTransport_acceptor(GetNode()->GetThreadPool()->get_io_context()));

                acceptor->acceptor.open();
                acceptor->acceptor.bind(ep);

                acceptor->acceptor.listen();
#ifdef ROBOTRACONTEUR_WINDOWS
                detail::LocalTransportUtil::SetWindowsSocketPermissions(pipename, public_);
#else
                chmod(pipename.c_str(), S_IRWXU | S_IRWXG);
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
        boost::filesystem::path pid_name_fname = socket_path / "by-nodename" / (name + ".pid");
        boost::filesystem::path info_name_fname = socket_path / "by-nodename" / (name + ".info");

        std::map<std::string, std::string> info;
        info.insert(std::make_pair("nodename", name.to_string()));
        info.insert(std::make_pair("nodeid", nodeid.ToString()));
        info.insert(std::make_pair("socket", pipename));
        info.insert(std::make_pair("ServiceStateNonce", GetNode()->GetServiceStateNonce()));
        RR_SHARED_PTR<NodeDirectoriesFD> h_pid_id_s;
        RR_SHARED_PTR<NodeDirectoriesFD> h_info_id_s;
        RR_SHARED_PTR<NodeDirectoriesFD> h_pid_name_s;
        RR_SHARED_PTR<NodeDirectoriesFD> h_info_name_s;
        try
        {
            h_pid_id_s = NodeDirectoriesUtil::CreatePidFile(pid_id_fname);
            h_info_id_s = NodeDirectoriesUtil::CreateInfoFile(info_id_fname, info);
        }
        catch (NodeDirectoriesResourceAlreadyInUse&)
        {
            throw NodeIDAlreadyInUse();
        }
        try
        {
            h_pid_name_s = NodeDirectoriesUtil::CreatePidFile(pid_name_fname);
            h_info_name_s = NodeDirectoriesUtil::CreateInfoFile(info_name_fname, info);
        }
        catch (NodeDirectoriesResourceAlreadyInUse&)
        {
            throw NodeNameAlreadyInUse();
        }

        try
        {
            GetNode()->SetNodeID(nodeid);
        }
        catch (std::exception&)
        {
            if (GetNode()->NodeID() != nodeid)
                throw;
        }

        try
        {
            GetNode()->SetNodeName(name);
        }
        catch (std::exception&)
        {
            if (GetNode()->NodeName() != name)
                throw;
        }

        RR_SHARED_PTR<detail::LocalTransport_socket> socket(
            new detail::LocalTransport_socket(GetNode()->GetThreadPool()->get_io_context()));
        acceptor->acceptor.async_accept(*socket->socket,
                                        boost::bind(&LocalTransport::handle_accept, shared_from_this(), acceptor,
                                                    socket, boost::asio::placeholders::error));

        {
            boost::mutex::scoped_lock lock(fds_lock);
            fds->h_nodename_s = nodeid1.fd;
            fds->h_pid_id_s = h_pid_id_s;
            fds->h_info_id_s = h_info_id_s;
            fds->h_pid_name_s = h_pid_name_s;
            fds->h_info_name_s = h_info_name_s;
            fds->nodeid_lock = nodeid_lock;
            fds->nodename_lock = nodename_lock;
        }

        socket_file_name = pipename;

        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1,
                                          "LocalTransport started server for NodeName \""
                                              << name << "\" with NodeID " << nodeid.ToString() << " unix socket "
                                              << pipename);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, Transport, -1,
                                           "LocalTransport could not start server: " << exp.what());
        throw;
    }
}

void LocalTransport::StartServerAsNodeID(const NodeID& nodeid1, bool public_)
{
    if (!IsLocalTransportSupported())
    {
        ROBOTRACONTEUR_LOG_WARNING_COMPONENT(
            node, Transport, -1,
            "LocalTransport not supported on this operating system. Other transports will operate normally");
        try
        {
            GetNode()->SetNodeID(nodeid1);
        }
        catch (std::exception&)
        {
            if (GetNode()->NodeID() != nodeid1)
                throw;
        }
        return;
    }

    const NodeID& nodeid = nodeid1;
    if (nodeid.IsAnyNode())
    {
        ROBOTRACONTEUR_LOG_WARNING_COMPONENT(node, Transport, -1,
                                             "LocalTransport cannot start server with zero node (any node) NodeID");
        throw InvalidArgumentException("NodeID must not be zero node");
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                       "LocalTransport starting server with NodeID " << nodeid.ToString());

    NodeDirectories node_dirs = GetNode()->GetNodeDirectories();

    try
    {
        boost::mutex::scoped_lock lock(acceptor_lock);

        RR_SHARED_PTR<detail::LocalTransportNodeIDLock> nodeid_lock = detail::LocalTransportNodeIDLock::Lock(nodeid);
        if (!nodeid_lock)
            throw NodeIDAlreadyInUse();

        int32_t tries = 0;

        std::string pipename;

        boost::filesystem::path socket_path;

        if (!public_)
        {
            socket_path = detail::LocalTransportUtil::GetTransportPrivateSocketPath(node_dirs);
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(
                node, Transport, -1, "LocalTransport server using private socket_path: \"" << socket_path << "\"");
        }
        else
        {
            boost::optional<boost::filesystem::path> socket_path1 =
                detail::LocalTransportUtil::GetTransportPublicSocketPath(node_dirs);
            if (!socket_path1)
                throw ConnectionException("Computer not initialized for public node server");
            socket_path = *socket_path1;
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(
                node, Transport, -1, "LocalTransport server using public socket_path: \"" << socket_path << "\"");
        }

        while (true)
        {

            if (public_)
            {
                pipename = socket_path.string() + ROBOTRACONTEUR_PATHSEP "socket" ROBOTRACONTEUR_PATHSEP;
            }
            else
            {
                pipename = node_dirs.user_run_dir.string() + ROBOTRACONTEUR_PATHSEP "socket" ROBOTRACONTEUR_PATHSEP;
            }
            pipename += GetNode()->GetRandomString(16) + ".sock";

            sockaddr_un empty_sockaddr = {};
            if (pipename.size() > (sizeof(empty_sockaddr.sun_path) - 1))
            {
                throw RobotRaconteur::SystemResourceException("Local socket path name exceeds UNIX_PATH_MAX");
            }

            try
            {

                RR_SHARED_PTR<detail::LocalTransport_socket> socket(
                    new detail::LocalTransport_socket(GetNode()->GetThreadPool()->get_io_context()));
                boost::asio::local::stream_protocol::endpoint ep(pipename);
                acceptor = RR_SHARED_PTR<detail::LocalTransport_acceptor>(
                    new detail::LocalTransport_acceptor(GetNode()->GetThreadPool()->get_io_context(), ep));
                acceptor->acceptor.open();
                acceptor->acceptor.listen();
#ifdef ROBOTRACONTEUR_WINDOWS
                detail::LocalTransportUtil::SetWindowsSocketPermissions(pipename, public_);
#else
                chmod(pipename.c_str(), S_IRWXU | S_IRWXG);
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

        RR_SHARED_PTR<NodeDirectoriesFD> h_pid_id_s;
        RR_SHARED_PTR<NodeDirectoriesFD> h_info_id_s;
        try
        {
            h_pid_id_s = NodeDirectoriesUtil::CreatePidFile(pid_id_fname);
            h_info_id_s = NodeDirectoriesUtil::CreateInfoFile(info_id_fname, info);
        }
        catch (NodeDirectoriesResourceAlreadyInUse&)
        {
            throw NodeIDAlreadyInUse();
        }

        try
        {
            GetNode()->SetNodeID(nodeid);
        }
        catch (std::exception&)
        {
            if (GetNode()->NodeID() != nodeid)
                throw;
        }

        RR_SHARED_PTR<detail::LocalTransport_socket> socket(
            new detail::LocalTransport_socket(GetNode()->GetThreadPool()->get_io_context()));
        acceptor->acceptor.async_accept(*socket->socket,
                                        boost::bind(&LocalTransport::handle_accept, shared_from_this(), acceptor,
                                                    socket, boost::asio::placeholders::error));

        {
            boost::mutex::scoped_lock lock(fds_lock);
            fds->h_pid_id_s = h_pid_id_s;
            fds->h_info_id_s = h_info_id_s;
            fds->nodeid_lock = nodeid_lock;
        }

        socket_file_name = pipename;

        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1,
                                          "LocalTransport started server for NodeID " << nodeid.ToString()
                                                                                      << " unix socket " << pipename);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, Transport, -1,
                                           "LocalTransport could not start server: " << exp.what());
        throw;
    }
}

void LocalTransport::SendMessage(const RR_INTRUSIVE_PTR<Message>& m)
{

    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(m->header->SenderEndpoint);
        if (e1 == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m->header->SenderEndpoint,
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e1->second;
    }
    t->SendMessage(m);
}

uint32_t LocalTransport::TransportCapability(boost::string_ref name)
{
    RR_UNUSED(name);
    return 0;
}

void LocalTransport::PeriodicCleanupTask()
{
    boost::mutex::scoped_lock lock(TransportConnections_lock);
    for (RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e = TransportConnections.begin();
         e != TransportConnections.end();)
    {
        try
        {
            RR_SHARED_PTR<LocalTransportConnection> e2 = rr_cast<LocalTransportConnection>(e->second);
            if (!e2->IsConnected())
            {
                e = TransportConnections.erase(e);
            }
            else
            {
                e++;
            }
        }
        catch (std::exception&)
        {}
    }
}

void LocalTransport::AsyncSendMessage(
    const RR_INTRUSIVE_PTR<Message>& m,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(m->header->SenderEndpoint);
        if (e1 == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m->header->SenderEndpoint,
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e1->second;
    }

    t->AsyncSendMessage(m, handler);
}

void LocalTransport::handle_accept(const RR_SHARED_PTR<LocalTransport>& parent,
                                   const RR_SHARED_PTR<detail::LocalTransport_acceptor>& acceptor,
                                   const RR_SHARED_PTR<detail::LocalTransport_socket>& socket,
                                   const boost::system::error_code& error)
{
    if (error)
        return;

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(parent->node, Transport, 0, "LocalTransport accepted socket");
    try
    {
        boost::function<void(const RR_SHARED_PTR<detail::LocalTransport_socket>&,
                             const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            h = boost::bind(&LocalTransport_connected_callback2, parent, RR_BOOST_PLACEHOLDERS(_1),
                            RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3));
        LocalTransport_attach_transport(parent, socket, true, 0, "{0}", h);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(parent->node, Transport, 0,
                                          "LocalTransport accepted socket closed with error: " << exp.what());
        RobotRaconteurNode::TryHandleException(parent->node, &exp);
    }

    boost::mutex::scoped_lock lock(parent->acceptor_lock);

    RR_SHARED_PTR<detail::LocalTransport_socket> socket2(
        new detail::LocalTransport_socket(parent->GetNode()->GetThreadPool()->get_io_context()));
    acceptor->acceptor.async_accept(*socket2->socket, boost::bind(&LocalTransport::handle_accept, parent, acceptor,
                                                                  socket2, boost::asio::placeholders::error));
}

void LocalTransport::register_transport(const RR_SHARED_PTR<ITransportConnection>& connection)
{
    boost::mutex::scoped_lock lock(TransportConnections_lock);
    TransportConnections.insert(std::make_pair(connection->GetLocalEndpoint(), connection));
}

void LocalTransport::erase_transport(const RR_SHARED_PTR<ITransportConnection>& connection)
{
    try
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(connection->GetLocalEndpoint());
        if (e1 == TransportConnections.end())
            return;
        if (e1->second == connection)
        {
            TransportConnections.erase(e1);
        }
    }
    catch (std::exception&)
    {}

    TransportConnectionClosed(connection->GetLocalEndpoint());
}

void LocalTransport::MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) { GetNode()->MessageReceived(m); }

void LocalTransport::AsyncGetDetectedNodes(
    const std::vector<std::string>& schemes,
    const boost::function<void(const RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >&)>& handler, int32_t timeout)
{
    RR_UNUSED(timeout);
    if (boost::range::find(schemes, "rr+local") == schemes.end() || schemes.empty())
    {
        RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > n = RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >();
        detail::PostHandler(node, handler, n, true);
        return;
    }

    RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > o = RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >();

    NodeDirectories node_dirs = GetNode()->GetNodeDirectories();

    boost::filesystem::path private_search_dir = detail::LocalTransportUtil::GetTransportPrivateSocketPath(node_dirs);

    std::string my_username = NodeDirectoriesUtil::GetLogonUserName();
    detail::LocalTransportUtil::FindNodesInDirectory(*o, private_search_dir, "rr+local", GetNode()->NowNodeTime(),
                                                     my_username);

    // TODO: search other users
    boost::optional<boost::filesystem::path> search_path =
        detail::LocalTransportUtil::GetTransportPublicSearchPath(node_dirs);
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
                    detail::LocalTransportUtil::FindNodesInDirectory(o1, *dir_itr, "rr+local", GetNode()->NowNodeTime(),
                                                                     username1);
                    boost::range::copy(o1, std::back_inserter(*o));
                }
                catch (std::exception& exp2)
                {
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                                       "Error detecting local nodes: " << exp2.what());
                }
            }
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "Error detecting local nodes: " << exp.what());
        }
    }

    detail::PostHandler(node, handler, o, true);
}

int32_t LocalTransport::GetMaxMessageSize()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return max_message_size;
}

void LocalTransport::SetMaxMessageSize(int32_t size)
{
    if (size < 16 * 1024 || size > 100 * 1024 * 1024)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Invalid MaxMessageSize: " << size);
        throw InvalidArgumentException("Invalid maximum message size");
    }
    boost::mutex::scoped_lock lock(parameter_lock);
    max_message_size = size;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "MaxMessageSize set to " << size << " bytes");
}

bool LocalTransport::GetDisableMessage4()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return disable_message4;
}
void LocalTransport::SetDisableMessage4(bool d)
{
    boost::mutex::scoped_lock lock(parameter_lock);
    disable_message4 = d;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "DisableMessage4 set to: " << d);
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
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "DisableStringTable set to: " << d);
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
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "DisableAsyncMessageIO set to: " << d);
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
    try
    {
        discovery->Refresh();
    }
    catch (std::exception&)
    {}
#endif

#if defined(ROBOTRACONTEUR_LINUX) && !defined(ROBOTRACONTEUR_ANDROID)
    discovery = RR_MAKE_SHARED<detail::LinuxLocalTransportDiscovery>(GetNode());
    discovery->Init();
    try
    {
        discovery->Refresh();
    }
    catch (std::exception&)
    {}
#endif

#ifdef ROBOTRACONTEUR_OSX
    discovery = RR_MAKE_SHARED<detail::DarwinLocalTransportDiscovery>(GetNode());
    discovery->Init();
    try
    {
        discovery->Refresh();
    }
    catch (std::exception&)
    {}
#endif

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "Node discovery listening enabled");
}

void LocalTransport::DisableNodeDiscoveryListening()
{
    boost::mutex::scoped_lock lock(discovery_lock);
    if (discovery)
    {
        discovery->Shutdown();
    }

    discovery.reset();
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "Node discovery listening disabled");
}

void LocalTransport::LocalNodeServicesChanged()
{
    boost::mutex::scoped_lock lock(fds_lock);
    if (fds && fds->h_info_id_s)
    {
        std::string service_nonce = GetNode()->GetServiceStateNonce();

        std::map<std::string, std::string> updated_info;
        updated_info.insert(std::make_pair("ServiceStateNonce", service_nonce));
        NodeDirectoriesUtil::RefreshInfoFile(fds->h_info_id_s, updated_info);
        if (fds->h_info_name_s)
        {
            NodeDirectoriesUtil::RefreshInfoFile(fds->h_info_name_s, updated_info);
        }
    }
}

LocalTransportConnection::LocalTransportConnection(const RR_SHARED_PTR<LocalTransport>& parent, bool server,
                                                   uint32_t local_endpoint)
    : ASIOStreamBaseTransport(parent->GetNode())
{
    this->parent = parent;
    this->server = server;
    this->m_LocalEndpoint = local_endpoint;
    this->m_RemoteEndpoint = 0;

    // The heartbeat isn't as necessary in local connections because the socket
    // will be closed immediately if the other side closes or is terminated
    this->HeartbeatPeriod = 30000;
    this->ReceiveTimeout = 600000;

    this->max_message_size = parent->GetMaxMessageSize();

    this->disable_message4 = parent->GetDisableMessage4();
    this->disable_string_table = parent->GetDisableStringTable();
    this->disable_async_io = parent->GetDisableAsyncMessageIO();
}

void LocalTransportConnection::AsyncAttachSocket(
    const RR_SHARED_PTR<detail::LocalTransport_socket>& socket, const std::string& noden,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    this->socket = socket;

    NodeID target_nodeid = NodeID::GetAny();
    std::string target_nodename;
    if (noden.find('{') != std::string::npos)
    {
        target_nodeid = NodeID(noden);
    }
    else
    {
        target_nodename = noden;
    }

    ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
}

void LocalTransportConnection::MessageReceived(const RR_INTRUSIVE_PTR<Message>& m)
{

    RR_SHARED_PTR<LocalTransport> p = parent.lock();
    if (!p)
        return;

    RR_INTRUSIVE_PTR<Message> ret = p->SpecialRequest(m, shared_from_this());
    if (ret != 0)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Sending special request response");
        try
        {
            if ((m->entries.at(0)->EntryType == MessageEntryType_ConnectionTest ||
                 m->entries.at(0)->EntryType == MessageEntryType_ConnectionTestRet))
            {
                if (m->entries.at(0)->Error != MessageErrorType_None)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint, "SpecialRequest failed");
                    Close();
                    return;
                }
            }

            if ((ret->entries.at(0)->EntryType == MessageEntryType_ConnectClientRet ||
                 ret->entries.at(0)->EntryType == MessageEntryType_ReconnectClient ||
                 ret->entries.at(0)->EntryType == MessageEntryType_ConnectClientCombinedRet) &&
                ret->entries.at(0)->Error == MessageErrorType_None)
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
                        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                            node, Transport, m_LocalEndpoint,
                            "LocalTransport connection assigned LocalEndpoint: " << m_LocalEndpoint);
                    }
                }
            }

            boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h = boost::bind(
                &LocalTransportConnection::SimpleAsyncEndSendMessage,
                RR_STATIC_POINTER_CAST<LocalTransportConnection>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1));
            AsyncSendMessage(ret, h);
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                               "SpecialRequest failed: " << exp.what());
            Close();
        }

        return;
    }

    try
    {

        // TODO: fix this (maybe??)...

        /*boost::asio::ip::address addr=socket->local_endpoint().address();
        uint16_t port=socket->local_endpoint().port();

        std::string connecturl;
        if (addr.is_v4())
        {
            connecturl="local://" + addr + ":" + boost::lexical_cast<std::string>(port) + "/";
        }
        else
        {
            boost::asio::ip::address_v6 addr2=addr.to_v6();
            addr2.scope_id(0);
            connecturl="tcp://[" + addr2 + "]:" + boost::lexical_cast<std::string>(port) + "/";
        }
        */

        std::string connecturl = "rr+local:///";
        // NOLINTBEGIN(cppcoreguidelines-owning-memory)
        Transport::m_CurrentThreadTransportConnectionURL.reset(new std::string(connecturl));
        Transport::m_CurrentThreadTransport.reset(new RR_SHARED_PTR<ITransportConnection>(
            RR_STATIC_POINTER_CAST<LocalTransportConnection>(shared_from_this())));
        // NOLINTEND(cppcoreguidelines-owning-memory)
        p->MessageReceived(m);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "LocalTransport failed receiving message: " << exp.what());
        RobotRaconteurNode::TryHandleException(node, &exp);
        Close();
    }

    Transport::m_CurrentThreadTransportConnectionURL.reset(0);
    Transport::m_CurrentThreadTransport.reset(0);
}

void LocalTransportConnection::async_write_some(
    const_buffers& b,
    const boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
{
    boost::mutex::scoped_lock lock(socket_lock);
    RobotRaconteurNode::asio_async_write_some(node, socket->socket, b, handler);
}

void LocalTransportConnection::async_read_some(
    mutable_buffers& b,
    const boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
{
    boost::mutex::scoped_lock lock(socket_lock);
    RobotRaconteurNode::asio_async_read_some(node, socket->socket, b, handler);
}

size_t LocalTransportConnection::available() { return 0; }

void LocalTransportConnection::Close()
{
    boost::recursive_mutex::scoped_lock lock(close_lock);

    {
        boost::mutex::scoped_lock lock(socket_lock);

        if (!socket->socket->is_open())
        {
            return;
        }

        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, m_LocalEndpoint, "LocalTransport closing connection");

        try
        {
            socket->socket->close();
        }
        catch (std::exception&)
        {}
    }

    try
    {
        RR_SHARED_PTR<LocalTransport> p = parent.lock();
        if (p)
            p->erase_transport(RR_STATIC_POINTER_CAST<LocalTransportConnection>(shared_from_this()));
    }
    catch (std::exception&)
    {}

    ASIOStreamBaseTransport::Close();
}

void LocalTransport::CheckConnection(uint32_t endpoint)
{
    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e =
            TransportConnections.find(endpoint);
        if (e == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e->second;
    }
    t->CheckConnection(endpoint);
}

uint32_t LocalTransportConnection::GetLocalEndpoint() { return m_LocalEndpoint; }

uint32_t LocalTransportConnection::GetRemoteEndpoint() { return m_RemoteEndpoint; }

void LocalTransportConnection::CheckConnection(uint32_t endpoint)
{
    if (endpoint != m_LocalEndpoint || !connected.load())
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Connection lost");
        throw ConnectionException("Connection lost");
    }
}

RR_SHARED_PTR<Transport> LocalTransportConnection::GetTransport()
{
    RR_SHARED_PTR<Transport> p = parent.lock();
    if (!p)
        throw InvalidOperationException("Transport has been released");
    return p;
}

void LocalTransport_connected_callback2(const RR_SHARED_PTR<LocalTransport>& parent,
                                        const RR_SHARED_PTR<detail::LocalTransport_socket>& socket,
                                        const RR_SHARED_PTR<ITransportConnection>& connection,
                                        const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    RR_UNUSED(socket);
    RR_UNUSED(connection);
    if (err)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(parent->GetNode(), Transport, 0,
                                          "LocalTransport accepted socket closed with error: " << err->what());
    }
    // This is just an empty method.  The connected transport will register when it has a local endpoint.
}

void LocalTransport_attach_transport(const RR_SHARED_PTR<LocalTransport>& parent,
                                     const RR_SHARED_PTR<detail::LocalTransport_socket>& socket, bool server,
                                     uint32_t endpoint, const std::string& noden,
                                     boost::function<void(const RR_SHARED_PTR<detail::LocalTransport_socket>&,
                                                          const RR_SHARED_PTR<ITransportConnection>&,
                                                          const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    try
    {
        RR_SHARED_PTR<LocalTransportConnection> t = RR_MAKE_SHARED<LocalTransportConnection>(parent, server, endpoint);
        boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
            boost::bind(callback, socket, t, RR_BOOST_PLACEHOLDERS(_1));
        t->AsyncAttachSocket(socket, noden, h);
        parent->AddCloseListener(t, &LocalTransportConnection::Close);
    }
    catch (std::exception&)
    {
        RobotRaconteurNode::TryPostToThreadPool(
            parent->GetNode(), boost::bind(callback, RR_SHARED_PTR<detail::LocalTransport_socket>(),
                                           RR_SHARED_PTR<LocalTransportConnection>(),
                                           RR_MAKE_SHARED<ConnectionException>("Could not connect to service")));
    }
}

namespace detail
{
namespace LocalTransportUtil
{
std::string GetLogonUserName()
{
#ifdef ROBOTRACONTEUR_WINDOWS
    DWORD size = 0;
    GetUserName(NULL, &size);

    boost::scoped_array<char> n(new char[size]);

    BOOL res = GetUserName(n.get(), &size);
    if (!res)
        throw ConnectionException("Could not determine current user");

    return std::string(n.get());
#else
    struct passwd* passwd = NULL;
    passwd = getpwuid(getuid());
    if (passwd == NULL)
        throw ConnectionException("Could not determine current user");

    std::string o(passwd->pw_name);
    if (o.empty())
        throw ConnectionException("Could not determine current user");

    return o;

#endif
}

boost::filesystem::path GetTransportPrivateSocketPath(const NodeDirectories& node_dirs)
{
    boost::filesystem::path user_run_path = node_dirs.user_run_dir;
    boost::filesystem::path path = user_run_path / "transport" / "local";

    boost::filesystem::path bynodeid_path = path / "by-nodeid";
    boost::filesystem::path bynodename_path = path / "by-nodename";
    boost::filesystem::path socket_path1 = path / "socket";
    boost::filesystem::path socket_path2 = user_run_path / "socket";

    boost::system::error_code ec1;
    boost::filesystem::create_directories(bynodeid_path, ec1);
    if (ec1)
        throw SystemResourceException("Could not activate system for local transport");

    boost::system::error_code ec2;
    boost::filesystem::create_directories(bynodename_path, ec2);
    if (ec2)
        throw SystemResourceException("Could not activate system for local transport");

    boost::system::error_code ec3;
    boost::filesystem::create_directories(socket_path1, ec3);
    if (ec3)
        throw SystemResourceException("Could not activate system for local transport");

    boost::system::error_code ec4;
    boost::filesystem::create_directories(socket_path2, ec4);
    if (ec4)
        throw SystemResourceException("Could not activate system for local transport");

    return path;
}

boost::optional<boost::filesystem::path> GetTransportPublicSocketPath(const NodeDirectories& node_dirs)
{
    boost::filesystem::path path1;
    if (boost::optional<boost::filesystem::path> path1_1 = GetTransportPublicSearchPath(node_dirs))
    {
        path1 = *path1_1;
    }
    else
    {
        return boost::optional<boost::filesystem::path>();
    }

    std::string username = GetLogonUserName();

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
    if (ec1)
        return boost::optional<boost::filesystem::path>();

    boost::system::error_code ec2;
    boost::filesystem::create_directories(bynodename_path, ec2);
    if (ec2)
        return boost::optional<boost::filesystem::path>();

    boost::system::error_code ec3;
    boost::filesystem::create_directories(socket_path, ec3);
    if (ec3)
        return boost::optional<boost::filesystem::path>();

#ifdef ROBOTRACONTEUR_WINDOWS
        // TODO: file permissions?
#else
    struct stat info = {};
    if (stat(path.string().c_str(), &info) < 0)
    {
        return boost::optional<boost::filesystem::path>();
    }

    uid_t my_uid = getuid();

    if (chmod(bynodeid_path.string().c_str(), S_ISGID | S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP) < 0)
    {
        throw SystemResourceException("Could not activate system for local transport");
    }
    if (chown(bynodeid_path.string().c_str(), my_uid, info.st_gid) < 0)
    {
        throw SystemResourceException("Could not activate system for local transport");
    }
    if (chmod(bynodename_path.string().c_str(), S_ISGID | S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP) < 0)
    {
        throw SystemResourceException("Could not activate system for local transport");
    }
    if (chown(bynodename_path.string().c_str(), my_uid, info.st_gid) < 0)
    {
        throw SystemResourceException("Could not activate system for local transport");
    }
    if (chmod(socket_path.string().c_str(), S_ISGID | S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP) < 0)
    {
        throw SystemResourceException("Could not activate system for local transport");
    }
    if (chown(socket_path.string().c_str(), my_uid, info.st_gid) < 0)
    {
        throw SystemResourceException("Could not activate system for local transport");
    }
#endif

    return path;
}

boost::optional<boost::filesystem::path> GetTransportPublicSearchPath(const NodeDirectories& node_dirs)
{
#ifdef ROBOTRACONTEUR_WINDOWS
    std::string username = GetLogonUserName();

    boost::filesystem::path path1 = node_dirs.system_run_dir;
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
    if (!GetFileSecurityW(path1.c_str(), OWNER_SECURITY_INFORMATION, (PSECURITY_DESCRIPTOR)security.get(),
                          security_size, &security_size))
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

    path1 /= "transport";
    path1 /= "local";

#else
    boost::filesystem::path path1 = node_dirs.system_run_dir / "transport/local";
#endif

    if (!boost::filesystem::is_directory(path1))
    {
        return boost::optional<boost::filesystem::path>();
    }

    return path1;
}

void FindNodesInDirectory(std::vector<NodeDiscoveryInfo>& nodeinfo, const boost::filesystem::path& path,
                          boost::string_ref scheme, const boost::posix_time::ptime& now,
                          boost::optional<std::string> username)
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
            if (!NodeDirectoriesUtil::ReadInfoFile(*dir_itr, info))
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

            NodeID nodeid(nodeid1->second);
            std::string& username2 = username1->second;

            std::string url;
            if (username)
            {
                if (username2 != *username)
                    continue;
                url = scheme.to_string() + "://" + *username + "@localhost/?nodeid=" + nodeid.ToString("D") +
                      "&service=RobotRaconteurServiceIndex";
            }
            else
            {
                url =
                    scheme.to_string() + ":///?nodeid=" + nodeid.ToString("D") + "&service=RobotRaconteurServiceIndex";
            }

            NodeDiscoveryInfo i;
            i.NodeID = nodeid;
            i.NodeName.clear();
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
        catch (std::exception&)
        {}
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
            if (!NodeDirectoriesUtil::ReadInfoFile(*dir_itr, info))
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

            NodeID nodeid(nodeid1->second);

            if (nodename1->second != boost::filesystem::change_extension(dir_itr->path().filename(), ""))
            {
                // TODO: Test?
                continue;
            }

            BOOST_FOREACH (NodeDiscoveryInfo& e1, nodeinfo)
            {
                if (e1.NodeID == nodeid)
                {
                    if (e1.NodeName.empty())
                    {
                        e1.NodeName = nodename1->second;
                    }
                }
            }
        }
        catch (std::exception&)
        {}
    }
}

RR_SHARED_PTR<detail::LocalTransport_socket> FindAndConnectLocalSocket(
    const ParseConnectionURLResult& url, const std::vector<boost::filesystem::path>& search_paths,
    const std::vector<std::string>& usernames, RR_BOOST_ASIO_IO_CONTEXT& _io_context_)
{
    RR_UNUSED(usernames);

    RR_SHARED_PTR<detail::LocalTransport_socket> socket;

    BOOST_FOREACH (const boost::filesystem::path& e, search_paths)
    {
        std::map<std::string, std::string> info_data;
        if (!url.nodeid.IsAnyNode())
        {
            boost::filesystem::path e2 = e / "by-nodeid";
            e2 /= url.nodeid.ToString("D") + ".info";

            if (!NodeDirectoriesUtil::ReadInfoFile(e2, info_data))
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
                if (!NodeDirectoriesUtil::ReadInfoFile(e3, info_data2))
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

            if (!NodeDirectoriesUtil::ReadInfoFile(e2, info_data))
            {
                continue;
            }
        }

        std::map<std::string, std::string>::iterator e2 = info_data.find("socket");
        if (e2 == info_data.end())
            continue;

        std::string pipename = e2->second;

        socket = RR_SHARED_PTR<detail::LocalTransport_socket>(new detail::LocalTransport_socket(_io_context_));
        boost::asio::local::stream_protocol::endpoint ep(pipename);
        boost::system::error_code ec;
        socket->socket->connect(ep, ec);
        if (ec)
        {
            socket.reset();
            continue;
        }
        break;
    }

    // TODO: Check user on unix
    if (!socket)
        return RR_SHARED_PTR<detail::LocalTransport_socket>();

    return socket;
}
} // namespace LocalTransportUtil

LocalTransportDiscovery::LocalTransportDiscovery(const RR_SHARED_PTR<RobotRaconteurNode>& node) { this->node = node; }

void LocalTransportDiscovery::Refresh()
{
    RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
    if (!node1)
        return;

    boost::posix_time::ptime now = node1->NowNodeTime();

    if (private_path)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(
            node, Transport, -1, "LocalTransport discovery searching private path \"" << *private_path << "\"");

        std::vector<NodeDiscoveryInfo> nodeinfo;
        LocalTransportUtil::FindNodesInDirectory(nodeinfo, *private_path, "rr+local", now,
                                                 boost::optional<std::string>());
        BOOST_FOREACH (NodeDiscoveryInfo& n, nodeinfo)
        {
            node1->NodeDetected(n);
        }
    }

    if (public_path)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                           "LocalTransport discovery searching public path \"" << *public_path << "\"");
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
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                                       "LocalTransport discovery searching public user path \""
                                                           << *dir_itr << "\"");
                    BOOST_FOREACH (NodeDiscoveryInfo& n, nodeinfo)
                    {
                        node1->NodeDetected(n);
                    }
                }
                catch (std::exception&)
                {}
            }
        }
        catch (std::exception&)
        {}
    }
}

} // namespace detail

} // namespace RobotRaconteur
