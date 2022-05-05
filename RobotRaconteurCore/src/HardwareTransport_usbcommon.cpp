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

#include "HardwareTransport_usbcommon_private.h"

#include <boost/foreach.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/locale.hpp>
#include <boost/range/iterator_range.hpp>

#ifdef ROBOTRACONTEUR_USE_SMALL_VECTOR
#include <boost/container/small_vector.hpp>
#endif

namespace RobotRaconteur
{
namespace detail
{
// UsbDeviceClaim_create_request
UsbDeviceClaim_create_request::UsbDeviceClaim_create_request(
    const ParseConnectionURLResult& url_res, uint32_t endpoint, boost::string_ref noden,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
{
    this->url_res = url_res;
    this->endpoint = endpoint;
    this->endpoint = endpoint;
    this->handler = handler;
}
// End UsbDeviceClaim_create_request

// UsbDevice_Claim_Lock

UsbDevice_Claim_Lock::UsbDevice_Claim_Lock(RR_SHARED_PTR<UsbDevice_Claim> parent) { this->parent = parent; }

UsbDevice_Claim_Lock::~UsbDevice_Claim_Lock()
{
    RR_SHARED_PTR<UsbDevice_Claim> c = parent.lock();
    if (!c)
        return;
    c->ClaimLockReleased(this);
}
// End UsbDevice_Claim_Lock

// UsbDeviceManager

UsbDeviceManager::UsbDeviceManager(RR_SHARED_PTR<HardwareTransport> parent)
{
    this->parent = parent;
    this->node = parent->GetNode();
    is_shutdown = false;
}

RR_SHARED_PTR<HardwareTransport> UsbDeviceManager::GetParent()
{
    RR_SHARED_PTR<HardwareTransport> t = parent.lock();
    if (!t)
        throw InvalidOperationException("Hardware transport has been released");
    return t;
}

RR_SHARED_PTR<RobotRaconteurNode> UsbDeviceManager::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

void UsbDeviceManager::UpdateDevices(boost::function<void()> handler)
{

    boost::mutex::scoped_lock lock(this_lock);

    if (is_shutdown)
    {
        detail::PostHandler(node, handler, true);
        return;
    }

    if (!InitUpdateDevices())
    {
        RobotRaconteurNode::TryPostToThreadPool(node, handler, true);
        return;
    }

    if (!RobotRaconteurNode::TryPostToThreadPool(
            node, boost::bind(&UsbDeviceManager::UpdateDevices1, shared_from_this(), boost::protect(handler))))
    {
        RobotRaconteurNode::TryPostToThreadPool(node, handler, true);
    }
}

void UsbDeviceManager::UpdateDevices1(boost::function<void()> handler)
{
    boost::mutex::scoped_lock lock(this_lock);

    if (is_shutdown)
    {
        detail::InvokeHandler(node, handler);
        lock.unlock();
        return;
    }

    std::list<UsbDeviceManager_detected_device> detected_devices = GetDetectedDevicesPaths();

    std::list<RR_SHARED_PTR<UsbDevice> > init_devices;

    BOOST_FOREACH (UsbDeviceManager_detected_device& p, detected_devices)
    {
        std::map<std::wstring, RR_SHARED_PTR<UsbDevice> >::iterator e = devices.find(p.path);
        if (e == devices.end())
        {
            RR_SHARED_PTR<UsbDevice> dev = CreateDevice(p);
            init_devices.push_back(dev);
            devices.insert(std::make_pair(p.path, dev));
        }
        else
        {
            UsbDeviceStatus s = e->second->GetDeviceStatus();
            if (s == Busy || s == Error)
            {
                init_devices.push_back(e->second);
            }
        }
    }

    typedef std::map<std::wstring, RR_SHARED_PTR<UsbDevice> >::value_type e_type;

    std::list<RR_SHARED_PTR<UsbDevice> > removed_devices;

    BOOST_FOREACH (e_type& e, devices)
    {
        bool found = false;
        BOOST_FOREACH (UsbDeviceManager_detected_device& p, detected_devices)
        {
            if (p.path == e.first)
            {
                found = true;
            }
        }

        if (!found)
        {
            removed_devices.push_back(e.second);
        }
    }

    if (init_devices.empty())
    {
        RobotRaconteurNode::TryPostToThreadPool(node, handler, true);
    }
    else
    {
        RR_SHARED_PTR<std::list<RR_SHARED_PTR<UsbDevice> > > l =
            RR_MAKE_SHARED<std::list<RR_SHARED_PTR<UsbDevice> > >(init_devices);
        BOOST_FOREACH (RR_SHARED_PTR<UsbDevice> dev, init_devices)
        {
            dev->InitializeDevice(boost::bind(&UsbDeviceManager::UpdateDevices2, shared_from_this(),
                                              RR_BOOST_PLACEHOLDERS(_1), dev, l, boost::protect(handler)));
        }
    }

    lock.unlock();

    BOOST_FOREACH (RR_SHARED_PTR<UsbDevice>& d, removed_devices)
    {
        d->Shutdown();
    }
}

void UsbDeviceManager::UpdateDevices2(UsbDeviceStatus status, RR_SHARED_PTR<UsbDevice> dev,
                                      RR_SHARED_PTR<std::list<RR_SHARED_PTR<UsbDevice> > > l,
                                      boost::function<void()> handler)
{
    boost::mutex::scoped_lock lock(this_lock);

    l->remove(dev);

    if (l->empty())
    {
        RobotRaconteurNode::TryPostToThreadPool(node, handler, true);
    }
}

void UsbDeviceManager::AsyncCreateTransportConnection(
    const ParseConnectionURLResult& url_res, uint32_t endpoint, boost::string_ref noden,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> handler)
{
    UpdateDevices(boost::bind(&UsbDeviceManager::AsyncCreateTransportConnection1, shared_from_this(), url_res, endpoint,
                              noden.to_string(), boost::protect(handler)));
}

void UsbDeviceManager::AsyncCreateTransportConnection1(
    const ParseConnectionURLResult& url_res, uint32_t endpoint, const std::string& noden,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> handler)
{
    boost::mutex::scoped_lock lock(this_lock);

    if (is_shutdown)
    {
        lock.unlock();
        detail::InvokeHandlerWithException(node, handler, RR_MAKE_SHARED<ConnectionException>("Transport closed"));
        return;
    }

    BOOST_FOREACH (RR_SHARED_PTR<UsbDevice>& dev, devices | boost::adaptors::map_values)
    {
        try
        {
            UsbDeviceStatus status = dev->GetDeviceStatus();
            if (status == Ready || status == Claimed || status == Closed || status == Busy)
            {

                boost::tuple<NodeID, std::string> dev_info = dev->GetNodeInfo();
                NodeID& dev_nodeid = dev_info.get<0>();
                std::string& dev_nodename = dev_info.get<1>();

                if (dev_nodeid.IsAnyNode() && dev_nodename.empty())
                {
                    continue;
                }

                bool match = false;
                if (!url_res.nodeid.IsAnyNode() && !url_res.nodename.empty())
                {
                    if (url_res.nodeid == dev_nodeid && url_res.nodename == dev_nodename)
                    {
                        match = true;
                    }
                }
                else if (!url_res.nodename.empty())
                {
                    if (url_res.nodename == dev_nodename)
                    {
                        match = true;
                    }
                }
                else if (!url_res.nodeid.IsAnyNode())
                {
                    if (url_res.nodeid == dev_nodeid)
                    {
                        match = true;
                    }
                }

                if (match)
                {
                    lock.unlock();
                    dev->AsyncCreateTransportConnection(url_res, endpoint, noden, handler);
                    return;
                }
            }
        }
        catch (std::exception&)
        {}
    }

    lock.unlock();

    handler(RR_SHARED_PTR<ITransportConnection>(),
            RR_MAKE_SHARED<ConnectionException>("Could not connect to device or device is busy"));
}

std::list<boost::tuple<NodeID, std::string> > UsbDeviceManager::GetDetectedDevices()
{
    boost::mutex::scoped_lock lock(this_lock);

    std::list<boost::tuple<NodeID, std::string> > o;

    BOOST_FOREACH (RR_SHARED_PTR<UsbDevice>& dev, devices | boost::adaptors::map_values)
    {
        try
        {
            UsbDeviceStatus status = dev->GetDeviceStatus();
            if (status == Ready || status == Claimed || status == Closed || status == Busy)
            {
                o.push_back(dev->GetNodeInfo());
            }
        }
        catch (std::exception&)
        {}
    }

    return o;
}

void UsbDeviceManager::Shutdown()
{
    {
        boost::mutex::scoped_lock lock(this_lock);
        if (is_shutdown)
            return;
        is_shutdown = true;
    }

    std::list<RR_SHARED_PTR<UsbDevice> > dev1;

    {
        boost::mutex::scoped_lock lock(this_lock);
        boost::range::copy(devices | boost::adaptors::map_values, std::back_inserter(dev1));
        devices.clear();
    }

    BOOST_FOREACH (RR_SHARED_PTR<UsbDevice>& dev, dev1)
    {
        try
        {
            dev->Shutdown();
        }
        catch (std::exception&)
        {}
    }
}

// End UsbDeviceManager

// UsbDevice_Initialize

UsbDevice_Initialize::UsbDevice_Initialize(RR_SHARED_PTR<UsbDevice> parent,
                                           const UsbDeviceManager_detected_device& detected_device)
{
    this->parent = parent;
    this->node = parent->GetNode();
    status = NotInitialized;
    this->detected_device = detected_device;
}

RR_SHARED_PTR<UsbDevice> UsbDevice_Initialize::GetParent()
{
    RR_SHARED_PTR<UsbDevice> d = parent.lock();
    if (!d)
        throw InvalidOperationException("Parent has been released");
    return d;
}

RR_SHARED_PTR<RobotRaconteurNode> UsbDevice_Initialize::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

void UsbDevice_Initialize::InitializeDevice(boost::function<void(UsbDeviceStatus)> handler)
{
    boost::mutex::scoped_lock lock(this_lock);

    if (status == Initializing)
    {
        initialize_handlers.push_back(handler);
        return;
    }

    RR_SHARED_PTR<ThreadPool> p = GetNode()->GetThreadPool();

    if (status != NotInitialized && status != Busy)
    {
        InitializeDevice_err(handler, Busy);
        return;
    }

    status = Initializing;
    uint32_t attempt = 0;
    if (!RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&UsbDevice_Initialize::InitializeDevice1,
                                                                   shared_from_this(), 0, boost::protect(handler),
                                                                   RR_SHARED_PTR<boost::asio::deadline_timer>())))
    {
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, Shutdown), true);
    }
}

