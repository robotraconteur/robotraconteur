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

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "HardwareTransport_private.h"
#include "RobotRaconteur/Client.h"
#include "RobotRaconteur/Service.h"
#include <boost/locale.hpp>

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
#include <boost/tuple/tuple_comparison.hpp>

#ifdef ROBOTRACONTEUR_WINDOWS
#include "HardwareTransport_win_private.h"
#endif

#if defined(ROBOTRACONTEUR_LINUX) && !defined(ROBOTRACONTEUR_ANDROID)
#include "HardwareTransport_linux_private.h"
#endif

#ifdef ROBOTRACONTEUR_ANDROID
//#include "HardwareTransport_android_private.h"
#endif

#include <boost/locale.hpp>

namespace RobotRaconteur
{

HardwareTransport::HardwareTransport(RR_SHARED_PTR<RobotRaconteurNode> node) : Transport(node)
{
    if (!node)
        throw InvalidArgumentException("Node cannot be null");

    transportopen = false;
    this->node = node;

    this->max_message_size = 12 * 1024 * 1024;

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

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "HardwareTransport created");

#ifdef ROBOTRACONTEUR_WINDOWS
    RR_SHARED_PTR<SetupApi_Functions> f = RR_MAKE_SHARED<SetupApi_Functions>();
    if (f->LoadFunctions())
    {
        internal1 = f;
    }
    else
    {
        ROBOTRACONTEUR_LOG_WARNING_COMPONENT(node, Transport, -1,
                                             "HardwareTransport could not load SetupApi functions");
    }
#endif

#if defined(ROBOTRACONTEUR_LINUX) && !defined(ROBOTRACONTEUR_ANDROID)
    RR_SHARED_PTR<detail::DBus_Functions> f1 = RR_MAKE_SHARED<detail::DBus_Functions>();
    if (f1->LoadFunctions())
    {
        internal1 = f1;
    }
    else
    {
        ROBOTRACONTEUR_LOG_WARNING_COMPONENT(node, Transport, -1, "HardwareTransport could not load DBus functions");
    }

    RR_SHARED_PTR<detail::Sdp_Functions> f4 = RR_MAKE_SHARED<detail::Sdp_Functions>();
    if (f4->LoadFunctions())
    {
        internal4 = f4;
    }
    else
    {
        ROBOTRACONTEUR_LOG_WARNING_COMPONENT(node, Transport, -1, "HardwareTransport could not load Sdp functions");
    }

#endif
}

HardwareTransport::~HardwareTransport() {}

bool HardwareTransport::IsServer() const { return false; }

bool HardwareTransport::IsClient() const { return true; }

std::string HardwareTransport::GetUrlSchemeString() const { return "rr+usb"; }

bool HardwareTransport::CanConnectService(boost::string_ref url)
{

    if (boost::starts_with(url, "rr+usb://"))
        return true;

    if (boost::starts_with(url, "rr+pci://"))
        return true;

    if (boost::starts_with(url, "rr+industrial://"))
        return true;

    if (boost::starts_with(url, "rr+bluetooth://"))
        return true;

    return false;
}

void HardwareTransport::AsyncCreateTransportConnection(
    boost::string_ref url, RR_SHARED_PTR<Endpoint> ep,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                      "HardwareTransport begin create transport connection with URL: " << url);

    try
    {
        ParseConnectionURLResult url_res = ParseConnectionURL(url);
        if (url_res.nodename.empty() && url_res.nodeid.IsAnyNode())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                node, Transport, ep->GetLocalEndpoint(),
                "HardwareTransport NodeID and/or NodeName not specified in URL: " << url);
            throw ConnectionException("NodeID and/or NodeName must be specified for HardwareTransport");
        }

        std::string noden;

        if (!(url_res.nodeid.IsAnyNode() && url_res.nodename != ""))
        {
            noden = url_res.nodeid.ToString();
        }
        else
        {
            noden = url_res.nodename;
        }

        std::string transport;

        std::string host = url_res.host;
        if (url_res.port != -1)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                               "HardwareTransport must not contain port, invalid URL: " << url);
            throw ConnectionException("Invalid URL for hardware transport");
        }
        if (url_res.path != "" && url_res.path != "/")
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                               "HardwareTransport must not contain a path, invalid URL: " << url);
            throw ConnectionException("Invalid URL for hardware transport");
        }

        if (host != "localhost" && host != "")
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                node, Transport, ep->GetLocalEndpoint(),
                "HardwareTransport host must be empty or \"localhost\", invalid URL: " << url);
            throw ConnectionException("Invalid host for hardware transport");
        }

        if (url_res.scheme == "rr+usb")
        {
            transport = "usb";
        }
        else if (url_res.scheme == "rr+pci")
        {
            transport = "pci";
        }
        else if (url_res.scheme == "rr+industrial")
        {
            transport = "industrial";
        }
        else if (url_res.scheme == "rr+bluetooth")
        {
            transport = "bluetooth";
        }
        else
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                               "LocalTransport invalid transport for URL: " << url);
            throw ConnectionException("Invalid connection transport");
        }

        RR_SHARED_PTR<HardwareTransportConnection_driver::socket_type> socket;

