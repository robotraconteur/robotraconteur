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

#include "HardwareTransport_libusb_private.h"
#include "HardwareTransport_private.h"
#include <boost/foreach.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/locale.hpp>
#include <boost/algorithm/string.hpp>
#include <dlfcn.h>

namespace RobotRaconteur
{
namespace detail
{

// LibUsb_Functions
LibUsb_Functions::LibUsb_Functions()
{
    lib_handle = NULL;
    LIBUSB_FUNCTIONS_INIT(LIBUSB_FUNCTIONS_PTR_VOID);
}

bool LibUsb_Functions::LoadFunctions()
{
    if (lib_handle)
        throw InvalidOperationException("libusb functions already loaded");
    lib_handle = dlopen("libusb-1.0.so", RTLD_LAZY);
    if (!lib_handle)
        return false;

    LIBUSB_FUNCTIONS_INIT(LIBUSB_FUNCTIONS_PTR_INIT);

    return true;
}

LibUsb_Functions::~LibUsb_Functions()
{
    if (lib_handle)
    {
        dlclose(lib_handle);
        lib_handle = NULL;
    }
}
// End LibUsb_Functions

static void libusb_status_to_ec(libusb_transfer_status err, boost::system::error_code& ec)
{
    switch (err)
    {
    case LIBUSB_TRANSFER_COMPLETED:
        ec = boost::system::error_code();
        break;
    case LIBUSB_TRANSFER_TIMED_OUT:
        ec = boost::asio::error::make_error_code(boost::asio::error::timed_out);
        break;
    case LIBUSB_TRANSFER_CANCELLED:
        ec = boost::asio::error::make_error_code(boost::asio::error::operation_aborted);
        break;
    case LIBUSB_TRANSFER_NO_DEVICE:
        ec = boost::asio::error::make_error_code(boost::asio::error::host_not_found);
        break;
    default:
        ec = boost::asio::error::make_error_code(boost::asio::error::broken_pipe);
        break;
    }
}

static void libusb_error_to_ec(int err, boost::system::error_code& ec)
{
    if (err >= 0)
    {
        ec = boost::system::error_code();
        return;
    }

    switch (err)
    {
    case LIBUSB_ERROR_TIMEOUT:
        ec = boost::asio::error::make_error_code(boost::asio::error::timed_out);
        break;
    case LIBUSB_ERROR_NO_DEVICE:
    case LIBUSB_ERROR_NOT_FOUND:
        ec = boost::asio::error::make_error_code(boost::asio::error::host_not_found);
        break;
    case LIBUSB_ERROR_BUSY:
        ec = boost::asio::error::make_error_code(boost::asio::error::connection_refused);
        break;
    default:
        ec = boost::asio::error::make_error_code(boost::asio::error::broken_pipe);
        break;
    }
}

// LibUsb_Transfer

LibUsb_Transfer::LibUsb_Transfer(RR_SHARED_PTR<LibUsb_Functions> f, RR_SHARED_PTR<libusb_device_handle> device_handle,
                                 RR_SHARED_PTR<LibUsbDeviceManager> device_manager)
{
    this->f = f;
    this->device_handle = device_handle;
    this->transfer = NULL;
    this->device_manager = device_manager;

    thread_pool = device_manager->GetNode()->GetThreadPool();
    ref_count.store(0);
}

LibUsb_Transfer::~LibUsb_Transfer()
{
    if (transfer != NULL)
    {
        f->libusb_free_transfer(transfer);
        transfer = NULL;
    }
}

void intrusive_ptr_release(LibUsb_Transfer* p)
{
    if (!p)
        return;
    p->ref_count--;
    if (p->ref_count == 0)
    {
        delete p;
    }
}
void intrusive_ptr_add_ref(LibUsb_Transfer* p)
{
    if (!p)
        return;
    p->ref_count++;
}

LibUsb_Transfer_control::LibUsb_Transfer_control(RR_SHARED_PTR<LibUsb_Functions> f,
                                                 RR_SHARED_PTR<libusb_device_handle> device_handle,
                                                 RR_SHARED_PTR<LibUsbDeviceManager> device_manager)
    : LibUsb_Transfer(f, device_handle, device_manager)
{
    temp_buf = NULL;
}

void LibUsb_Transfer_control::FillTransfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                           boost::asio::mutable_buffer& buf,
                                           boost::function<void(const boost::system::error_code&, size_t)> handler)
{
    size_t s = 8 + boost::asio::buffer_size(buf);
    transfer = f->libusb_alloc_transfer(0);
    if (!transfer)
        throw SystemResourceException("Memory error");
    this->temp_buf = new uint8_t[s];
    this->data_buf = buf;
    boost::asio::buffer_copy(boost::asio::buffer(this->temp_buf, s) + 8, buf);
    libusb_fill_control_setup(this->temp_buf, bmRequestType, bRequest, wValue, wIndex, boost::asio::buffer_size(buf));

    libusb_fill_control_transfer(transfer, device_handle.get(), temp_buf, &LibUsbDeviceManager::transfer_complete, this,
                                 5000);

    this->handler = handler;
    ref_count++;
}

void LibUsb_Transfer_control::CompleteTransfer()
{
    RR_SHARED_PTR<ThreadPool> p = thread_pool.lock();
    if (!p)
        return;

    if (transfer->status != LIBUSB_TRANSFER_COMPLETED)
    {
        boost::system::error_code ec1;

        libusb_status_to_ec(transfer->status, ec1);

        p->Post(boost::bind(handler, ec1, 0));

        return;
    }

    size_t actual_length = transfer->actual_length;
    uint8_t* temp_buf1 = libusb_control_transfer_get_data(transfer);
    boost::asio::buffer_copy(data_buf, boost::asio::buffer(temp_buf1, actual_length));

    boost::system::error_code ec2;

    p->Post(boost::bind(handler, ec2, actual_length));
}

LibUsb_Transfer_control::~LibUsb_Transfer_control()
{
    if (temp_buf)
    {
        delete[] temp_buf;
        temp_buf = NULL;
    }
}

void LibUsb_Transfer_bulk::FillTransfer(uint8_t ep, boost::asio::mutable_buffer& buf,
                                        boost::function<void(const boost::system::error_code&, size_t)> handler)
{
    transfer = f->libusb_alloc_transfer(0);
    if (!transfer)
        throw SystemResourceException("Memory error");
    this->data_buf = buf;

    libusb_fill_bulk_transfer(transfer, device_handle.get(), ep, boost::asio::buffer_cast<uint8_t*>(buf),
                              boost::asio::buffer_size(buf), &LibUsbDeviceManager::transfer_complete, this, 0);

    transfer->flags |= LIBUSB_TRANSFER_ADD_ZERO_PACKET;

    this->handler = handler;
    ref_count++;
}

void LibUsb_Transfer_bulk::CompleteTransfer()
{
    RR_SHARED_PTR<ThreadPool> p = thread_pool.lock();
    if (!p)
        return;

    if (transfer->status != LIBUSB_TRANSFER_COMPLETED)
    {
        boost::system::error_code ec1;

        libusb_status_to_ec(transfer->status, ec1);

        p->Post(boost::bind(handler, ec1, 0));

        return;
    }

    size_t actual_length = transfer->actual_length;

    boost::system::error_code ec2;

    handler(ec2, actual_length);
}

LibUsb_Transfer_bulk::LibUsb_Transfer_bulk(RR_SHARED_PTR<LibUsb_Functions> f,
                                           RR_SHARED_PTR<libusb_device_handle> device_handle,
                                           RR_SHARED_PTR<LibUsbDeviceManager> device_manager)
    : LibUsb_Transfer(f, device_handle, device_manager)
{}

LibUsb_Transfer_bulk::~LibUsb_Transfer_bulk() {}

// End LibUsb_Transfer

// LibUsbDeviceManager

LibUsbDeviceManager::LibUsbDeviceManager(RR_SHARED_PTR<HardwareTransport> parent) : UsbDeviceManager(parent)
{
    running = false;
    hotplug_cb_handle = 0;
}

LibUsbDeviceManager::~LibUsbDeviceManager() {}

static void LibUsb_Functions_libusb_exit(RR_SHARED_PTR<LibUsb_Functions> f, libusb_context* c) { f->libusb_exit(c); }

static void LibUsb_Functions_libusb_unref(RR_SHARED_PTR<LibUsb_Functions> f, libusb_device* dev)
{
    f->libusb_unref_device(dev);
}

static UsbDeviceStatus LibUsbDevice_open_device(RR_SHARED_PTR<LibUsbDeviceManager> m, RR_SHARED_PTR<LibUsb_Functions> f,
                                                RR_SHARED_PTR<libusb_device> device, RR_SHARED_PTR<void>& device_handle)
{
    libusb_device_handle* dev_h;
    int res = f->libusb_open(device.get(), &dev_h);
    if (res != 0 || !dev_h)
    {
        if (res == LIBUSB_ERROR_BUSY)
        {
            return Busy;
        }
        if (res == LIBUSB_ERROR_ACCESS)
        {
            return Unauthorized;
        }
        return Error;
    }

    RR_WEAK_PTR<LibUsbDeviceManager> m1 = m;

    RR_SHARED_PTR<libusb_device_handle> dev_h1(
        dev_h, boost::bind(&LibUsbDeviceManager::LibUsbCloseDevice, m1, f, RR_BOOST_PLACEHOLDERS(_1)));

    device_handle = dev_h1;
    return Open;
}

bool LibUsbDeviceManager::InitUpdateDevices()
{
    if (!f)
    {
        RR_SHARED_PTR<LibUsb_Functions> f1 = RR_MAKE_SHARED<LibUsb_Functions>();
        if (!f1->LoadFunctions())
        {
            return false;
        }

        f = f1;
    }

    if (!context)
    {
        libusb_context* c;
        if (f->libusb_init(&c) != 0)
        {
            return false;
        }

        context.reset(c, boost::bind(&LibUsb_Functions_libusb_exit, f, RR_BOOST_PLACEHOLDERS(_1)));

        f->libusb_hotplug_register_callback(
            context.get(),
            static_cast<libusb_hotplug_event>(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
            static_cast<libusb_hotplug_flag>(0), LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY,
            LIBUSB_HOTPLUG_MATCH_ANY, &LibUsbDeviceManager::OnUsbHotplugEvent, this, &hotplug_cb_handle);

        running = true;
        usb_thread = boost::thread(boost::bind(&LibUsbDeviceManager::UsbThread,
                                               RR_STATIC_POINTER_CAST<LibUsbDeviceManager>(shared_from_this())));
    }

    return true;
}

std::list<UsbDeviceManager_detected_device> LibUsbDeviceManager::GetDetectedDevicesPaths()
{
    std::list<UsbDeviceManager_detected_device> devices;

    libusb_device** list1;
    ssize_t device_count = f->libusb_get_device_list(context.get(), &list1);
    if (device_count <= 0)
    {
        f->libusb_free_device_list(list1, 1);
        return devices;
    }

    for (ssize_t i = 0; i < device_count; i++)
    {
        uint8_t interface_number = 0;
        int bus_num = f->libusb_get_bus_number(list1[i]);
        int port_num = f->libusb_get_device_address(list1[i]);
        if (bus_num <= 0 || port_num <= 0)
            continue;

        std::wstring ws_path = boost::lexical_cast<std::wstring>(bus_num) + std::wstring(L",") +
                               boost::lexical_cast<std::wstring>(port_num);

        libusb_device_descriptor device_descriptor;
        if (f->libusb_get_device_descriptor(list1[i], &device_descriptor) != 0)
        {
            continue;
        }

        libusb_config_descriptor* config_desc;
        if (f->libusb_get_active_config_descriptor(list1[i], &config_desc) != 0)
        {
            continue;
        }

        uint8_t interface_ = 0;
        bool rr_found = false;

        RR_SHARED_PTR<HardwareTransport> t = GetParent();

        for (uint8_t j = 0; j < config_desc->bNumInterfaces; j++)
        {
            if (config_desc->interface[j].num_altsetting < 1)
            {
                continue;
            }

            const libusb_interface_descriptor& inter = config_desc->interface[j].altsetting[0];

            if (t->IsValidUsbDevice(device_descriptor.idVendor, device_descriptor.idProduct, inter.bInterfaceNumber))
            {
                interface_ = inter.bInterfaceNumber;
                rr_found = true;
                break;
            }

            if (device_descriptor.bDeviceClass != 0xFF && device_descriptor.bDeviceClass != 0x00)
            {
                continue;
            }

            if (inter.bInterfaceClass != 0xFF)
            {
                continue;
            }

            if (inter.extra_length < sizeof(robotraconteur_interface_descriptor))
            {
                continue;
            }

            boost::asio::const_buffer extra_desc(inter.extra, inter.extra_length);

            while (boost::asio::buffer_size(extra_desc) >= 2)
            {
                const robotraconteur_usb_common_descriptor* c1 =
                    boost::asio::buffer_cast<const robotraconteur_usb_common_descriptor*>(extra_desc);
                if (c1->bLength > boost::asio::buffer_size(extra_desc))
                {
                    continue;
                }

                if (c1->bDescriptorType == RR_USB_CS_INTERFACE_DESCRIPTOR_TYPE)
                {
                    const robotraconteur_interface_common_descriptor* c3 =
                        reinterpret_cast<const robotraconteur_interface_common_descriptor*>(c1);
                    if (c3->bDescriptorSubType == 0)
                    {

                        if (c3->bLength != sizeof(robotraconteur_interface_descriptor))
                        {
                            continue;
                        }
                        const robotraconteur_interface_descriptor* c4 =
                            reinterpret_cast<const robotraconteur_interface_descriptor*>(c3);

                        if (memcmp(RR_USB_CS_INTERFACE_UUID_DETECT, c4->uuidRobotRaconteurDetect,
                                   sizeof(RR_USB_CS_INTERFACE_UUID_DETECT)) == 0)
                        {
                            interface_ = inter.bInterfaceNumber;
                            rr_found = true;
                            break;
                        }
                    }
                }

                extra_desc = extra_desc + c1->bLength;
            }
        }

        f->libusb_free_config_descriptor(config_desc);

        if (!rr_found)
            continue;

        RR_SHARED_PTR<libusb_device> dev_sp(list1[i],
                                            boost::bind(&LibUsb_Functions_libusb_unref, f, RR_BOOST_PLACEHOLDERS(_1)));
        f->libusb_ref_device(dev_sp.get());
        UsbDeviceManager_detected_device d;
        d.handle = dev_sp;
        d.interface_ = interface_;
        d.path = ws_path;
        devices.push_back(d);
    }

    f->libusb_free_device_list(list1, 1);

    return devices;
}

RR_SHARED_PTR<UsbDevice> LibUsbDeviceManager::CreateDevice(const UsbDeviceManager_detected_device& device)
{
    return RR_MAKE_SHARED<LibUsbDevice>(RR_STATIC_POINTER_CAST<LibUsbDeviceManager>(shared_from_this()), f, device);
}

void LibUsbDeviceManager::UsbThread()
{
    RR_SHARED_PTR<libusb_context> context = this->context;

    if (!context)
        return;

    while (true)
    {
        {
            boost::mutex::scoped_lock lock(manager_transfer_lock);
            if (!running)
            {
                if (manager_transfer_list.empty())
                {
                    return;
                }
            }
        }

        int rv = f->libusb_handle_events(context.get());
        if (rv < 0)
        {
            if (rv != LIBUSB_ERROR_TIMEOUT)
            {
                {
                    boost::mutex::scoped_lock lock(this_lock);

                    BOOST_FOREACH (RR_SHARED_PTR<UsbDevice>& d, devices | boost::adaptors::map_values)
                    {
                        d->Close();
                    }

                    devices.clear();

                    context.reset();
                    hotplug_cb_handle = 0;
                    try
                    {
                        std::string message =
                            "Internal usb event loop failure: " + boost::lexical_cast<std::string>(rv);
                        ConnectionException exp(message);
                        RobotRaconteurNode::TryHandleException(node, &exp);
                    }
                    catch (std::exception&)
                    {}
                }
                return;
            }
        }
    }
}

void LibUsb_Functions_libusb_close(RR_SHARED_PTR<LibUsb_Functions> f, libusb_device_handle* h) { f->libusb_close(h); }

void LibUsbDeviceManager::LibUsbCloseDevice(RR_WEAK_PTR<LibUsbDeviceManager> d, RR_SHARED_PTR<LibUsb_Functions> f,
                                            libusb_device_handle* h)
{
    RR_SHARED_PTR<LibUsbDeviceManager> d1 = d.lock();
    if (!d1)
    {
        // We lost the USB device manager...
        f->libusb_close(h);
        return;
    }

    {
        boost::mutex::scoped_lock lock(d1->manager_transfer_lock);
        d1->closing_device_handles.erase(h);
    }

    if (boost::this_thread::get_id() == d1->usb_thread.get_id())
    {
        // Send the close request to the thread pool to avoid deadlock
        RobotRaconteurNode::TryPostToThreadPool(d1->node, boost::bind(&LibUsb_Functions_libusb_close, f, h), true);
    }
    else
    {
        f->libusb_close(h);
    }
}

int LibUsbDeviceManager::OnUsbHotplugEvent(libusb_context* ctx, libusb_device* device, libusb_hotplug_event event,
                                           void* user_data)
{
    return 0;
}

void LibUsbDeviceManager::submit_transfer(boost::intrusive_ptr<LibUsb_Transfer>& transfer)
{
    {
        boost::mutex::scoped_lock lock(manager_transfer_lock);

        if (!running)
        {
            transfer->transfer->actual_length = 0;
            transfer->transfer->status = LIBUSB_TRANSFER_ERROR;
            RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&LibUsb_Transfer::CompleteTransfer, transfer),
                                                    true);
            return;
        }

        if (!closing_device_handles.empty())
        {
            if (closing_device_handles.find(transfer->device_handle.get()) != closing_device_handles.end())
            {
                transfer->transfer->actual_length = 0;
                transfer->transfer->status = LIBUSB_TRANSFER_ERROR;
                RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&LibUsb_Transfer::CompleteTransfer, transfer),
                                                        true);
                return;
            }
        }

        manager_transfer_list.push_back(*transfer);
    }
    int rv = f->libusb_submit_transfer(transfer->transfer);
    if (rv != 0)
    {
        transfer->transfer->actual_length = 0;
        transfer->transfer->status = LIBUSB_TRANSFER_ERROR;
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&LibUsb_Transfer::CompleteTransfer, transfer), true);
    }
}