void UsbDevice_Initialize::InitializeDevice1(uint32_t attempt, boost::function<void(UsbDeviceStatus)> handler,
                                             RR_SHARED_PTR<boost::asio::deadline_timer> timer)
{
    boost::mutex::scoped_lock lock(this_lock);

    RR_SHARED_PTR<ThreadPool> p = GetNode()->GetThreadPool();

    RR_SHARED_PTR<void> dev_h;

    UsbDeviceStatus res = OpenDevice(dev_h);

    if (res != Open || !dev_h)
    {

        if (res == Busy)
        {
            attempt++;
            if (attempt > 20)
            {
                lock.unlock();
                InitializeDevice_err(handler, Busy);
                return;
            }
            RR_SHARED_PTR<boost::asio::deadline_timer> t(
                new boost::asio::deadline_timer(GetNode()->GetThreadPool()->get_io_context()));
            t->expires_from_now(boost::posix_time::milliseconds(100));
            RobotRaconteurNode::asio_async_wait(node, t,
                                                boost::bind(&UsbDevice_Initialize::InitializeDevice1,
                                                            shared_from_this(), attempt, boost::protect(handler), t));
            return;
        }
        lock.unlock();
        InitializeDevice_err(handler, res);
        return;
    }

    RR_SHARED_PTR<UsbDevice_Settings> settings = RR_MAKE_SHARED<UsbDevice_Settings>();

    UsbDeviceStatus res2 = ReadInterfaceSettings(dev_h, settings);
    if (res2 != Open)
    {
        lock.unlock();
        InitializeDevice_err(handler, res);
        return;
    }

    if (boost::range::count(settings->supported_protocols, 0x0100) == 0)
    {
        lock.unlock();
        InitializeDevice_err(handler, Error);
        return;
    }

    UsbDeviceStatus res3 = ReadPipeSettings(dev_h, settings);
    if (res3 != Open)
    {
        lock.unlock();
        InitializeDevice_err(handler, res);
        return;
    }

    lock.unlock();

    ReadRRDeviceString(settings->interface_number, settings->string_nodeid_index,
                       boost::bind(&UsbDevice_Initialize::InitializeDevice2, shared_from_this(),
                                   RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), boost::protect(handler), dev_h,
                                   settings),
                       dev_h);
}

void UsbDevice_Initialize::InitializeDevice2(const boost::system::error_code& ec, const std::string& device_nodeid,
                                             boost::function<void(UsbDeviceStatus)> handler, RR_SHARED_PTR<void> dev_h,
                                             RR_SHARED_PTR<UsbDevice_Settings> settings)
{

    if (ec)
    {
        InitializeDevice_err(handler);
        return;
    }

    try
    {
        NodeID id1(device_nodeid);
        settings->nodeid = id1;
    }
    catch (std::exception&)
    {
        InitializeDevice_err(handler);
        return;
    }

    ReadRRDeviceString(settings->interface_number, settings->string_nodename_index,
                       boost::bind(&UsbDevice_Initialize::InitializeDevice3, shared_from_this(),
                                   RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), boost::protect(handler), dev_h,
                                   settings),
                       dev_h);
}

void UsbDevice_Initialize::InitializeDevice3(const boost::system::error_code& ec, const std::string& device_nodename,
                                             boost::function<void(UsbDeviceStatus)> handler, RR_SHARED_PTR<void> dev_h,
                                             RR_SHARED_PTR<UsbDevice_Settings> settings)
{
    if (ec)
    {
        InitializeDevice_err(handler);
        return;
    }

    settings->nodename = device_nodename;

    {
        boost::mutex::scoped_lock lock(this_lock);
        status = Ready;

        GetParent()->DeviceInitialized(settings);

        BOOST_FOREACH (boost::function<void(UsbDeviceStatus)>& e, initialize_handlers)
        {
            if (!RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(e, status)))
            {
                try
                {
                    e(status);
                }
                catch (std::exception& e)
                {
                    RobotRaconteurNode::TryHandleException(node, &e);
                }
            }
        }

        initialize_handlers.clear();
    }

    handler(Ready);
}