#ifdef ROBOTRACONTEUR_WINDOWS

        if (transport == "usb")
        {
            boost::optional<std::wstring> win_path =
                detail::HardwareTransport_win_find_usb(internal1, url_res.nodeid, url_res.nodename);
            if (!win_path)
            {
                // If we don't find the USB device using driver interface, attempt to find it using WinUSB.
                RR_SHARED_PTR<detail::WinUsbDeviceManager> m;
                {
                    boost::mutex::scoped_lock lock(parameter_lock);
                    if (!internal2)
                    {
                        RR_SHARED_PTR<SetupApi_Functions> f1 = RR_STATIC_POINTER_CAST<SetupApi_Functions>(internal1);
                        m = RR_MAKE_SHARED<detail::WinUsbDeviceManager>(shared_from_this(), f1);
                        internal2 = m;
                    }
                    else
                    {
                        m = RR_STATIC_POINTER_CAST<detail::WinUsbDeviceManager>(internal2);
                    }

                    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>
                        h = boost::bind(&HardwareTransport::AsyncCreateTransportConnection2, shared_from_this(), noden,
                                        RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), callback);
                    m->AsyncCreateTransportConnection(url_res, ep->GetLocalEndpoint(), noden, h);
                    return;
                }
            }

            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                               "HardwareTransport opening path \""
                                                   << boost::locale::conv::utf_to_utf<char>(*win_path)
                                                   << "\" for URL: " << url);

            HANDLE h = CreateFileW(win_path->c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
                                   FILE_FLAG_OVERLAPPED | SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION, NULL);
            if (h != INVALID_HANDLE_VALUE)
            {
                socket.reset(new HardwareTransportConnection_driver::socket_type(
                    GetNode()->GetThreadPool()->get_io_context(), h));
            }
            else
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                                   "HardwareTransport failed opening path \""
                                                       << boost::locale::conv::utf_to_utf<char>(*win_path)
                                                       << "\" error code " << GetLastError());
            }
        }
        else if (transport == "pci")
        {
            boost::optional<std::wstring> win_path =
                detail::HardwareTransport_win_find_pci(internal1, url_res.nodeid, url_res.nodename);
            if (win_path)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                                   "HardwareTransport opening path \""
                                                       << boost::locale::conv::utf_to_utf<char>(*win_path)
                                                       << "\" for URL: " << url);
                HANDLE h = CreateFileW(win_path->c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
                                       FILE_FLAG_OVERLAPPED | SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION, NULL);
                if (h != INVALID_HANDLE_VALUE)
                {
                    socket.reset(new HardwareTransportConnection_driver::socket_type(
                        GetNode()->GetThreadPool()->get_io_context(), h));
                }
                else
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                                       "HardwareTransport failed opening path \""
                                                           << boost::locale::conv::utf_to_utf<char>(*win_path)
                                                           << "\" error code " << GetLastError());
                }
            }
        }
        else if (transport == "bluetooth")
        {
            boost::optional<std::wstring> win_path =
                detail::HardwareTransport_win_find_bluetooth(internal1, url_res.nodeid, url_res.nodename);
            if (!win_path)
            {
                RR_SHARED_PTR<detail::WinsockBluetoothConnector> bt_connector =
                    RR_MAKE_SHARED<detail::WinsockBluetoothConnector>(shared_from_this());
                bt_connector->Connect(url_res, noden, ep->GetLocalEndpoint(),
                                      boost::bind(&HardwareTransport::AsyncCreateTransportConnection2,
                                                  shared_from_this(), noden, RR_BOOST_PLACEHOLDERS(_1),
                                                  RR_BOOST_PLACEHOLDERS(_2), callback));
                return;
            }

            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                               "HardwareTransport opening path \""
                                                   << boost::locale::conv::utf_to_utf<char>(*win_path)
                                                   << "\" for URL: " << url);
            HANDLE h = CreateFileW(win_path->c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
                                   FILE_FLAG_OVERLAPPED | SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION, NULL);
            if (h != INVALID_HANDLE_VALUE)
            {
                socket.reset(new HardwareTransportConnection_driver::socket_type(
                    GetNode()->GetThreadPool()->get_io_context(), h));
            }
            else
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                                   "HardwareTransport failed opening path \""
                                                       << boost::locale::conv::utf_to_utf<char>(*win_path)
                                                       << "\" error code " << GetLastError());
            }
        }
        else
        {
            throw ConnectionException("Invalid connection transport");
        }