void LibUsbDeviceManager::transfer_complete(libusb_transfer* transfer)
{
    boost::intrusive_ptr<LibUsb_Transfer> t1(static_cast<LibUsb_Transfer*>(transfer->user_data), false);

    {
        RR_SHARED_PTR<LibUsbDeviceManager> m = t1->device_manager.lock();
        if (m)
        {
            boost::mutex::scoped_lock lock(m->manager_transfer_lock);
            m->manager_transfer_list.erase(m->manager_transfer_list.iterator_to(*t1));

            if (!m->closing_device_handles.empty())
            {
                std::map<libusb_device_handle*, boost::function<void()> >::iterator e =
                    m->closing_device_handles.find(t1->device_handle.get());
                if (e != m->closing_device_handles.end())
                {
                    int count = 0;
                    for (LibUsb_Transfer::manager_transfer_list_t::iterator e2 = m->manager_transfer_list.begin();
                         e2 != m->manager_transfer_list.end(); e2++)
                    {
                        if (e2->device_handle == t1->device_handle)
                        {
                            count++;
                        }
                    }
                    if (count == 0)
                    {
                        boost::function<void()> draw_down_handler = e->second;
                        e->second.clear();
                        if (draw_down_handler)
                        {
                            try
                            {
                                RobotRaconteurNode::TryPostToThreadPool(m->GetNode(), draw_down_handler, true);
                            }
                            catch (std::exception&)
                            {}
                        }
                    }
                }
            }
        }
    }

    t1->CompleteTransfer();
}