void UsbDevice_Initialize::InitializeDevice_err(boost::function<void(UsbDeviceStatus)>& handler,
                                                UsbDeviceStatus status1)
{
    RR_SHARED_PTR<ThreadPool> p = GetNode()->GetThreadPool();

    boost::mutex::scoped_lock lock(this_lock);
    status = status1;

    GetParent()->DeviceInitialized(RR_SHARED_PTR<UsbDevice_Settings>(), status);

    RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, status1), true);

    BOOST_FOREACH (boost::function<void(UsbDeviceStatus)>& e, initialize_handlers)
    {
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(e, status1), true);
    }

    initialize_handlers.clear();
}

void UsbDevice_Initialize::ReadRRDeviceString(
    uint8_t interface_number, uint8_t property_index,
    boost::function<void(const boost::system::error_code&, const std::string&)> handler, RR_SHARED_PTR<void> dev_h)
{
    boost::shared_array<uint8_t> buf(new uint8_t[255]);

    boost::asio::mutable_buffer buf3(buf.get(), 255);
    AsyncControlTransfer(0x80, 0x06, 0x0300, 0x409, buf3,
                         boost::bind(&UsbDevice_Initialize::ReadRRDeviceString1, shared_from_this(),
                                     RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), interface_number,
                                     property_index, buf, boost::protect(handler), dev_h),
                         dev_h);
}

void UsbDevice_Initialize::ReadRRDeviceString1(
    const boost::system::error_code& ec, size_t bytes_transferred, uint8_t interface_number, uint8_t property_index,
    boost::shared_array<uint8_t> buf,
    boost::function<void(const boost::system::error_code&, const std::string&)> handler, RR_SHARED_PTR<void> dev_h)
{
    if (ec)
    {
        handler(ec, "");
        return;
    }

    if (bytes_transferred < 4)
    {
        handler(boost::asio::error::connection_aborted, "");
        return;
    }

    boost::asio::mutable_buffer buf4(buf.get(), bytes_transferred);
    uint8_t l;
    uint8_t t;
    uint16_t code;
    boost::asio::buffer_copy(boost::asio::mutable_buffer(&l, 1), buf4);
    buf4 = buf4 + 1;
    boost::asio::buffer_copy(boost::asio::mutable_buffer(&t, 1), buf4);
    buf4 = buf4 + 1;
    boost::asio::buffer_copy(boost::asio::mutable_buffer(&code, 2), buf4);

    if (l < 4 || t != 0x03)
    {
        handler(boost::asio::error::connection_aborted, "");
        return;
    }

    boost::shared_array<uint8_t> buf2(new uint8_t[512]);
    memset(buf2.get(), 0, 512);

    boost::asio::mutable_buffer buf3(buf2.get(), 255);
    AsyncControlTransfer(0x80, 0x06, 0x0300 | property_index, code, buf3,
                         boost::bind(&UsbDevice_Initialize::ReadRRDeviceString2, shared_from_this(),
                                     RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), interface_number,
                                     property_index, buf2, boost::protect(handler), dev_h),
                         dev_h);
}

void UsbDevice_Initialize::ReadRRDeviceString2(
    const boost::system::error_code& ec, size_t bytes_transferred, uint8_t interface_number, uint8_t property_index,
    boost::shared_array<uint8_t> buf,
    boost::function<void(const boost::system::error_code&, const std::string&)> handler, RR_SHARED_PTR<void> dev_h)
{
    if (ec)
    {
        handler(ec, "");
        return;
    }

    if (bytes_transferred < 2)
    {
        handler(boost::asio::error::connection_aborted, "");
        return;
    }

    uint8_t len = *reinterpret_cast<uint8_t*>(buf.get());
    if (len != bytes_transferred)
    {
        handler(boost::asio::error::connection_aborted, "");
        return;
    }

    std::string res = boost::locale::conv::utf_to_utf<char>(reinterpret_cast<uint16_t*>(buf.get() + 2));
    boost::system::error_code ec1;
    handler(ec1, res);
}

// End UsbDevice_Initialize

// UsbDevice_Claim

UsbDevice_Claim::UsbDevice_Claim(RR_SHARED_PTR<UsbDevice> parent,
                                 const UsbDeviceManager_detected_device& detected_device)
{
    this->parent = parent;
    this->node = parent->GetNode();
    status = parent->status;
    this->detected_device = detected_device;
    this->settings = parent->settings;
    this->stream_connection_in_progress = 0;

    if (status == Closed)
    {
        status = Ready;
    }

    read_count = 0;
    read_last_complete = 0;
}

RR_SHARED_PTR<UsbDevice> UsbDevice_Claim::GetParent()
{
    RR_SHARED_PTR<UsbDevice> d = parent.lock();
    if (!d)
        throw InvalidOperationException("Parent has been released");
    return d;
}

RR_SHARED_PTR<RobotRaconteurNode> UsbDevice_Claim::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

void UsbDevice_Claim::AsyncCreateTransportConnection(
    const ParseConnectionURLResult& url_res, uint32_t endpoint, boost::string_ref noden,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> handler)
{
    boost::mutex::scoped_lock lock(this_lock);
    stream_connection_in_progress++;
    RR_SHARED_PTR<ThreadPool> p = GetNode()->GetThreadPool();

    // This next operation might block
    if (!RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&UsbDevice_Claim::AsyncCreateTransportConnection1,
                                                                   shared_from_this(), url_res, endpoint,
                                                                   noden.to_string(), boost::protect(handler), 0)))
    {
        RobotRaconteurNode::TryPostToThreadPool(node,
                                                boost::bind(handler, RR_SHARED_PTR<ITransportConnection>(),
                                                            RR_MAKE_SHARED<ConnectionException>("Node shutdown")),
                                                true);
    }
}

void UsbDevice_Claim::AsyncCreateTransportConnection1(
    const ParseConnectionURLResult& url_res, uint32_t endpoint, const std::string& noden,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> handler,
    uint32_t attempt)
{
    boost::mutex::scoped_lock lock(this_lock);

    switch (status)
    {
    case Claimed: {

        lock.unlock();

        AsyncCreateTransportConnection5(url_res, endpoint, noden, handler);
        return;
    }
    case Busy:
    case Ready: {

        RR_SHARED_PTR<void> dev_h;
        UsbDeviceStatus res = ClaimDevice(dev_h);
        if (res != Open)
        {
            if (res == Busy)
            {
                attempt++;
                if (attempt > 20)
                {
                    AsyncCreateTransportConnection_err(handler, Busy);
                    return;
                }
                RR_SHARED_PTR<boost::asio::deadline_timer> t(
                    new boost::asio::deadline_timer(GetNode()->GetThreadPool()->get_io_context()));
                t->expires_from_now(boost::posix_time::milliseconds(100));
                RobotRaconteurNode::asio_async_wait(node, t,
                                                    boost::bind(&UsbDevice_Claim::AsyncCreateTransportConnection1,
                                                                shared_from_this(), url_res, endpoint, noden,
                                                                boost::protect(handler), attempt));
                return;
            }
            else if (res == Unauthorized)
            {
                AsyncCreateTransportConnection_err(handler, Unauthorized);
                return;
            }
            else
            {
                AsyncCreateTransportConnection_err(handler, res);
                return;
            }
        }

        RR_SHARED_PTR<ThreadPool> p = GetNode()->GetThreadPool();

        status = Claiming;

        boost::shared_array<uint8_t> buf(new uint8_t[4]);

        boost::asio::mutable_buffer b1(buf.get(), 4);
        AsyncControlTransferNoLock(VendorInterfaceRequest, RR_USB_CONTROL_RESET_ALL_STREAM, 0,
                                   settings->interface_number, b1,
                                   boost::bind(&UsbDevice_Claim::AsyncCreateTransportConnection2, shared_from_this(),
                                               RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), buf, url_res,
                                               endpoint, noden, boost::protect(handler)));
        break;
    }
    case Claiming: {
        UsbDeviceClaim_create_request req(url_res, endpoint, noden, handler);
        create_requests.push_back(req);
        break;
    }
    case Closing:
    case Closed: {
        RobotRaconteurNode::TryPostToThreadPool(
            node,
            boost::bind(handler, RR_SHARED_PTR<ITransportConnection>(),
                        RR_MAKE_SHARED<ConnectionException>("USB Device Released. Try again.")),
            true);
        stream_connection_in_progress--;
        break;
    }
    default:
        RobotRaconteurNode::TryPostToThreadPool(node,
                                                boost::bind(handler, RR_SHARED_PTR<ITransportConnection>(),
                                                            RR_MAKE_SHARED<ConnectionException>("USB Device Error.")),
                                                true);
        stream_connection_in_progress--;
        break;
    }
}