#elif defined(ROBOTRACONTEUR_LINUX) && !defined(ROBOTRACONTEUR_ANDROID)
        if (transport == "usb")
        {
            boost::optional<std::string> dev_path =
                detail::HardwareTransport_linux_find_usb(url_res.nodeid, url_res.nodename);
            if (!dev_path)
            {
                // If we don't find the USB device using driver interface, attempt to find it using WinUSB.
                RR_SHARED_PTR<detail::LibUsbDeviceManager> m;
                {
                    boost::mutex::scoped_lock lock(parameter_lock);
                    if (!internal2)
                    {
                        m = RR_MAKE_SHARED<detail::LibUsbDeviceManager>(shared_from_this());
                        internal2 = m;
                    }
                    else
                    {
                        m = RR_STATIC_POINTER_CAST<detail::LibUsbDeviceManager>(internal2);
                    }

                    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>
                        h = boost::bind(&HardwareTransport::AsyncCreateTransportConnection2, shared_from_this(), noden,
                                        RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), callback);
                    m->AsyncCreateTransportConnection(url_res, ep->GetLocalEndpoint(), noden, h);
                    return;
                }
            }

            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                               "HardwareTransport opening path \"" << *dev_path
                                                                                   << "\" for URL: " << url);

            int fd = open(dev_path->c_str(), O_RDWR);
            if (fd > 0)
            {
                RR_SHARED_PTR<HardwareTransportConnection_driver::socket_type> socket1(
                    new HardwareTransportConnection_driver::socket_type(GetNode()->GetThreadPool()->get_io_context(),
                                                                        fd));
                socket = socket1;
            }
            else
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                                   "HardwareTransport failed opening path \"" << *dev_path
                                                                                              << "\" errno: " << errno);
            }
        }
        else if (transport == "pci")
        {
            boost::optional<std::string> dev_path =
                detail::HardwareTransport_linux_find_pci(url_res.nodeid, url_res.nodename);
            if (dev_path)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                                   "HardwareTransport opening path \"" << *dev_path
                                                                                       << "\" for URL: " << url);

                int fd = open(dev_path->c_str(), O_RDWR);
                if (fd > 0)
                {
                    RR_SHARED_PTR<HardwareTransportConnection_driver::socket_type> socket1(
                        new HardwareTransportConnection_driver::socket_type(
                            GetNode()->GetThreadPool()->get_io_context(), fd));
                    socket = socket1;
                }
                else
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                                       "HardwareTransport failed opening path \""
                                                           << *dev_path << "\" errno: " << errno);
                }
            }
        }
        else if (transport == "bluetooth")
        {

            boost::optional<std::string> dev_path =
                detail::HardwareTransport_linux_find_bluetooth(url_res.nodeid, url_res.nodename);
            if (!dev_path && internal1 && internal4)
            {
                RR_SHARED_PTR<detail::BluezBluetoothConnector> bt_connector =
                    RR_MAKE_SHARED<detail::BluezBluetoothConnector>(shared_from_this(), internal1, internal4);
                bt_connector->Connect(url_res, noden, ep->GetLocalEndpoint(),
                                      boost::bind(&HardwareTransport::AsyncCreateTransportConnection2,
                                                  shared_from_this(), noden, RR_BOOST_PLACEHOLDERS(_1),
                                                  RR_BOOST_PLACEHOLDERS(_2), callback));
                return;
            }

            if (dev_path)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                                   "HardwareTransport opening path \"" << *dev_path
                                                                                       << "\" for URL: " << url);
                int fd = open(dev_path->c_str(), O_RDWR);
                if (fd > 0)
                {
                    RR_SHARED_PTR<HardwareTransportConnection_driver::socket_type> socket1(
                        new HardwareTransportConnection_driver::socket_type(
                            GetNode()->GetThreadPool()->get_io_context(), fd));
                    socket = socket1;
                }
                else
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                                       "HardwareTransport failed opening path \""
                                                           << *dev_path << "\" error code " << errno);
                }
            }
        }
        else
        {
            throw ConnectionException("Invalid connection transport");
        }