void LibUsbDeviceManager::DrawDownRequests(RR_SHARED_PTR<libusb_device_handle> h, boost::function<void()> handler)
{
    // TODO: draw down requests
    // GetNode()->GetThreadPool()->Post(handler);

    boost::mutex::scoped_lock lock(manager_transfer_lock);

    if (closing_device_handles.find(h.get()) != closing_device_handles.end())
    {
        RobotRaconteurNode::TryPostToThreadPool(node, handler, true);
        return;
    }

    int count = 0;
    for (LibUsb_Transfer::manager_transfer_list_t::iterator e = manager_transfer_list.begin();
         e != manager_transfer_list.end(); e++)
    {
        if (e->device_handle == h)
        {
            f->libusb_cancel_transfer(e->transfer);
            count++;
        }
    }

    if (count == 0)
    {
        // This will most likely never happen...
        RobotRaconteurNode::TryPostToThreadPool(node, handler, true);
    }
    else
    {
        closing_device_handles.insert(std::make_pair(h.get(), handler));
    }
}

void LibUsbDeviceManager::Shutdown()
{
    UsbDeviceManager::Shutdown();
    bool r;
    {
        boost::mutex::scoped_lock lock(manager_transfer_lock);
        r = running;
        running = false;
    }

    if (r)
    {
        if (f)
        {
            f->libusb_hotplug_deregister_callback(context.get(), hotplug_cb_handle);
        }
        usb_thread.join();
    }
}