void UsbDevice_Claim::AsyncCreateTransportConnection2(
    const boost::system::error_code& ec, size_t bytes_transferred, boost::shared_array<uint8_t> buf,
    const ParseConnectionURLResult& url_res, uint32_t endpoint, const std::string& noden,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> handler)
{
    if (ec || bytes_transferred != 4)
    {
        {
            boost::mutex::scoped_lock lock(this_lock);
            stream_connection_in_progress--;
        }
        handler(RR_SHARED_PTR<ITransportConnection>(), RR_MAKE_SHARED<ConnectionException>("USB Device Error"));
        return;
    }

    ClearHalt(settings->in_pipe_id);
    ClearHalt(settings->out_pipe_id);

    boost::shared_array<uint8_t> buf2(new uint8_t[1]);
    boost::asio::mutable_buffer buf3(buf2.get(), 0);
    AsyncWritePipe(settings->out_pipe_id, buf3,
                   boost::bind(&UsbDevice_Claim::AsyncCreateTransportConnection3, shared_from_this(),
                               RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), buf2, url_res, endpoint, noden,
                               boost::protect(handler)));
}

void UsbDevice_Claim::AsyncCreateTransportConnection3(
    const boost::system::error_code& ec, size_t bytes_transferred, boost::shared_array<uint8_t> buf,
    const ParseConnectionURLResult& url_res, uint32_t endpoint, const std::string& noden,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> handler)
{
    ClearHalt(settings->in_pipe_id);
    ClearHalt(settings->out_pipe_id);

    boost::shared_array<uint8_t> buf2(new uint8_t[2]);
    (*reinterpret_cast<uint16_t*>(buf2.get())) = 0x0100;

    boost::asio::mutable_buffer b1(buf2.get(), 2);
    AsyncControlTransfer(VendorInterfaceOutRequest, RR_USB_CONTROL_CURRENT_PROTOCOL, 0, settings->interface_number, b1,
                         boost::bind(&UsbDevice_Claim::AsyncCreateTransportConnection4, shared_from_this(),
                                     RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), buf2, url_res, endpoint,
                                     noden, boost::protect(handler)));
}

void UsbDevice_Claim::AsyncCreateTransportConnection4(
    const boost::system::error_code& ec, size_t bytes_transferred, boost::shared_array<uint8_t> buf,
    const ParseConnectionURLResult& url_res, uint32_t endpoint, const std::string& noden,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> handler)
{
    if (ec)
    {
        {
            boost::mutex::scoped_lock lock(this_lock);
            stream_connection_in_progress--;
            AsyncCreateTransportConnection_err(handler);
        }
        // handler(RR_SHARED_PTR<ITransportConnection>(), RR_MAKE_SHARED<ConnectionException>("USB Device Error"));
        return;
    }

    RR_SHARED_PTR<ThreadPool> p = GetNode()->GetThreadPool();

    {
        boost::mutex::scoped_lock lock(this_lock);

        if (status != Claiming)
        {
            stream_connection_in_progress--;
            AsyncCreateTransportConnection_err(handler);
            return;
        }

        status = Claimed;

        while (read_buf.size() + read_buf_use.size() < 4)
        {
            read_buf.push_back(boost::shared_array<uint8_t>(new uint8_t[settings->in_pipe_buffer_size]));
        }

        while (write_buf.size() + write_buf_use.size() < 4)
        {
            write_buf.push_back(boost::shared_array<uint8_t>(new uint8_t[settings->out_pipe_buffer_size]));
        }

        DoRead();

        BOOST_FOREACH (UsbDeviceClaim_create_request& req, create_requests)
        {
            if (!RobotRaconteurNode::TryPostToThreadPool(
                    node, boost::bind(&UsbDevice_Claim::AsyncCreateTransportConnection1, shared_from_this(),
                                      req.url_res, req.endpoint, req.noden, boost::protect(req.handler), 0)))
            {
                RobotRaconteurNode::TryPostToThreadPool(
                    node,
                    boost::bind(req.handler, RR_SHARED_PTR<ITransportConnection>(),
                                RR_MAKE_SHARED<ConnectionException>("Node shutdown")),
                    true);
            }
        }

        create_requests.clear();
    }

    AsyncCreateTransportConnection5(url_res, endpoint, noden, handler);
}

void UsbDevice_Claim::AsyncCreateTransportConnection5(
    const ParseConnectionURLResult& url_res, uint32_t endpoint, const std::string& noden,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> handler)
{
    boost::shared_array<uint8_t> buf(new uint8_t[4]);

    boost::asio::mutable_buffer b1(buf.get(), 4);
    AsyncControlTransfer(VendorInterfaceRequest, RR_USB_CONTROL_CONNECT_STREAM, 0, settings->interface_number, b1,
                         boost::bind(&UsbDevice_Claim::AsyncCreateTransportConnection6, shared_from_this(),
                                     RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), buf, url_res, endpoint,
                                     noden, boost::protect(handler)));
}