#endif

#ifdef ROBOTRACONTEUR_ANDROID
        if (transport == "bluetooth")
        {
            // detail::AndroidHardwareDirector::ConnectBluetooth(shared_from_this(), url_res, noden,
            // ep->GetLocalEndpoint(), boost::bind(&HardwareTransport::AsyncCreateTransportConnection2,
            // shared_from_this(), noden, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2),
            // boost::protect(callback)));
            return;
        }

#endif

        // TODO: test this
        if (!socket)
        {
            throw ConnectionException("Could not connect to service");
        }
        boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> h =
            boost::bind(&HardwareTransport::AsyncCreateTransportConnection2, shared_from_this(), noden,
                        RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), callback);
        HardwareTransport_attach_transport(shared_from_this(), socket, false, ep->GetLocalEndpoint(), noden,
                                           url_res.scheme, h);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(
            node, Transport, ep->GetLocalEndpoint(),
            "HardwareTransport could not connect to service URL: " << url << " error: " << exp.what());
    }
}

void HardwareTransport::AsyncCreateTransportConnection2(
    const std::string& noden, RR_SHARED_PTR<ITransportConnection> transport, RR_SHARED_PTR<RobotRaconteurException> err,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
    if (err)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, transport->GetLocalEndpoint(),
                                          "HardwareTransport failed to connect to device: " << err->what());
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
                                      "HardwareTransport connected transport to device");
    callback(transport, RR_SHARED_PTR<RobotRaconteurException>());
}

RR_SHARED_PTR<ITransportConnection> HardwareTransport::CreateTransportConnection(boost::string_ref url,
                                                                                 RR_SHARED_PTR<Endpoint> e)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<ITransportConnection> > d =
        RR_MAKE_SHARED<detail::sync_async_handler<ITransportConnection> >(
            RR_MAKE_SHARED<ConnectionException>("Timeout exception"));

    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> h =
        boost::bind(&detail::sync_async_handler<ITransportConnection>::operator(), d, RR_BOOST_PLACEHOLDERS(_1),
                    RR_BOOST_PLACEHOLDERS(_2));
    AsyncCreateTransportConnection(url, e, h);

    return d->end();
}