// End LibUsbDeviceManager

// LibUsbDevice_Initialize

LibUsbDevice_Initialize::LibUsbDevice_Initialize(RR_SHARED_PTR<UsbDevice> parent, RR_SHARED_PTR<LibUsb_Functions> f,
                                                 const UsbDeviceManager_detected_device& detected_device)
    : UsbDevice_Initialize(parent, detected_device)
{
    this->f = f;
    this->m = RR_STATIC_POINTER_CAST<LibUsbDeviceManager>(parent->GetParent());
}

void LibUsbDevice_Initialize::AsyncControlTransfer(
    uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, boost::asio::mutable_buffer& buf,
    boost::function<void(const boost::system::error_code&, size_t)> handler, RR_SHARED_PTR<void> dev_h)
{
    boost::mutex::scoped_lock lock(this_lock);
    AsyncControlTransferNoLock(bmRequestType, bRequest, wValue, wIndex, buf, handler, dev_h);
}

// Call with lock
void LibUsbDevice_Initialize::AsyncControlTransferNoLock(
    uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, boost::asio::mutable_buffer& buf,
    boost::function<void(const boost::system::error_code&, size_t)> handler, RR_SHARED_PTR<void> dev_h)
{
    RR_SHARED_PTR<LibUsbDeviceManager> m1 = m.lock();
    if (!m1)
    {
        throw InvalidOperationException("Device manager lost");
    }
    RR_SHARED_PTR<libusb_device_handle> h = RR_STATIC_POINTER_CAST<libusb_device_handle>(dev_h);
    boost::intrusive_ptr<LibUsb_Transfer> t(new LibUsb_Transfer_control(f, h, m1));
    boost::static_pointer_cast<LibUsb_Transfer_control>(t)->FillTransfer(bmRequestType, bRequest, wValue, wIndex, buf,
                                                                         handler);

    m1->submit_transfer(t);
}

