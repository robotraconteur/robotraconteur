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

#include "LocalTransport_linux_private.h"
#include "LocalTransport_private.h"

#include "sys/inotify.h"
#include <boost/foreach.hpp>

namespace RobotRaconteur
{
namespace detail
{

static int LinuxLocalTransportDiscovery_add_watch(int notify_fd, const boost::filesystem::path& path)
{
    return inotify_add_watch(notify_fd, path.c_str(),
                             IN_ATTRIB | IN_MODIFY | IN_DELETE | IN_DELETE_SELF | IN_ONLYDIR | IN_CREATE);
}

LinuxLocalTransportDiscovery_dir::LinuxLocalTransportDiscovery_dir()
{
    notify_fd = -1;
    dir_wd = -1;
    bynodeid_wd = -1;
    bynodename_wd = -1;
}

LinuxLocalTransportDiscovery_dir::~LinuxLocalTransportDiscovery_dir() {}

bool LinuxLocalTransportDiscovery_dir::Init(const boost::filesystem::path& path)
{
    this->path = path;
    int notify_fd1 = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
    if (notify_fd1 < 0)
    {
        return false;
    }

    int dir_wd1 = LinuxLocalTransportDiscovery_add_watch(notify_fd1, (path).c_str());

    if (!dir_wd1)
    {
        close(notify_fd1);
        return false;
    }

    int bynodeid_wd1 = LinuxLocalTransportDiscovery_add_watch(notify_fd1, (path / "by-nodeid").c_str());
    int bynodename_wd1 = LinuxLocalTransportDiscovery_add_watch(notify_fd1, (path / "by-nodename").c_str());

    notify_fd = notify_fd1;
    dir_wd = dir_wd1;
    if (bynodeid_wd1 > 0)
    {
        bynodeid_wd = bynodeid_wd1;
    }
    if (bynodename_wd1 > 0)
    {
        bynodename_wd = bynodename_wd1;
    }

    return true;
}

bool LinuxLocalTransportDiscovery_dir::Refresh()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event* event = NULL;

    if (notify_fd < 0)
        return false;

    if (dir_wd < 0)
    {
        int dir_wd1 = LinuxLocalTransportDiscovery_add_watch(notify_fd, path.c_str());
        if (dir_wd1 < 0)
            return false;
        dir_wd = dir_wd1;
    }

    if (bynodeid_wd < 0)
    {
        int bynodeid_wd1 = LinuxLocalTransportDiscovery_add_watch(notify_fd, (path / "by-nodeid").c_str());
        if (bynodeid_wd1 > 0)
        {
            bynodeid_wd = bynodeid_wd1;
        }
    }

    if (bynodename_wd < 0)
    {
        int bynodename_wd1 = LinuxLocalTransportDiscovery_add_watch(notify_fd, (path / "by-nodename").c_str());
        if (bynodename_wd1 > 0)
        {
            bynodename_wd = bynodename_wd1;
        }
    }

    ssize_t len = read(notify_fd, buf, sizeof(buf));
    if (len == -1 && errno != EAGAIN)
    {
        return errno == EAGAIN;
    }

    if (len == 0)
        return true;

    for (char* ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len)
    {
        event = reinterpret_cast<const struct inotify_event*>(ptr);

        if (event->mask & IN_DELETE_SELF)
        {
            if (event->wd == dir_wd)
            {
                return false;
            }

            if (event->wd == bynodeid_wd)
            {
                bynodeid_wd = -1;
            }

            if (event->wd == bynodename_wd)
            {
                bynodename_wd = -1;
            }
        }
    }

    return true;
}

LinuxLocalTransportDiscovery::LinuxLocalTransportDiscovery(const RR_SHARED_PTR<RobotRaconteurNode>& node)
    : LocalTransportDiscovery(node)
{
    public_wd = -1;
}

void LinuxLocalTransportDiscovery::Init()
{
    shutdown_evt = RR_MAKE_SHARED<LocalTransportUtil::FD>(eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK));
    if (shutdown_evt->fd() < 0)
        throw InternalErrorException("Internal error");