void HardwareTransport::CloseTransportConnection(RR_SHARED_PTR<Endpoint> e)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, e->GetLocalEndpoint(),
                                       "HardwareTransport request close transport connection");

    RR_SHARED_PTR<ServerEndpoint> e2 = boost::dynamic_pointer_cast<ServerEndpoint>(e);
    if (e2)
    {
        RR_SHARED_PTR<boost::asio::deadline_timer> timer(
            new boost::asio::deadline_timer(GetNode()->GetThreadPool()->get_io_context()));
        timer->expires_from_now(boost::posix_time::milliseconds(1000));
        RobotRaconteurNode::asio_async_wait(node, timer,
                                            boost::bind(&HardwareTransport::CloseTransportConnection_timed,
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

void HardwareTransport::CloseTransportConnection_timed(const boost::system::error_code& err, RR_SHARED_PTR<Endpoint> e,
                                                       RR_SHARED_PTR<void> timer)
{
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

void HardwareTransport::SendMessage(RR_INTRUSIVE_PTR<Message> m)
{

    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(m->header->SenderEndpoint);
        if (e1 == TransportConnections.end())
            throw ConnectionException("Transport connection to remote host not found");
        t = e1->second;
    }
    t->SendMessage(m);
}

uint32_t HardwareTransport::TransportCapability(boost::string_ref name) { return 0; }

void HardwareTransport::PeriodicCleanupTask()
{
    boost::mutex::scoped_lock lock(TransportConnections_lock);
    for (RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e = TransportConnections.begin();
         e != TransportConnections.end();)
    {
        try
        {
            RR_SHARED_PTR<HardwareTransportConnection> e2 =
                RR_DYNAMIC_POINTER_CAST<HardwareTransportConnection>(e->second);
            if (e2)
            {
                if (!e2->IsConnected())
                {
                    e = TransportConnections.erase(e);
                }
                else
                {
                    e++;
                }
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

void HardwareTransport::AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m,
                                         boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& handler)
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

void HardwareTransport::MessageReceived(RR_INTRUSIVE_PTR<Message> m) { GetNode()->MessageReceived(m); }

void HardwareTransport::AsyncGetDetectedNodes(
    const std::vector<std::string>& schemes,
    boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)>& handler, int32_t timeout)
{

    boost::mutex::scoped_lock lock(discovery_lock);
#ifdef ROBOTRACONTEUR_WINDOWS
    RR_SHARED_PTR<detail::HardwareTransport_win_discovery> d;

    if (!internal3)
    {
        RR_SHARED_PTR<detail::WinUsbDeviceManager> m;

        if (!internal2)
        {
            RR_SHARED_PTR<SetupApi_Functions> f1 = RR_STATIC_POINTER_CAST<SetupApi_Functions>(internal1);
            m = RR_MAKE_SHARED<detail::WinUsbDeviceManager>(shared_from_this(), f1);
            internal2 = m;
        }
        else
        {
            m = RR_STATIC_POINTER_CAST<detail::WinUsbDeviceManager>(internal2);
        }

        RR_SHARED_PTR<detail::WinsockBluetoothConnector> bt =
            RR_MAKE_SHARED<detail::WinsockBluetoothConnector>(shared_from_this());

        d = RR_MAKE_SHARED<detail::HardwareTransport_win_discovery>(shared_from_this(), schemes, m, bt, internal1);
        internal3 = d;
        d->Init();
    }
    else
    {
        d = RR_STATIC_POINTER_CAST<detail::HardwareTransport_win_discovery>(internal3);
    }

    d->GetAll(handler, timeout);
#elif defined(ROBOTRACONTEUR_LINUX) && !defined(ROBOTRACONTEUR_ANDROID)
    RR_SHARED_PTR<detail::HardwareTransport_linux_discovery> d;

    if (!internal3)
    {
        RR_SHARED_PTR<detail::LibUsbDeviceManager> m;
        {
            boost::mutex::scoped_lock lock(parameter_lock);
            if (!internal2)
            {

                m = RR_MAKE_SHARED<detail::LibUsbDeviceManager>(shared_from_this());
                internal2 = m;
            }
            else
            {
                m = RR_STATIC_POINTER_CAST<detail::LibUsbDeviceManager>(internal2);
            }
        }

        RR_SHARED_PTR<detail::BluezBluetoothConnector> bt;

        if (internal1 && internal4)
        {
            bt = RR_MAKE_SHARED<detail::BluezBluetoothConnector>(shared_from_this(), internal1, internal4);
        }

        d = RR_MAKE_SHARED<detail::HardwareTransport_linux_discovery>(shared_from_this(), schemes, m, bt);
        internal3 = d;
        d->Init();
    }
    else
    {
        d = RR_STATIC_POINTER_CAST<detail::HardwareTransport_linux_discovery>(internal3);
    }

    d->GetAll(handler, timeout);

#else
    RobotRaconteurNode::TryPostToThreadPool(
        node, boost::bind(handler, RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >()), true);
#endif
}

void HardwareTransport::register_transport(RR_SHARED_PTR<ITransportConnection> connection)
{
    boost::mutex::scoped_lock lock(TransportConnections_lock);
    TransportConnections.insert(std::make_pair(connection->GetLocalEndpoint(), connection));
}

void HardwareTransport::erase_transport(RR_SHARED_PTR<ITransportConnection> connection)
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

int32_t HardwareTransport::GetMaxMessageSize()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return max_message_size;
}

void HardwareTransport::SetMaxMessageSize(int32_t size)
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

bool HardwareTransport::GetDisableMessage4()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return disable_message4;
}
void HardwareTransport::SetDisableMessage4(bool d)
{
    boost::mutex::scoped_lock lock(parameter_lock);
    disable_message4 = d;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "DisableMessage4 set to: " << d);
}

bool HardwareTransport::GetDisableStringTable()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return disable_string_table;
}
void HardwareTransport::SetDisableStringTable(bool d)
{
    boost::mutex::scoped_lock lock(parameter_lock);
    disable_string_table = d;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "DisableStringTable set to: " << d);
}

bool HardwareTransport::GetDisableAsyncMessageIO()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return disable_async_message_io;
}
void HardwareTransport::SetDisableAsyncMessageIO(bool d)
{
    boost::mutex::scoped_lock lock(parameter_lock);
    disable_async_message_io = d;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "DisableAsyncMessageIO set to: " << d);
}