// Call with lock
UsbDeviceStatus LibUsbDevice_Initialize::OpenDevice(RR_SHARED_PTR<void>& dev_h)
{
    RR_SHARED_PTR<LibUsbDeviceManager> m1 = m.lock();
    if (!m1)
        return Error;
    return LibUsbDevice_open_device(m1, f, RR_STATIC_POINTER_CAST<libusb_device>(detected_device.handle), dev_h);
}

// Call with lock
UsbDeviceStatus LibUsbDevice_Initialize::ReadPipeSettings(RR_SHARED_PTR<void> dev_h,
                                                          RR_SHARED_PTR<UsbDevice_Settings>& settings)
{

    RR_SHARED_PTR<libusb_device_handle> h = RR_STATIC_POINTER_CAST<libusb_device_handle>(dev_h);

    libusb_config_descriptor* config_desc;
    int desired_config;
    if (f->libusb_get_config_descriptor(f->libusb_get_device(h.get()), 0, &config_desc) != 0)
    {
        return Error;
    }

    desired_config = config_desc->bConfigurationValue;

    const libusb_interface_descriptor* interface_desc = NULL;
    for (uint8_t i = 0; i < config_desc->bNumInterfaces; i++)
    {
        if (config_desc->interface[i].num_altsetting < 1)
        {
            continue;
        }
        if (config_desc->interface[i].altsetting[0].bInterfaceNumber != settings->interface_number)
        {
            continue;
        }
        interface_desc = &config_desc->interface[i].altsetting[0];
    }

    if (!interface_desc)
    {
        f->libusb_free_config_descriptor(config_desc);
        return Error;
    }

    bool in_found = false;
    bool out_found = false;

    for (uint8_t i = 0; i < interface_desc->bNumEndpoints; i++)
    {
        const libusb_endpoint_descriptor* ep = &interface_desc->endpoint[i];
        if (USB_ENDPOINT_TYPE_BULK(ep->bmAttributes) && USB_ENDPOINT_DIRECTION_IN(ep->bEndpointAddress) && !in_found)
        {
            in_found = true;
            settings->in_pipe_id = ep->bEndpointAddress;
            settings->in_pipe_maxpacket = RR_USB_MAX_PACKET_SIZE;
            settings->in_pipe_buffer_size = settings->in_pipe_maxpacket;
        }

        if (USB_ENDPOINT_TYPE_BULK(ep->bmAttributes) && USB_ENDPOINT_DIRECTION_OUT(ep->bEndpointAddress) && !out_found)
        {
            out_found = true;
            settings->out_pipe_id = ep->bEndpointAddress;
            settings->out_pipe_maxpacket = RR_USB_MAX_PACKET_SIZE;
            settings->out_pipe_buffer_size = settings->out_pipe_maxpacket;
        }
    }

    settings->interface_alt_setting = interface_desc->bAlternateSetting;
    settings->device_desired_config = desired_config;

    f->libusb_free_config_descriptor(config_desc);

    if (!in_found && !out_found)
    {
        return Error;
    }

    return Open;
}