    public_evt = RR_MAKE_SHARED<LocalTransportUtil::FD>(inotify_init1(IN_NONBLOCK | IN_CLOEXEC));
    if (public_evt->fd() < 0)
        throw InternalErrorException("Internal error");

    poll_thread = boost::thread(boost::bind(&LinuxLocalTransportDiscovery::run, shared_from_this()));
}

void LinuxLocalTransportDiscovery::Shutdown()
{
    int ret = 0;
    do
    {
        ret = eventfd_write(shutdown_evt->fd(), 1);
    } while (ret < 0 && errno == EINTR);

    poll_thread.join();
}

LinuxLocalTransportDiscovery::~LinuxLocalTransportDiscovery() {}

void LinuxLocalTransportDiscovery::run()
{
    while (true)
    {
        bool refresh_now = false;

        if (!private_dir)
        {
            try
            {
                private_path = LocalTransportUtil::GetTransportPrivateSocketPath();
                if (private_path)
                {
                    RR_SHARED_PTR<LinuxLocalTransportDiscovery_dir> private_dir1 =
                        RR_MAKE_SHARED<LinuxLocalTransportDiscovery_dir>();
                    if (private_dir1->Init(*private_path))
                    {
                        private_dir = private_dir1;
                        refresh_now = true;
                    }
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                                       "LocalTransport discovery watching private path \""
                                                           << *private_path << "\"")
                }
            }
            catch (std::exception&)
            {}
        }

        if (update_public())
        {
            refresh_now = true;
        }

        if (public_path)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(
                node, Transport, -1, "LocalTransport discovery watching public path \"" << *public_path << "\"")
        }

        std::vector<struct pollfd> poll_fds;

        struct pollfd shutdown_evt_poll = {};
        shutdown_evt_poll.fd = shutdown_evt->fd();
        shutdown_evt_poll.events = POLLIN | POLLERR | POLLRDHUP;
        shutdown_evt_poll.revents = 0;
        poll_fds.push_back(shutdown_evt_poll);

        if (private_dir)
        {
            struct pollfd private_dir_poll = {};
            private_dir_poll.fd = private_dir->notify_fd;
            private_dir_poll.events = POLLIN | POLLERR | POLLRDHUP;
            private_dir_poll.revents = 0;
            poll_fds.push_back(private_dir_poll);
        }

        if (public_evt && public_evt->fd() > 0)
        {
            struct pollfd public_evt_poll = {};
            public_evt_poll.fd = public_evt->fd();
            public_evt_poll.events = POLLIN | POLLERR | POLLRDHUP;
            public_evt_poll.revents = 0;
            poll_fds.push_back(public_evt_poll);
        }

        for (std::map<std::string, RR_SHARED_PTR<LinuxLocalTransportDiscovery_dir> >::iterator e =
                 public_user_dirs.begin();
             e != public_user_dirs.end(); e++)
        {
            if (!e->second)
                continue;
            if (e->second->notify_fd > 0)
            {
                struct pollfd public_dir_poll = {};
                public_dir_poll.fd = e->second->notify_fd;
                public_dir_poll.events = POLLIN | POLLERR | POLLRDHUP;
                public_dir_poll.revents = 0;
                poll_fds.push_back(public_dir_poll);
            }
        }

        int ret = 0;

        if (!refresh_now)
        {
            ret = poll(&poll_fds[0], poll_fds.size(), 55000);

            if (ret < 0 && errno != EINTR)
                return;

            struct pollfd shutdown_evt_poll2 = {};
            shutdown_evt_poll2.fd = shutdown_evt->fd();
            shutdown_evt_poll2.events = POLLIN | POLLERR | POLLRDHUP;
            shutdown_evt_poll2.revents = 0;

            ret = poll(&shutdown_evt_poll2, 1, 250);

            if (ret < 0 && errno != EINTR)
                return;
        }

        eventfd_t shutdown_evt_val = 0;
        if (eventfd_read(shutdown_evt->fd(), &shutdown_evt_val) >= 0)
        {
            return;
        }
        errno = 0;

        if (private_dir)
        {
            if (!private_dir->Refresh())
            {
                private_dir.reset();
                private_path.reset();
            }
        }

        refresh_public();

        Refresh();
    }
}