void HardwareTransport::AddUsbDevice(uint16_t vid, uint16_t pid, uint8_t interface_)
{
#ifdef ROBOTRACONTEUR_WINDOWS
    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Use WinUsb INF file on Windows to add USB device");
    throw InvalidOperationException("Use WinUsb INF file on Windows to add USB device");
#endif

    boost::mutex::scoped_lock lock(parameter_lock);
    boost::tuple<uint16_t, uint16_t, uint8_t> u = boost::make_tuple<uint16_t, uint16_t, uint8_t>(vid, pid, interface_);
    if (boost::range::find(usb_devices, u) == usb_devices.end())
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1,
                                           "Added USB device " << std::hex << vid << ":" << pid << ":" << interface_);
        usb_devices.push_back(u);
    }
}

void HardwareTransport::RemoveUsbDevice(uint16_t vid, uint16_t pid, uint8_t interface_)
{
#ifdef ROBOTRACONTEUR_WINDOWS
    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Use WinUsb INF file on Windows to add USB device");
    throw InvalidOperationException("Use WinUsb INF file on Windows to add USB device");
#endif
    boost::mutex::scoped_lock lock(parameter_lock);
    boost::tuple<uint16_t, uint16_t, uint8_t> u = boost::make_tuple<uint16_t, uint16_t, uint8_t>(vid, pid, interface_);

    usb_devices.remove(u);

    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1,
                                       "Removed USB device " << std::hex << vid << ":" << pid << ":" << interface_);
}

bool HardwareTransport::IsValidUsbDevice(uint16_t vid, uint16_t pid, uint8_t interface_)
{
    boost::mutex::scoped_lock lock(parameter_lock);
    boost::tuple<uint16_t, uint16_t, uint8_t> u = boost::make_tuple<uint16_t, uint16_t, uint8_t>(vid, pid, interface_);
    return (boost::range::find(usb_devices, u) != usb_devices.end());
}

void HardwareTransport::CheckConnection(uint32_t endpoint)
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

void HardwareTransport::Close()
{
    {
        boost::mutex::scoped_lock lock(closed_lock);
        if (closed)
            return;
        closed = true;
    }

    std::list<RR_SHARED_PTR<ITransportConnection> > c;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        boost::range::copy(this->TransportConnections | boost::adaptors::map_values, std::back_inserter(c));
        TransportConnections.clear();
    }

    BOOST_FOREACH (RR_SHARED_PTR<ITransportConnection> c1, c)
    {
        try
        {
            c1->Close();
        }
        catch (std::exception&)
        {}
    }

#ifdef ROBOTRACONTEUR_WINDOWS
    {
        boost::mutex::scoped_lock lock(discovery_lock);
        if (internal3)
        {
            RR_SHARED_PTR<detail::HardwareTransport_win_discovery> d =
                RR_STATIC_POINTER_CAST<detail::HardwareTransport_win_discovery>(internal3);
            d->Close();
            internal3.reset();
        }
    }
#endif

#if defined(ROBOTRACONTEUR_LINUX) && !defined(ROBOTRACONTEUR_ANDROID)
    {
        boost::mutex::scoped_lock lock(parameter_lock);
        RR_SHARED_PTR<detail::LibUsbDeviceManager> m = RR_STATIC_POINTER_CAST<detail::LibUsbDeviceManager>(internal2);
        if (m)
        {
            m->Shutdown();
        }
    }
    {
        boost::mutex::scoped_lock lock(discovery_lock);
        if (internal3)
        {
            RR_SHARED_PTR<detail::HardwareTransport_linux_discovery> d =
                RR_STATIC_POINTER_CAST<detail::HardwareTransport_linux_discovery>(internal3);
            d->Close();
            internal3.reset();
        }
    }
#endif

    close_signal();

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1, "HardwareTransport closed");
}

HardwareTransportConnection::HardwareTransportConnection(RR_SHARED_PTR<HardwareTransport> parent, bool server,
                                                         uint32_t local_endpoint)
    : ASIOStreamBaseTransport(parent->GetNode())
{
    this->parent = parent;
    this->server = server;
    this->m_LocalEndpoint = local_endpoint;
    this->m_RemoteEndpoint = 0;

    this->HeartbeatPeriod = 30000;
    this->ReceiveTimeout = 600000;

    this->max_message_size = parent->GetMaxMessageSize();

    this->disable_message4 = parent->GetDisableMessage4();
    this->disable_string_table = parent->GetDisableStringTable();
    this->disable_async_io = parent->GetDisableAsyncMessageIO();
}