UsbDeviceStatus LibUsbDevice_Initialize::ReadInterfaceSettings(RR_SHARED_PTR<void> dev_h,
                                                               RR_SHARED_PTR<UsbDevice_Settings>& settings)
{
    RR_SHARED_PTR<libusb_device_handle> h = RR_STATIC_POINTER_CAST<libusb_device_handle>(dev_h);

    libusb_config_descriptor* config_desc;
    int desired_config;
    if (f->libusb_get_config_descriptor(f->libusb_get_device(h.get()), 0, &config_desc) != 0)
    {
        return Error;
    }

    desired_config = config_desc->bConfigurationValue;

    const libusb_interface_descriptor* interface_desc = NULL;
    for (uint8_t i = 0; i < config_desc->bNumInterfaces; i++)
    {
        if (config_desc->interface[i].num_altsetting < 1)
        {
            continue;
        }
        if (config_desc->interface[i].altsetting[0].bInterfaceNumber != settings->interface_number)
        {
            continue;
        }
        interface_desc = &config_desc->interface[i].altsetting[0];
    }

    if (!interface_desc)
    {
        f->libusb_free_config_descriptor(config_desc);
        return Error;
    }

    boost::asio::const_buffer desc(interface_desc->extra, interface_desc->extra_length);

    bool cs_interface_found = false;
    uint16_t num_protocols;

    while (boost::asio::buffer_size(desc) >= 2)
    {
        const robotraconteur_usb_common_descriptor* c1 =
            boost::asio::buffer_cast<const robotraconteur_usb_common_descriptor*>(desc);
        if (c1->bLength > boost::asio::buffer_size(desc))
        {
            return Error;
        }

        if (c1->bDescriptorType == RR_USB_CS_INTERFACE_DESCRIPTOR_TYPE)
        {
            const robotraconteur_interface_common_descriptor* c3 =
                reinterpret_cast<const robotraconteur_interface_common_descriptor*>(c1);
            if (c3->bDescriptorSubType == 0)
            {

                if (c3->bLength != sizeof(robotraconteur_interface_descriptor))
                {
                    f->libusb_free_config_descriptor(config_desc);
                    return Invalid;
                }
                const robotraconteur_interface_descriptor* c4 =
                    reinterpret_cast<const robotraconteur_interface_descriptor*>(c3);

                if (memcmp(RR_USB_CS_INTERFACE_UUID_DETECT, c4->uuidRobotRaconteurDetect,
                           sizeof(RR_USB_CS_INTERFACE_UUID_DETECT)) != 0)
                {
                    f->libusb_free_config_descriptor(config_desc);
                    return Invalid;
                }

                settings->string_lang_index = 0;
                settings->string_nodeid_index = c4->iNodeID;
                settings->string_nodename_index = c4->iNodeName;
                num_protocols = c4->wNumProtocols;

                cs_interface_found = true;
            }

            if (c3->bDescriptorSubType == 1)
            {
                if (c3->bLength < sizeof(robotraconteur_protocol_descriptor))
                {
                    f->libusb_free_config_descriptor(config_desc);
                    return Error;
                }

                const robotraconteur_protocol_descriptor* c4 =
                    reinterpret_cast<const robotraconteur_protocol_descriptor*>(c3);

                settings->supported_protocols.push_back(c4->wRRProtocol);
            }
        }

        desc = desc + c1->bLength;
    }

    f->libusb_free_config_descriptor(config_desc);

    if (!cs_interface_found)
    {
        return Invalid;
    }

    if (num_protocols != settings->supported_protocols.size())
    {
        return Invalid;
    }

    return Open;
}