bool LinuxLocalTransportDiscovery::update_public()
{
    bool refresh_now = false;

    if (public_wd < 0)
    {
        try
        {
            public_path = LocalTransportUtil::GetTransportPublicSearchPath();
            if (public_path)
            {
                public_wd = LinuxLocalTransportDiscovery_add_watch(public_evt->fd(), *public_path);
                if (public_wd < 0)
                {
                    public_path.reset();
                }
                else
                {
                    refresh_now = true;
                }
            }
        }
        catch (std::exception&)
        {}

        if (public_wd > 0)
        {
            try
            {
                if (public_path)
                {
                    if (boost::filesystem::exists(*public_path))
                    {
                        for (boost::filesystem::directory_iterator dir_itr(*public_path);
                             dir_itr != boost::filesystem::directory_iterator(); dir_itr++)
                        {
                            try
                            {
                                boost::filesystem::path new_path = *dir_itr;
                                std::string username = new_path.filename().string();
                                RR_SHARED_PTR<LinuxLocalTransportDiscovery_dir> d =
                                    RR_MAKE_SHARED<LinuxLocalTransportDiscovery_dir>();
                                if (d->Init(new_path))
                                {
                                    public_user_dirs.insert(std::make_pair(username, d));
                                    refresh_now = true;
                                }
                            }
                            catch (std::exception&)
                            {}
                        }
                    }
                }
            }
            catch (std::exception&)
            {}
        }
    }

    return refresh_now;
}

void LinuxLocalTransportDiscovery::refresh_public()
{
    if (public_wd > 0)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
        char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
        const struct inotify_event* event = NULL;

        ssize_t len = read(public_evt->fd(), buf, sizeof(buf));
        if (len > 0)
        {
            for (char* ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len)
            {
                event = reinterpret_cast<const struct inotify_event*>(ptr);

                if ((event->mask & IN_DELETE_SELF))
                {
                    if (event->wd == public_wd)
                    {
                        public_wd = -1;
                    }
                }

                if (event->mask & IN_CREATE)
                {
                    std::string username(event->name);
                    boost::filesystem::path new_path(*public_path / username);
                    if (boost::filesystem::is_directory(new_path))
                    {
                        std::string username = new_path.filename().string();
                        RR_SHARED_PTR<LinuxLocalTransportDiscovery_dir> d =
                            RR_MAKE_SHARED<LinuxLocalTransportDiscovery_dir>();
                        if (d->Init(new_path))
                        {
                            std::map<std::string, RR_SHARED_PTR<LinuxLocalTransportDiscovery_dir> >::iterator e_find =
                                public_user_dirs.find(username);
                            if (e_find != public_user_dirs.end())
                            {
                                public_user_dirs.erase(e_find);
                            }

                            public_user_dirs.insert(std::make_pair(username, d));
                        }
                    }
                }
            }
        }
    }

    std::list<std::string> delete_users;

    for (std::map<std::string, RR_SHARED_PTR<LinuxLocalTransportDiscovery_dir> >::iterator e = public_user_dirs.begin();
         e != public_user_dirs.end(); e++)
    {
        if (!e->second)
        {
            delete_users.push_back(e->first);
            continue;
        }
        if (!e->second->Refresh())
        {
            delete_users.push_back(e->first);
        }
    }

    BOOST_FOREACH (std::string& e, delete_users)
    {
        public_user_dirs.erase(e);
    }
}

} // namespace detail

} // namespace RobotRaconteur
