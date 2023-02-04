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

#pragma once

#define BOOST_ASIO_HAS_LOCAL_SOCKETS 1

#include "RobotRaconteur/LocalTransport.h"
#include "RobotRaconteur/ASIOStreamBaseTransport.h"
#include <boost/filesystem.hpp>
#include <boost/call_traits.hpp>
#include <set>

#ifdef ROBOTRACONTEUR_WINDOWS
#include "LocalTransport_win_private.h"
#endif

#ifdef ROBOTRACONTEUR_LINUX
#include "LocalTransport_linux_private.h"
#endif

#ifdef ROBOTRACONTEUR_OSX
#include "LocalTransport_darwin_private.h"
#endif

#include "NodeDirectories_private.h"

namespace RobotRaconteur
{

namespace detail
{
namespace LocalTransportUtil
{
boost::filesystem::path GetTransportPrivateSocketPath(const NodeDirectories& node_dirs);

boost::optional<boost::filesystem::path> GetTransportPublicSocketPath(const NodeDirectories& node_dirs);

boost::optional<boost::filesystem::path> GetTransportPublicSearchPath(const NodeDirectories& node_dirs);

void FindNodesInDirectory(std::vector<NodeDiscoveryInfo>& nodeinfo, const boost::filesystem::path& path,
                          boost::string_ref scheme, const boost::posix_time::ptime& now,
                          boost::optional<std::string> username = boost::optional<std::string>());

RR_SHARED_PTR<detail::LocalTransport_socket> FindAndConnectLocalSocket(
    const ParseConnectionURLResult& url, const std::vector<boost::filesystem::path>& search_paths,
    const std::vector<std::string>& usernames, RR_BOOST_ASIO_IO_CONTEXT& io_service_);

#ifndef ROBOTRACONTEUR_WINDOWS

class FD
{
  public:
    int _fd;

    FD(int f) { _fd = f; }

    int fd() const { return _fd; };

    ~FD() { close(_fd); }
};
#endif

} // namespace LocalTransportUtil
} // namespace detail

class LocalTransportConnection : public detail::ASIOStreamBaseTransport
{
  public:
    LocalTransportConnection(const RR_SHARED_PTR<LocalTransport>& parent, bool server, uint32_t local_endpoint);

    void AsyncAttachSocket(const RR_SHARED_PTR<detail::LocalTransport_socket>& socket, const std::string& noden,
                           const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

    RR_OVIRTUAL void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;

  protected:
    RR_OVIRTUAL void async_write_some(
        const_buffers& b,
        const boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
        RR_OVERRIDE;

    RR_OVIRTUAL void async_read_some(
        mutable_buffers& b,
        const boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
        RR_OVERRIDE;

    RR_OVIRTUAL size_t available() RR_OVERRIDE;

  public:
    RR_OVIRTUAL void Close() RR_OVERRIDE;

    RR_OVIRTUAL uint32_t GetLocalEndpoint() RR_OVERRIDE;

    RR_OVIRTUAL uint32_t GetRemoteEndpoint() RR_OVERRIDE;

    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE;

    RR_OVIRTUAL RR_SHARED_PTR<Transport> GetTransport() RR_OVERRIDE;

  protected:
    RR_SHARED_PTR<detail::LocalTransport_socket> socket;

    boost::mutex socket_lock;
    bool server;

    RR_WEAK_PTR<LocalTransport> parent;

    uint32_t m_RemoteEndpoint;
    uint32_t m_LocalEndpoint;

    boost::recursive_mutex close_lock;
};

void LocalTransport_attach_transport(const RR_SHARED_PTR<LocalTransport>& parent,
                                     const RR_SHARED_PTR<detail::LocalTransport_socket>& socket, bool server,
                                     uint32_t endpoint, const std::string& noden,
                                     boost::function<void(const RR_SHARED_PTR<detail::LocalTransport_socket>&,
                                                          const RR_SHARED_PTR<ITransportConnection>&,
                                                          const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

void LocalTransport_connected_callback2(const RR_SHARED_PTR<LocalTransport>& parent,
                                        const RR_SHARED_PTR<detail::LocalTransport_socket>& socket,
                                        const RR_SHARED_PTR<ITransportConnection>& connection,
                                        const RR_SHARED_PTR<RobotRaconteurException>& err);

namespace detail
{
template <typename T>
class LocalTransportNodeLock
{
  public:
    static std::set<T> nodeids;
    static boost::mutex nodeids_lock;

    static RR_SHARED_PTR<LocalTransportNodeLock<T> > Lock(typename boost::call_traits<T>::param_type id)
    {
        boost::mutex::scoped_lock lock(nodeids_lock);
        std::pair<typename std::set<T>::iterator, bool> res = nodeids.insert(id);
        if (!res.second)
        {
            return RR_SHARED_PTR<LocalTransportNodeLock<T> >();
        }
        else
        {
            RR_SHARED_PTR<LocalTransportNodeLock<T> > o = RR_MAKE_SHARED<LocalTransportNodeLock<T> >();
            o->release_id = id;
            return o;
        }
    }

    T release_id;
    ~LocalTransportNodeLock()
    {
        try
        {
            boost::mutex::scoped_lock lock(nodeids_lock);

            nodeids.erase(release_id);
        }
        catch (std::exception& e)
        {}
    }
};

template <typename T>
typename std::set<T> LocalTransportNodeLock<T>::nodeids;
template <typename T>
boost::mutex LocalTransportNodeLock<T>::nodeids_lock;

typedef LocalTransportNodeLock<NodeID> LocalTransportNodeIDLock;
typedef LocalTransportNodeLock<std::string> LocalTransportNodeNameLock;

class LocalTransportFDs
{
  public:
    boost::shared_ptr<NodeDirectoriesFD> h_nodename_s;
    boost::shared_ptr<NodeDirectoriesFD> h_pid_id_s;
    boost::shared_ptr<NodeDirectoriesFD> h_info_id_s;
    boost::shared_ptr<NodeDirectoriesFD> h_pid_name_s;
    boost::shared_ptr<NodeDirectoriesFD> h_info_name_s;
    boost::shared_ptr<LocalTransportNodeIDLock> nodeid_lock;
    boost::shared_ptr<LocalTransportNodeNameLock> nodename_lock;

    LocalTransportFDs() {}

    ~LocalTransportFDs() {}
};

} // namespace detail

} // namespace RobotRaconteur