void HardwareTransportConnection::AsyncAttachSocket1(
    std::string noden, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
    NodeID target_nodeid = NodeID::GetAny();
    std::string target_nodename;
    if (noden.find("{") != std::string::npos)
    {
        target_nodeid = NodeID(noden);
    }
    else
    {
        target_nodename = noden;
    }

    ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
}

void HardwareTransportConnection::MessageReceived(RR_INTRUSIVE_PTR<Message> m)
{

    RR_SHARED_PTR<HardwareTransport> p = parent.lock();
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
                 ret->entries.at(0)->EntryType == MessageEntryType_ReconnectClient) &&
                ret->entries.at(0)->Error == MessageErrorType_None)
            {
                if (ret->header->SenderNodeID == GetNode()->NodeID())
                {
                    boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
                    if (m_LocalEndpoint != 0)
                    {
                        throw InvalidOperationException("Already connected");
                    }
                    m_RemoteEndpoint = ret->header->ReceiverEndpoint;
                    m_LocalEndpoint = ret->header->SenderEndpoint;

                    p->register_transport(RR_STATIC_POINTER_CAST<HardwareTransportConnection>(shared_from_this()));
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                        node, Transport, m_LocalEndpoint,
                        "HardwareTransport connection assigned LocalEndpoint: " << m_LocalEndpoint);
                }
            }

            boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(
                &HardwareTransportConnection::SimpleAsyncEndSendMessage,
                RR_STATIC_POINTER_CAST<HardwareTransportConnection>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1));
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

        std::string connecturl = scheme + ":///";
        Transport::m_CurrentThreadTransportConnectionURL.reset(new std::string(connecturl));
        Transport::m_CurrentThreadTransport.reset(new RR_SHARED_PTR<ITransportConnection>(
            RR_STATIC_POINTER_CAST<HardwareTransportConnection>(shared_from_this())));
        p->MessageReceived(m);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "HardwareTransport failed receiving message: " << exp.what());
        RobotRaconteurNode::TryHandleException(node, &exp);
        Close();
    }

    Transport::m_CurrentThreadTransportConnectionURL.reset(0);
    Transport::m_CurrentThreadTransport.reset(0);
}

void HardwareTransportConnection::Close()
{
    boost::recursive_mutex::scoped_lock lock(close_lock);

    Close1();

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, m_LocalEndpoint, "HardwareTransport closing connection");

    try
    {
        RR_SHARED_PTR<HardwareTransport> p = parent.lock();
        if (p)
            p->erase_transport(RR_STATIC_POINTER_CAST<HardwareTransportConnection>(shared_from_this()));
    }
    catch (std::exception&)
    {}

    ASIOStreamBaseTransport::Close();
}

uint32_t HardwareTransportConnection::GetLocalEndpoint() { return m_LocalEndpoint; }

uint32_t HardwareTransportConnection::GetRemoteEndpoint() { return m_RemoteEndpoint; }

void HardwareTransportConnection::CheckConnection(uint32_t endpoint)
{
    if (endpoint != m_LocalEndpoint || !connected.load())
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Connection lost");
        throw ConnectionException("Connection lost");
    }
}

RR_SHARED_PTR<Transport> HardwareTransportConnection::GetTransport()
{
    RR_SHARED_PTR<Transport> p = parent.lock();
    if (!p)
        throw InvalidOperationException("Transport has been released");
    return p;
}

void HardwareTransport_attach_transport(
    RR_SHARED_PTR<HardwareTransport> parent, RR_SHARED_PTR<HardwareTransportConnection_driver::socket_type> socket,
    bool server, uint32_t endpoint, std::string noden, boost::string_ref scheme,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
    try
    {
        RR_SHARED_PTR<HardwareTransportConnection_driver> t =
            RR_MAKE_SHARED<HardwareTransportConnection_driver>(parent, server, endpoint, scheme);
        boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =
            boost::bind(callback, t, RR_BOOST_PLACEHOLDERS(_1));
        t->AsyncAttachSocket(socket, noden, h);
        parent->AddCloseListener(t, &HardwareTransportConnection_driver::Close);
    }
    catch (std::exception&)
    {
        RobotRaconteurNode::TryPostToThreadPool(
            parent->GetNode(), boost::bind(callback, RR_SHARED_PTR<HardwareTransportConnection>(),
                                           RR_MAKE_SHARED<ConnectionException>("Could not connect to service")));
    }
}