void UsbDevice_Claim::AsyncCreateTransportConnection6(
    const boost::system::error_code& ec, size_t bytes_transferred, boost::shared_array<uint8_t> buf,
    const ParseConnectionURLResult& url_res, uint32_t endpoint, const std::string& noden,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> handler)
{
    try
    {
        boost::mutex::scoped_lock lock(this_lock);
        stream_connection_in_progress--;
        if (ec || status != Claimed)
        {
            CleanupConnections();

            lock.unlock();
            try
            {
                handler(RR_SHARED_PTR<ITransportConnection>(), RR_MAKE_SHARED<ConnectionException>("USB Device Error"));
            }
            catch (std::exception& e)
            {
                RobotRaconteurNode::TryHandleException(node, &e);
            }
            return;
        }

        int32_t id = *reinterpret_cast<int32_t*>(buf.get());

        if (id < 0)
        {
            CleanupConnections();
            AsyncCreateTransportConnection_err(handler);
            return;
        }

        RR_SHARED_PTR<UsbDevice> usb_dev = this->parent.lock();
        if (!usb_dev)
            throw InvalidOperationException("USBDevice has been released");
        RR_SHARED_PTR<HardwareTransport> hw = usb_dev->GetParent()->GetParent();

        RR_SHARED_PTR<UsbDeviceTransportConnection> t = RR_MAKE_SHARED<UsbDeviceTransportConnection>(
            GetParent()->GetParent()->GetParent(), endpoint, shared_from_this(), id);
        transport_connections.insert(std::make_pair(id, t));
        transport_write_idle.push_back(t);

        lock.unlock();
        boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =
            boost::bind(handler, t, RR_BOOST_PLACEHOLDERS(_1));
        t->AsyncAttachSocket(noden, h);
        hw->AddCloseListener(t, &UsbDeviceTransportConnection::Close);
    }
    catch (std::exception&)
    {
        handler(RR_SHARED_PTR<ITransportConnection>(),
                RR_MAKE_SHARED<ConnectionException>("Error creating USB transport connection"));
    }
}

// Call with this_lock locked
void UsbDevice_Claim::AsyncCreateTransportConnection_err(
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& handler,
    UsbDeviceStatus status)
{
    RobotRaconteurNode::TryPostToThreadPool(node,
                                            boost::bind(handler, RR_SHARED_PTR<ITransportConnection>(),
                                                        RR_MAKE_SHARED<ConnectionException>("USB Device Error")),
                                            true);

    BOOST_FOREACH (UsbDeviceClaim_create_request& req, create_requests)
    {
        RobotRaconteurNode::TryPostToThreadPool(node,
                                                boost::bind(req.handler, RR_SHARED_PTR<ITransportConnection>(),
                                                            RR_MAKE_SHARED<ConnectionException>("USB Device Error")),
                                                true);
    }

    create_requests.clear();

    stream_connection_in_progress = 0;

    CleanupConnections();

    GetParent()->DeviceClaimError(shared_from_this(), status);
}

// Call with this_lock locked
void UsbDevice_Claim::CleanupConnections()
{
    // TODO: Fix cleanup connections problem

    if (status == Closing || status == Closed)
        return;

    if (stream_connection_in_progress == 0 && transport_connections.empty() && claim_locks.empty())
    {
        status = Cleanup;
        GetParent()->DeviceClaimReleased(shared_from_this());

        try
        {
            RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&UsbDevice_Claim::Close, shared_from_this()),
                                                    true);
        }
        catch (std::exception&)
        {}
    }
}

void UsbDevice_Claim::transport_write_remove(std::list<RR_WEAK_PTR<UsbDeviceTransportConnection> >& l, int32_t id)
{
    for (std::list<RR_WEAK_PTR<UsbDeviceTransportConnection> >::iterator e = l.begin(); e != l.end();)
    {
        RR_SHARED_PTR<UsbDeviceTransportConnection> c = e->lock();
        if (!c)
        {
            e = l.erase(e);
            continue;
        }

        if (c->stream_id == id)
        {
            e = l.erase(e);
            continue;
        }
        e++;
    }
}

void UsbDevice_Claim::ConnectionClosed(RR_SHARED_PTR<UsbDeviceTransportConnection> connection)
{
    try
    {
        int32_t id;
        {
            boost::mutex::scoped_lock lock(this_lock);

            id = connection->stream_id;
            std::map<int32_t, RR_SHARED_PTR<UsbDeviceTransportConnection> >::iterator e =
                transport_connections.find(id);
            if (e != transport_connections.end())
            {
                transport_write_remove(transport_write, e->second->stream_id);
                transport_write_remove(transport_write_idle, e->second->stream_id);
                transport_connections.erase(e);
            }
        }

        if (!connection->remote_closed)
        {

            boost::mutex::scoped_lock lock(this_lock);

            boost::shared_array<uint8_t> buf(new uint8_t[4]);
            int32_t* buf1 = reinterpret_cast<int32_t*>(buf.get());
            *buf1 = id;

            boost::asio::mutable_buffer b1(buf.get(), 4);
            AsyncControlTransferNoLock(VendorInterfaceOutRequest, RR_USB_CONTROL_CLOSE_STREAM, 0,
                                       settings->interface_number, b1,
                                       boost::bind(&UsbDevice_Claim::ConnectionClosed1, shared_from_this(),
                                                   RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), buf));
        }
    }
    catch (std::exception&)
    {}
}

void UsbDevice_Claim::ConnectionClosed1(const boost::system::error_code& ec, size_t bytes_transferred,
                                        boost::shared_array<uint8_t> buf)
{
    boost::mutex::scoped_lock lock(this_lock);
    CleanupConnections();
}

void UsbDevice_Claim::TransportCanRead()
{
    // TODO: data handling
}

void UsbDevice_Claim::TransportCanWrite()
{
    boost::mutex::scoped_lock lock(this_lock);
    DoWrite();
}

// Call while locked
void UsbDevice_Claim::DoRead()
{

    while (read_buf.size() > 0)
    {
        std::list<boost::shared_array<uint8_t> >::iterator e = read_buf.begin();

        uint64_t c = read_count + 1;
        boost::asio::mutable_buffer b1(e->get(), RR_USB_MAX_PACKET_SIZE);
        AsyncReadPipeNoLock(settings->in_pipe_id, b1,
                            boost::bind(&UsbDevice_Claim::EndRead, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                                        RR_BOOST_PLACEHOLDERS(_2), *e, c));
        read_buf_use.splice(read_buf_use.end(), read_buf, e);
        read_count = c;
    }
}

// Call while locked
void UsbDevice_Claim::DoWrite()
{
    while (write_buf.size() > 0)
    {
        typedef boost::tuple<boost::asio::mutable_buffer, boost::shared_array<uint8_t> > writes_type;
#ifdef ROBOTRACONTEUR_USE_SMALL_VECTOR
        boost::container::small_vector<writes_type, 4> writes;
#else
        std::list<writes_type> writes;
#endif

        if (transport_write_idle.size() == 0 && transport_write.size() == 0)
            return;

        if (transport_write.size() == 0)
        {
            for (std::list<RR_WEAK_PTR<UsbDeviceTransportConnection> >::iterator e = transport_write_idle.begin();
                 e != transport_write_idle.end();)
            {
                RR_SHARED_PTR<UsbDeviceTransportConnection> c = e->lock();
                if (!c)
                {
                    e = transport_write.erase(e);
                    continue;
                }
                if (c->CanDoWrite() > 0)
                {
                    transport_write.splice(transport_write.end(), transport_write_idle, e++);
                    continue;
                }
                e++;
            }
        }

        if (transport_write.size() == 0)
        {
            return;
        }

        for (std::list<RR_WEAK_PTR<UsbDeviceTransportConnection> >::iterator e = transport_write.begin();
             e != transport_write.end() && write_buf.size() > 0;)
        {
            RR_SHARED_PTR<UsbDeviceTransportConnection> c = e->lock();
            if (!c)
            {
                e = transport_write.erase(e);
                continue;
            }

            if (c->CanDoWrite() > 0)
            {
                std::list<boost::shared_array<uint8_t> >::iterator e2 = write_buf.begin();
                boost::asio::mutable_buffer b(e2->get(), RR_USB_MAX_PACKET_SIZE);

                boost::asio::mutable_buffer b2 = b + 8;

                size_t s = c->DoWrite(b2);
                subpacket_header* h = static_cast<subpacket_header*>(RR_BOOST_ASIO_BUFFER_CAST(void*, b));
                h->id = c->stream_id;
                h->flags = 0;
                h->len = boost::numeric_cast<uint16_t>(s + 8);

                writes.push_back(boost::make_tuple(boost::asio::buffer(b, s + 8), *e2));

                write_buf_use.splice(write_buf_use.end(), write_buf, e2);

                if (c->CanDoWrite() == 0)
                {
                    transport_write_idle.splice(transport_write_idle.end(), transport_write, e++);
                }
                else
                {
                    transport_write.splice(transport_write.end(), transport_write, e++);
                }
                continue;
            }
            e++;
        }

        BOOST_FOREACH (writes_type& e, writes)
        {
            AsyncWritePipeNoLock(settings->out_pipe_id, e.get<0>(),
                                 boost::bind(&UsbDevice_Claim::EndWrite, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                                             RR_BOOST_PLACEHOLDERS(_2), e.get<1>()));
        }
    }
}