// End LibUsbDevice_Initialize

// LibUsbDevice_Claim
LibUsbDevice_Claim::LibUsbDevice_Claim(RR_SHARED_PTR<UsbDevice> parent, RR_SHARED_PTR<LibUsb_Functions> f,
                                       const UsbDeviceManager_detected_device& detected_device)
    : UsbDevice_Claim(parent, detected_device)
{
    this->f = f;
    m = RR_STATIC_POINTER_CAST<LibUsbDeviceManager>(parent->GetParent());
}
LibUsbDevice_Claim::~LibUsbDevice_Claim() {}

void LibUsbDevice_Claim::AsyncControlTransfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                              boost::asio::mutable_buffer& buf,
                                              boost::function<void(const boost::system::error_code&, size_t)> handler,
                                              RR_SHARED_PTR<void> dev_h)
{
    boost::mutex::scoped_lock lock(this_lock);
    AsyncControlTransferNoLock(bmRequestType, bRequest, wValue, wIndex, buf, handler, dev_h);
}

void LibUsbDevice_Claim::AsyncControlTransferNoLock(
    uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, boost::asio::mutable_buffer& buf,
    boost::function<void(const boost::system::error_code&, size_t)> handler, RR_SHARED_PTR<void> dev_h)
{
    RR_SHARED_PTR<LibUsbDeviceManager> m1 = m.lock();
    if (!m1)
    {
        throw InvalidOperationException("Device manager lost");
    }

    if (!dev_h)
    {
        dev_h = device_handle;
    }

    if (!device_handle)
    {
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, boost::asio::error::broken_pipe, 0), true);
        return;
    }

    RR_SHARED_PTR<libusb_device_handle> h = RR_STATIC_POINTER_CAST<libusb_device_handle>(dev_h);
    boost::intrusive_ptr<LibUsb_Transfer> t(new LibUsb_Transfer_control(f, h, m1));
    boost::static_pointer_cast<LibUsb_Transfer_control>(t)->FillTransfer(bmRequestType, bRequest, wValue, wIndex, buf,
                                                                         handler);

    m1->submit_transfer(t);
}

void LibUsbDevice_Claim::AsyncReadPipe(uint8_t ep, boost::asio::mutable_buffer& buf,
                                       boost::function<void(const boost::system::error_code&, size_t)> handler)
{
    boost::mutex::scoped_lock lock(this_lock);
    AsyncPipeOp(ep, buf, handler);
}

void LibUsbDevice_Claim::AsyncReadPipeNoLock(uint8_t ep, boost::asio::mutable_buffer& buf,
                                             boost::function<void(const boost::system::error_code&, size_t)> handler)
{
    AsyncPipeOp(ep, buf, handler);
}

void LibUsbDevice_Claim::AsyncWritePipe(uint8_t ep, boost::asio::mutable_buffer& buf,
                                        boost::function<void(const boost::system::error_code&, size_t)> handler)
{
    boost::mutex::scoped_lock lock(this_lock);
    AsyncPipeOp(ep, buf, handler);
}

