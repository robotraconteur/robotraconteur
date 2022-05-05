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

namespace RobotRaconteur
{

namespace detail
{
class LocalTransportFD;

namespace LocalTransportUtil
{
std::string GetLogonUserName();

boost::filesystem::path GetUserDataPath();

boost::filesystem::path GetUserRunPath();

boost::filesystem::path GetUserNodeIDPath();

boost::filesystem::path GetTransportPrivateSocketPath();

boost::optional<boost::filesystem::path> GetTransportPublicSocketPath();

boost::optional<boost::filesystem::path> GetTransportPublicSearchPath();

bool ReadInfoFile(const boost::filesystem::path& fname, std::map<std::string, std::string>& data);

boost::tuple<NodeID, RR_SHARED_PTR<LocalTransportFD> > GetNodeIDForNodeNameAndLock(boost::string_ref nodename);

RR_SHARED_PTR<LocalTransportFD> CreatePidFile(const boost::filesystem::path& path, bool for_name = false);
RR_SHARED_PTR<LocalTransportFD> CreateInfoFile(const boost::filesystem::path& path,
                                               std::map<std::string, std::string> info, bool for_name = false);
void RefreshInfoFile(RR_SHARED_PTR<LocalTransportFD> h_info, boost::string_ref service_nonce);

void FindNodesInDirectory(std::vector<NodeDiscoveryInfo>& nodeinfo, const boost::filesystem::path& path,
                          boost::string_ref scheme, const boost::posix_time::ptime& now,
                          boost::optional<std::string> username = boost::optional<std::string>());

RR_SHARED_PTR<detail::LocalTransport_socket> FindAndConnectLocalSocket(
    ParseConnectionURLResult url, const std::vector<boost::filesystem::path>& search_paths,
    const std::vector<std::string>& usernames, RR_BOOST_ASIO_IO_CONTEXT& io_service_);

#ifndef ROBOTRACONTEUR_WINDOWS

class FD
{
  public:
    int _fd;

    FD(int f) { _fd = f; }

    int fd() { return _fd; }

    ~FD() { close(_fd); }
};
#endif

} // namespace LocalTransportUtil
} // namespace detail

class LocalTransportConnection : public detail::ASIOStreamBaseTransport
{
  public:
    LocalTransportConnection(RR_SHARED_PTR<LocalTransport> parent, bool server, uint32_t local_endpoint);

    void AsyncAttachSocket(RR_SHARED_PTR<detail::LocalTransport_socket> socket, std::string noden,
                           boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m);

  protected:
    virtual void async_write_some(
        const_buffers& b,
        boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler);

    virtual void async_read_some(
        mutable_buffers& b,
        boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler);

    virtual size_t available();

  public:
    virtual void Close();

    virtual uint32_t GetLocalEndpoint();

    virtual uint32_t GetRemoteEndpoint();

    virtual void CheckConnection(uint32_t endpoint);

    virtual RR_SHARED_PTR<Transport> GetTransport();

  protected:
    RR_SHARED_PTR<detail::LocalTransport_socket> socket;

    boost::mutex socket_lock;
    bool server;

    RR_WEAK_PTR<LocalTransport> parent;

    uint32_t m_RemoteEndpoint;
    uint32_t m_LocalEndpoint;

    boost::recursive_mutex close_lock;
};

void LocalTransport_attach_transport(
    RR_SHARED_PTR<LocalTransport> parent, RR_SHARED_PTR<detail::LocalTransport_socket> socket, bool server,
    uint32_t endpoint, std::string noden,
    boost::function<void(RR_SHARED_PTR<detail::LocalTransport_socket>, RR_SHARED_PTR<ITransportConnection>,
                         RR_SHARED_PTR<RobotRaconteurException>)>& callback);

void LocalTransport_connected_callback2(RR_SHARED_PTR<LocalTransport> parent,
                                        RR_SHARED_PTR<detail::LocalTransport_socket> socket,
                                        RR_SHARED_PTR<ITransportConnection> connection,
                                        RR_SHARED_PTR<RobotRaconteurException> err);

namespace detail
{
class LocalTransportFD
{
  public:
    boost::mutex this_lock;

    std::map<std::string, std::string> info;

#ifdef ROBOTRACONTEUR_WINDOWS
    HANDLE fd;
#else
    int fd;
#endif

    LocalTransportFD();

    ~LocalTransportFD();

    void open_read(const boost::filesystem::path& path, boost::system::error_code& err);
    void open_lock_write(const boost::filesystem::path& path, bool delete_on_close, boost::system::error_code& err);
    // void reopen_lock_write(bool delete_on_close, boost::system::error_code& err);

    bool read(std::string& data);

    bool read_info();

    bool write(boost::string_ref data);

    bool write_info();

    bool reset();

    size_t file_len();
};

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
        boost::mutex::scoped_lock lock(nodeids_lock);
        nodeids.erase(release_id);
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
    boost::shared_ptr<LocalTransportFD> h_nodename_s;
    boost::shared_ptr<LocalTransportFD> h_pid_id_s;
    boost::shared_ptr<LocalTransportFD> h_info_id_s;
    boost::shared_ptr<LocalTransportFD> h_pid_name_s;
    boost::shared_ptr<LocalTransportFD> h_info_name_s;
    boost::shared_ptr<LocalTransportNodeIDLock> nodeid_lock;
    boost::shared_ptr<LocalTransportNodeNameLock> nodename_lock;

    LocalTransportFDs() {}

    ~LocalTransportFDs() {}
};

} // namespace detail

} // namespace RobotRaconteur