// HardwareTransportConnection_driver

HardwareTransportConnection_driver::HardwareTransportConnection_driver(RR_SHARED_PTR<HardwareTransport> parent,
                                                                       bool server, uint32_t local_endpoint,
                                                                       boost::string_ref scheme)
    : HardwareTransportConnection(parent, server, local_endpoint)
{
    this->scheme = RR_MOVE(scheme.to_string());
}

void HardwareTransportConnection_driver::async_write_some(
    const_buffers& b, boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
{
    boost::mutex::scoped_lock lock(socket_lock);
    RobotRaconteurNode::asio_async_write_some(node, socket, b, handler);
}

void HardwareTransportConnection_driver::async_read_some(
    mutable_buffers& b,
    boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
{
    boost::mutex::scoped_lock lock(socket_lock);
    RobotRaconteurNode::asio_async_read_some(node, socket, b, handler);
}

size_t HardwareTransportConnection_driver::available()
{
#ifdef ROBOTRACONTEUR_WINDOWS
    boost::mutex::scoped_lock lock(socket_lock);
    DWORD lpTotalBytesAvail;
    ::PeekNamedPipe(socket->native_handle(), NULL, NULL, NULL, &lpTotalBytesAvail, NULL);
    return (size_t)lpTotalBytesAvail;
#else
    return 0;
#endif
}

void HardwareTransportConnection_driver::AsyncAttachSocket(
    RR_SHARED_PTR<HardwareTransportConnection_driver::socket_type> socket, std::string noden,
    boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
    this->socket = socket;

    AsyncAttachSocket1(noden, callback);
}

void HardwareTransportConnection_driver::Close1()
{
    boost::mutex::scoped_lock lock(socket_lock);
    socket->close();
}

// HardwareTransportConnection_bluetooth

HardwareTransportConnection_bluetooth::HardwareTransportConnection_bluetooth(RR_SHARED_PTR<HardwareTransport> parent,
                                                                             bool server, uint32_t local_endpoint)
    : HardwareTransportConnection(parent, server, local_endpoint)
{
    scheme = "rr+bluetooth";
}

void HardwareTransportConnection_bluetooth::async_write_some(
    const_buffers& b, boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
{
    boost::mutex::scoped_lock lock(socket_lock);
    RobotRaconteurNode::asio_async_write_some(node, socket, b, handler);
}

void HardwareTransportConnection_bluetooth::async_read_some(
    mutable_buffers& b,
    boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
{
    boost::mutex::scoped_lock lock(socket_lock);
    RobotRaconteurNode::asio_async_read_some(node, socket, b, handler);
}

size_t HardwareTransportConnection_bluetooth::available()
{
    boost::mutex::scoped_lock lock(socket_lock);
    return socket->available();
}

void HardwareTransportConnection_bluetooth::AsyncAttachSocket(
    RR_SHARED_PTR<boost::asio::generic::stream_protocol::socket> socket, std::string noden,
    boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
    this->socket = socket;

    AsyncAttachSocket1(noden, callback);
}

void HardwareTransportConnection_bluetooth::Close1()
{
    boost::mutex::scoped_lock lock(socket_lock);
    socket->close();
}

void HardwareTransport_attach_transport_bluetooth(
    RR_SHARED_PTR<HardwareTransport> parent, RR_SHARED_PTR<boost::asio::generic::stream_protocol::socket> socket,
    bool server, uint32_t endpoint, std::string noden,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
    try
    {
        RR_SHARED_PTR<HardwareTransportConnection_bluetooth> t =
            RR_MAKE_SHARED<HardwareTransportConnection_bluetooth>(parent, server, endpoint);
        boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =
            boost::bind(callback, t, RR_BOOST_PLACEHOLDERS(_1));
        t->AsyncAttachSocket(socket, noden, h);
        parent->AddCloseListener(t, &HardwareTransportConnection_bluetooth::Close);
    }
    catch (std::exception&)
    {
        RobotRaconteurNode::TryPostToThreadPool(
            parent->GetNode(), boost::bind(callback, RR_SHARED_PTR<HardwareTransportConnection>(),
                                           RR_MAKE_SHARED<ConnectionException>("Could not connect to service")));
    }
}
} // namespace RobotRaconteur