void LibUsbDevice_Claim::AsyncWritePipeNoLock(uint8_t ep, boost::asio::mutable_buffer& buf,
                                              boost::function<void(const boost::system::error_code&, size_t)> handler)
{
    AsyncPipeOp(ep, buf, handler);
}

void LibUsbDevice_Claim::AsyncPipeOp(uint8_t ep, boost::asio::mutable_buffer& buf,
                                     boost::function<void(const boost::system::error_code&, size_t)> handler)
{
    RR_SHARED_PTR<LibUsbDeviceManager> m1 = m.lock();
    if (!m1)
    {
        throw InvalidOperationException("Device manager lost");
    }

    if (!device_handle)
    {
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, boost::asio::error::broken_pipe, 0), true);
        return;
    }

    RR_SHARED_PTR<libusb_device_handle> h = RR_STATIC_POINTER_CAST<libusb_device_handle>(device_handle);
    boost::intrusive_ptr<LibUsb_Transfer> t(new LibUsb_Transfer_bulk(f, h, m1));
    boost::static_pointer_cast<LibUsb_Transfer_bulk>(t)->FillTransfer(ep, buf, handler);

    m1->submit_transfer(t);
}

// Call with lock
UsbDeviceStatus LibUsbDevice_Claim::ClaimDevice(RR_SHARED_PTR<void>& dev_h)
{
    RR_SHARED_PTR<LibUsbDeviceManager> m1 = m.lock();
    if (!m1)
        return Error;
    UsbDeviceStatus res =
        LibUsbDevice_open_device(m1, f, RR_STATIC_POINTER_CAST<libusb_device>(detected_device.handle), dev_h);
    if (res != Open)
    {
        return res;
    }

    RR_SHARED_PTR<libusb_device_handle> dev_h1 = RR_STATIC_POINTER_CAST<libusb_device_handle>(dev_h);

    int current_config = 0;
    if (f->libusb_get_configuration(dev_h1.get(), &current_config) != 0)
    {
        return Error;
    }

    if (current_config != settings->device_desired_config)
    {
        f->libusb_set_configuration(dev_h1.get(), 0);
    }

    int res1 = f->libusb_claim_interface(dev_h1.get(), settings->interface_number);
    if (res1 != 0)
    {
        if (res1 == LIBUSB_ERROR_BUSY)
        {
            return Busy;
        }

        if (res1 == LIBUSB_ERROR_ACCESS)
        {
            return Unauthorized;
        }

        return Error;
    }

    f->libusb_clear_halt(dev_h1.get(), settings->in_pipe_id);
    f->libusb_clear_halt(dev_h1.get(), settings->out_pipe_id);

    device_handle = dev_h1;
    return Open;
}

// Call with lock
void LibUsbDevice_Claim::ReleaseClaim()
{
    if (!device_handle)
        return;

    f->libusb_release_interface(device_handle.get(), settings->interface_number);

    device_handle.reset();
}

void LibUsbDevice_Claim::DrawDownRequests(boost::function<void()> handler)
{
    if (!device_handle)
        return;

    // TODO: Draw down active requests

    // f->libusb_(device_handle.get(), settings->in_pipe_id);
    // f->WinUsb_AbortPipe(device_handle.get(), settings->out_pipe_id);

    RR_SHARED_PTR<LibUsbDeviceManager> m;
    try
    {
        m = RR_STATIC_POINTER_CAST<LibUsbDeviceManager>(GetParent()->GetParent());
    }
    catch (std::exception&)
    {}

    if (m)
    {
        m->DrawDownRequests(device_handle, handler);
    }
    else
    {
        RobotRaconteurNode::TryPostToThreadPool(node, handler, true);
    }
}

void LibUsbDevice_Claim::ClearHalt(uint8_t ep)
{
    boost::mutex::scoped_lock lock(this_lock);
    if (!device_handle)
        return;
    f->libusb_clear_halt(device_handle.get(), ep);
}

// End LibUsbDevice_Claim

// LibUsbDevice

LibUsbDevice::LibUsbDevice(RR_SHARED_PTR<LibUsbDeviceManager> parent, RR_SHARED_PTR<LibUsb_Functions> f,
                           const UsbDeviceManager_detected_device& device)
    : UsbDevice(parent, device)
{

    this->f = f;
}

LibUsbDevice::~LibUsbDevice() {}

RR_SHARED_PTR<UsbDevice_Initialize> LibUsbDevice::CreateInitialize()
{
    return RR_MAKE_SHARED<LibUsbDevice_Initialize>(shared_from_this(), f, detected_device);
}
RR_SHARED_PTR<UsbDevice_Claim> LibUsbDevice::CreateClaim()
{
    return RR_MAKE_SHARED<LibUsbDevice_Claim>(shared_from_this(), f, detected_device);
}

// End LibUsbDevice
} // namespace detail
} // namespace RobotRaconteur