void UsbDevice_Claim::EndRead(const boost::system::error_code& ec, size_t bytes_transferred,
                              boost::shared_array<uint8_t> buf, uint64_t c)
{
    if (ec)
    {
        DeviceError(ec);
        return;
    }

    if (bytes_transferred < 8 && bytes_transferred != 0)
    {
        DeviceError(boost::asio::error::broken_pipe);
        return;
    }

    boost::mutex::scoped_lock lock(this_lock);

    if (c < (read_last_complete + 1))
    {
        DeviceError(boost::asio::error::fault);
        return;
    }

    if (c == (read_last_complete + 1))
    {
        EndRead2(bytes_transferred, buf, c);
        read_last_complete++;
        std::list<boost::shared_array<uint8_t> >::iterator e = boost::range::find(read_buf_use, buf);
        read_buf.splice(read_buf.end(), read_buf_use, e);
        while (!read_out_of_order.empty() && status == Claimed)
        {
            bool found = false;

            typedef std::list<boost::tuple<size_t, boost::shared_array<uint8_t>, uint64_t> >::iterator e_type;
            for (e_type e2 = read_out_of_order.begin(); e2 != read_out_of_order.end();)
            {
                if (e2->get<2>() == read_last_complete + 1)
                {
                    EndRead2(e2->get<0>(), e2->get<1>(), e2->get<2>());
                    read_last_complete++;
                    found = true;
                    std::list<boost::shared_array<uint8_t> >::iterator e3 =
                        boost::range::find(read_buf_use, e2->get<1>());
                    read_buf.splice(read_buf.end(), read_buf_use, e3);
                    e2 = read_out_of_order.erase(e2);
                    break;
                }
                else
                {
                    e2++;
                }
            }

            if (!found)
                break;
        }
    }
    else
    {
        read_out_of_order.push_back(boost::make_tuple(bytes_transferred, buf, c));
    }

    if (status == Claimed)
    {
        DoRead();
    }
}

// Call with this_lock locked
void UsbDevice_Claim::EndRead2(size_t bytes_transferred, boost::shared_array<uint8_t> buf, uint64_t c)
{
    // Ignore zero length packets
    if (bytes_transferred == 0)
        return;

    boost::asio::mutable_buffer b(buf.get(), bytes_transferred);

    subpacket_header* h = static_cast<subpacket_header*>(RR_BOOST_ASIO_BUFFER_CAST(void*, b));
    uint16_t l = h->len;
    if (l != boost::asio::buffer_size(b))
    {
        RobotRaconteurNode::TryPostToThreadPool(
            node, boost::bind(&UsbDevice_Claim::DeviceError, shared_from_this(), boost::asio::error::fault), true);
        return;
    }

    boost::asio::const_buffer b1 = boost::asio::buffer(b + 8, l);

    if (h->flags == 0)
    {
        std::map<int32_t, RR_SHARED_PTR<UsbDeviceTransportConnection> >::iterator e = transport_connections.find(h->id);
        if (e != transport_connections.end())
        {
            e->second->DoRead(b1);
        }
    }
    else if (h->flags == RR_USB_SUBPACKET_FLAG_COMMAND)
    {
        this->status = Error;
        Close();
    }
    else if (h->flags == RR_USB_SUBPACKET_FLAG_NOTIFICATION)
    {
        if (boost::asio::buffer_size(b1) >= 2)
        {
            uint16_t n = *RR_BOOST_ASIO_BUFFER_CAST(const uint16_t*, b1);
            switch (n)
            {
            case RR_USB_NOTIFICATION_SOCKET_CLOSED: {
                std::map<int32_t, RR_SHARED_PTR<UsbDeviceTransportConnection> >::iterator e =
                    transport_connections.find(h->id);
                if (e != transport_connections.end())
                {
                    RR_SHARED_PTR<UsbDeviceTransportConnection> t = e->second;
                    t->remote_closed = true;
                    transport_write_remove(transport_write, e->second->stream_id);
                    transport_write_remove(transport_write_idle, e->second->stream_id);
                    transport_connections.erase(e);
                    RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&UsbDeviceTransportConnection::Close, t),
                                                            true);
                }
                break;
            }
            case RR_USB_NOTIFICATION_PAUSE_REQUEST: {
                std::map<int32_t, RR_SHARED_PTR<UsbDeviceTransportConnection> >::iterator e =
                    transport_connections.find(h->id);
                if (e != transport_connections.end())
                {
                    RR_SHARED_PTR<UsbDeviceTransportConnection>& t = e->second;
                    t->send_paused = true;
                }
                break;
            }
            case RR_USB_NOTIFICATION_RESUME_REQUEST: {
                std::map<int32_t, RR_SHARED_PTR<UsbDeviceTransportConnection> >::iterator e =
                    transport_connections.find(h->id);
                if (e != transport_connections.end())
                {
                    RR_SHARED_PTR<UsbDeviceTransportConnection>& t = e->second;
                    t->send_paused = false;
                    RobotRaconteurNode::TryPostToThreadPool(
                        node, boost::bind(&UsbDevice_Claim::TransportCanWrite, shared_from_this()));
                }
                break;
            }
            default:
                break;
            }
        }
    }
    else
    {
        status = Error;
        Close();
        return;
    }
}

void UsbDevice_Claim::EndWrite(const boost::system::error_code& ec, size_t bytes_transferred,
                               boost::shared_array<uint8_t> buf)
{
    if (ec)
    {
        DeviceError(ec);
        return;
    }

    boost::mutex::scoped_lock lock(this_lock);
    std::list<boost::shared_array<uint8_t> >::iterator e = boost::range::find(write_buf_use, buf);
    write_buf.splice(write_buf.end(), write_buf_use, e);

    if (status == Claimed)
    {
        DoWrite();
    }
}

void UsbDevice_Claim::DeviceError(const boost::system::error_code& ec) { Close(); }

void UsbDevice_Claim::Close()
{
    // TODO: Handle closing state

    GetParent()->DeviceClaimReleased(shared_from_this());

    std::list<RR_SHARED_PTR<UsbDeviceTransportConnection> > connections;
    {
        boost::mutex::scoped_lock lock(this_lock);

        if (status == Closed || status == Closing)
            return;

        status = Closing;

        boost::range::copy(transport_connections | boost::adaptors::map_values, std::back_inserter(connections));
        transport_connections.clear();
        transport_write_idle.clear();
        transport_write.clear();

        boost::shared_array<uint8_t> buf(new uint8_t[4]);
        boost::asio::mutable_buffer b1(buf.get(), 4);
        AsyncControlTransferNoLock(VendorInterfaceRequest, RR_USB_CONTROL_RESET_ALL_STREAM, 0,
                                   settings->interface_number, b1,
                                   boost::bind(&UsbDevice_Claim::Closed1, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                                               RR_BOOST_PLACEHOLDERS(_2), buf));
    }

    BOOST_FOREACH (RR_SHARED_PTR<UsbDeviceTransportConnection>& c, connections)
    {
        try
        {
            // Connection was closed with the RESET_ALL command, don't send another close notification
            c->remote_closed = true;
            c->Close();
        }
        catch (std::exception&)
        {}
    }
}

void UsbDevice_Claim::Closed1(const boost::system::error_code& ec, size_t bytes_transferred,
                              boost::shared_array<uint8_t> buf)
{
    DrawDownRequests(boost::bind(&UsbDevice_Claim::Closed2, shared_from_this()));
}

void UsbDevice_Claim::Closed2()
{
    boost::mutex::scoped_lock lock(this_lock);

    status = Closed;

    ReleaseClaim();

    if (!create_requests.empty())
    {
        // This should never happen...

        BOOST_FOREACH (UsbDeviceClaim_create_request& req, create_requests)
        {
            RobotRaconteurNode::TryPostToThreadPool(
                node,
                boost::bind(req.handler, RR_SHARED_PTR<ITransportConnection>(),
                            RR_MAKE_SHARED<ConnectionException>("USB Device Released. Try again.")),
                true);
        }

        create_requests.clear();
    }
}

RR_SHARED_PTR<void> UsbDevice_Claim::RequestClaimLock()
{
    boost::mutex::scoped_lock lock(this_lock);
    if (status != Claiming && status != Claimed && status != Busy && status != Ready)
    {
        return RR_SHARED_PTR<void>();
    }
    RR_SHARED_PTR<UsbDevice_Claim_Lock> claim_lock = RR_MAKE_SHARED<UsbDevice_Claim_Lock>(shared_from_this());
    claim_locks.push_back(claim_lock.get());
    return claim_lock;
}

void UsbDevice_Claim::ClaimLockReleased(UsbDevice_Claim_Lock* claim_lock)
{
    boost::mutex::scoped_lock lock(this_lock);

    claim_locks.remove(claim_lock);

    CleanupConnections();
}

UsbDeviceStatus UsbDevice_Claim::GetStatus()
{
    boost::mutex::scoped_lock lock(this_lock);
    return status;
}

// End UsbDevice_Claim

// UsbDevice

UsbDevice::UsbDevice(RR_SHARED_PTR<UsbDeviceManager> parent, const UsbDeviceManager_detected_device& detected_device)
{
    this->parent = parent;
    this->detected_device = detected_device;
    node = parent->GetNode();
    status = NotInitialized;
}

UsbDevice::~UsbDevice() {}

RR_SHARED_PTR<UsbDeviceManager> UsbDevice::GetParent()
{
    RR_SHARED_PTR<UsbDeviceManager> m = parent.lock();
    if (!m)
        throw InvalidOperationException("UsbDeviceManager has been released");
    return m;
}

RR_SHARED_PTR<RobotRaconteurNode> UsbDevice::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

UsbDeviceStatus UsbDevice::GetDeviceStatus()
{
    boost::mutex::scoped_lock lock(this_lock);

    RR_SHARED_PTR<UsbDevice_Claim> c = claim.lock();
    if (c)
    {
        return c->GetStatus();
    }
    else
    {
        return status;
    }
}

void UsbDevice::InitializeDevice(boost::function<void(UsbDeviceStatus)> handler)
{
    boost::mutex::scoped_lock lock(this_lock);

    if (status == NotInitialized || status == Initializing || status == Busy)
    {
        RR_SHARED_PTR<UsbDevice_Initialize> init1 = init.lock();
        if (!init1)
        {
            init1 = CreateInitialize();
            init = init1;
            status = Initializing;
        }

        init1->InitializeDevice(handler);
        return;
    }
    else
    {
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, status), true);
    }
}

boost::tuple<NodeID, std::string> UsbDevice::GetNodeInfo()
{
    if (!settings || !settings->nodeid || !settings->nodename)
    {
        throw InvalidOperationException("Invalid device");
    }

    return boost::make_tuple(*settings->nodeid, *settings->nodename);
}

void UsbDevice::AsyncCreateTransportConnection(
    const ParseConnectionURLResult& url_res, uint32_t endpoint, boost::string_ref noden,
    boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> handler)
{
    boost::mutex::scoped_lock lock(this_lock);

    RR_SHARED_PTR<UsbDevice_Claim> c = claim.lock();
    if (c)
    {
        // Make sure we don't lose the claim before starting create request
        RR_SHARED_PTR<void> claim_lock = c->RequestClaimLock();
        if (claim_lock)
        {
            lock.unlock();
            c->AsyncCreateTransportConnection(url_res, endpoint, noden, handler);
            return;
        }
        else
        {
            claim.reset();
            status = Closed;
        }
    }

    if (!settings || (status != Busy && status != Ready && status != Error && status != Closed && status != Error))
    {
        RobotRaconteurNode::TryPostToThreadPool(
            node,
            boost::bind(handler, RR_SHARED_PTR<ITransportConnection>(),
                        RR_MAKE_SHARED<ConnectionException>("Invalid USB device status")),
            true);
        return;
    }

    c = CreateClaim();

    claim = c;

    RR_SHARED_PTR<void> claim_lock = c->RequestClaimLock();
    lock.unlock();
    c->AsyncCreateTransportConnection(url_res, endpoint, noden, handler);
    return;
}

void UsbDevice::Close()
{
    boost::mutex::scoped_lock lock(this_lock);

    status = Closed;

    try
    {
        RR_SHARED_PTR<UsbDevice_Claim> c = claim.lock();
        if (c)
        {
            c->Close();
            claim.reset();
        }
    }
    catch (std::exception&)
    {}
}

void UsbDevice::Shutdown()
{
    boost::mutex::scoped_lock lock(this_lock);

    status = RobotRaconteur::detail::Shutdown;

    try
    {
        RR_SHARED_PTR<UsbDevice_Claim> c = claim.lock();
        if (c)
        {
            c->Close();
            claim.reset();
        }
    }
    catch (std::exception&)
    {}
}

void UsbDevice::DeviceInitialized(RR_SHARED_PTR<UsbDevice_Settings> settings, UsbDeviceStatus status)
{
    boost::mutex::scoped_lock lock(this_lock);

    if (this->status == Initializing || this->status == Busy)
    {

        if (settings)
            this->settings = settings;

        this->status = status;
    }
}

void UsbDevice::DeviceClaimed(RR_SHARED_PTR<UsbDevice_Claim> claim, UsbDeviceStatus status)
{
    boost::mutex::scoped_lock lock(this_lock);

    RR_SHARED_PTR<UsbDevice_Claim> c = this->claim.lock();
    if (!c)
    {
        claim = c;
        this->status = status;
    }
}

void UsbDevice::DeviceClaimError(RR_SHARED_PTR<UsbDevice_Claim> claim, UsbDeviceStatus status)
{
    boost::mutex::scoped_lock lock(this_lock);

    RR_SHARED_PTR<UsbDevice_Claim> c = this->claim.lock();
    if (!c || c == claim)
    {
        this->claim.reset();
        this->status = status;
    }
}

void UsbDevice::DeviceClaimReleased(RR_SHARED_PTR<UsbDevice_Claim> claim)
{
    RR_SHARED_PTR<UsbDevice_Claim> c = this->claim.lock();
    if (!c || c == claim)
    {
        this->claim.reset();
        this->status = Closed;
    }
}

// End UsbDevice

// UsbDeviceTransportConnection

UsbDeviceTransportConnection::UsbDeviceTransportConnection(RR_SHARED_PTR<HardwareTransport> parent,
                                                           uint32_t local_endpoint,
                                                           RR_SHARED_PTR<UsbDevice_Claim> device, int32_t stream_id)
    : HardwareTransportConnection(parent, false, local_endpoint)
{
    scheme = "rr+usb";
    this->stream_id = stream_id;
    this->device = device;
    this->remote_closed = false;
    this->send_paused = false;
}

void UsbDeviceTransportConnection::AsyncAttachSocket(
    boost::string_ref noden, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
    AsyncAttachSocket1(noden.to_string(), callback);
}

void UsbDeviceTransportConnection::async_write_some(
    const_buffers& b, boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
{
    if (!connected.load())
        throw ConnectionException("Connection lost");
    boost::mutex::scoped_lock lock(do_write_lock);
    // TODO: handle write
    // socket->async_write_some(b, handler);
    RR_SHARED_PTR<UsbDevice_Claim> device1 = device.lock();
    if (!device1)
    {
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, boost::asio::error::broken_pipe, 0), true);
        return;
    }

    write_ops.push_back(boost::make_tuple(b, handler));
    lock.unlock();
    device1->TransportCanWrite();
}

void UsbDeviceTransportConnection::async_read_some(
    mutable_buffers& b,
    boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
{
    if (!connected.load())
        throw ConnectionException("Connection lost");
    boost::mutex::scoped_lock lock(do_read_lock);
    // TODO: handle read
    // socket->async_read_some(b, handler);
    RR_SHARED_PTR<UsbDevice_Claim> device1 = device.lock();
    if (!device1)
    {
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, boost::asio::error::broken_pipe, 0), true);
        return;
    }

    if (!read_bufs.empty())
    {
        boost::tuple<boost::asio::mutable_buffer, boost::shared_array<uint8_t> >& b2 = read_bufs.front();
        size_t s1 = boost::asio::buffer_size(b);
        size_t s2 = boost::asio::buffer_size(b2.get<0>());
        size_t s3 = boost::asio::buffer_copy(b, b2.get<0>());
        if (s1 >= s2)
        {
            read_bufs.pop_front();
        }
        else
        {
            b2.get<0>() = b2.get<0>() + s3;
        }

        boost::system::error_code ec2;
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, ec2, s3), true);
        lock.unlock();
        device1->TransportCanRead();
        return;
    }

    read_ops.push_back(boost::make_tuple(b, handler));
    lock.unlock();
    device1->TransportCanRead();
}

size_t UsbDeviceTransportConnection::available() { return 0; }

void UsbDeviceTransportConnection::Close()
{
    boost::recursive_mutex::scoped_lock lock(close_lock);

    if (!connected.load())
        return;

    {

        try
        {
            RR_SHARED_PTR<UsbDevice_Claim> d = device.lock();
            if (d)
            {
                d->ConnectionClosed(RR_STATIC_POINTER_CAST<UsbDeviceTransportConnection>(shared_from_this()));
            }
        }
        catch (std::exception&)
        {}
    }

    try
    {
        RR_SHARED_PTR<HardwareTransport> p = parent.lock();
        if (p)
            p->erase_transport(RR_STATIC_POINTER_CAST<UsbDeviceTransportConnection>(shared_from_this()));
    }
    catch (std::exception&)
    {}

    try
    {
        ASIOStreamBaseTransport::Close();
    }
    catch (std::exception&)
    {}

    try
    {
        std::list<boost::function<void(const boost::system::error_code&, size_t)> > abort_handlers;
        {
            boost::mutex::scoped_lock lock(do_read_lock);
            typedef boost::tuple<mutable_buffers, boost::function<void(const boost::system::error_code&, size_t)> >
                e_type;
            BOOST_FOREACH (e_type& e, read_ops)
            {
                abort_handlers.push_back(e.get<1>());
            }

            read_ops.clear();
        }
        {
            boost::mutex::scoped_lock lock(do_write_lock);
            typedef boost::tuple<const_buffers, boost::function<void(const boost::system::error_code&, size_t)> >
                e_type;
            BOOST_FOREACH (e_type& e, write_ops)
            {
                abort_handlers.push_back(e.get<1>());
            }
            write_ops.clear();
        }

        BOOST_FOREACH (boost::function<void(const boost::system::error_code&, size_t)>& e1, abort_handlers)
        {
            RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(e1, boost::asio::error::broken_pipe, 0), true);
        }
    }
    catch (std::exception&)
    {}
}

size_t UsbDeviceTransportConnection::CanDoRead() { return std::numeric_limits<size_t>::max(); }

size_t UsbDeviceTransportConnection::CanDoWrite()
{
    boost::mutex::scoped_lock lock(do_write_lock);
    if (write_ops.empty())
    {
        return 0;
    }
    else
    {
        return boost::asio::buffer_size(write_ops.front().get<0>());
    }
}

void UsbDeviceTransportConnection::DoRead(boost::asio::const_buffer& buf)
{
    boost::asio::const_buffer buf1 = buf;
    boost::mutex::scoped_lock lock(do_read_lock);
    while (!read_ops.empty() && boost::asio::buffer_size(buf1) > 0)
    {
        boost::tuple<mutable_buffers, boost::function<void(const boost::system::error_code&, size_t)> > op =
            read_ops.front();
        read_ops.pop_front();
        size_t s = boost::asio::buffer_copy(op.get<0>(), buf1);
        buf1 = buf1 + s;
        boost::system::error_code ec;
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(op.get<1>(), ec, s), true);
    }

    if (boost::asio::buffer_size(buf1) > 0)
    {
        size_t s = boost::asio::buffer_size(buf1);
        boost::shared_array<uint8_t> b(new uint8_t[s]);
        boost::asio::buffer_copy(boost::asio::buffer(b.get(), s), buf1);
        read_bufs.push_back(boost::make_tuple(boost::asio::buffer(b.get(), s), b));
    }
}

size_t UsbDeviceTransportConnection::DoWrite(boost::asio::mutable_buffer& buf)
{
    boost::mutex::scoped_lock lock(do_write_lock);
    if (write_ops.empty())
        return 0;
    boost::tuple<const_buffers, boost::function<void(const boost::system::error_code&, size_t)> > op =
        write_ops.front();
    write_ops.pop_front();
    size_t s = boost::asio::buffer_copy(buf, op.get<0>());
    boost::system::error_code ec;
    RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(op.get<1>(), ec, s), true);
    return s;
}
} // namespace detail
} // namespace RobotRaconteur
