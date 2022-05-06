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

#include "HardwareTransport_win_private.h"
#include "HardwareTransport_private.h"
#include <boost/foreach.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/locale.hpp>
#include <initguid.h>
#include <dbt.h>

// {E274FBA3-1510-4C58-B745-18564DDF78CB}
DEFINE_GUID(GUID_DEVINTERFACE_RobotRaconteurUSBDriver, 0xe274fba3, 0x1510, 0x4c58, 0xb7, 0x45, 0x18, 0x56, 0x4d, 0xdf,
            0x78, 0xcb);

// {6F31E87A-198F-44AD-A546-C553C6334987}
DEFINE_GUID(GUID_DEVINTERFACE_RobotRaconteurPCIDriver, 0x6f31e87a, 0x198f, 0x44ad, 0xa5, 0x46, 0xc5, 0x53, 0xc6, 0x33,
            0x49, 0x87);

// {01BA2F3B-105A-4CB1-87BD-1ECD6543C00C}
DEFINE_GUID(GUID_DEVINTERFACE_RobotRaconteurBluetoothDriver, 0x1ba2f3b, 0x105a, 0x4cb1, 0x87, 0xbd, 0x1e, 0xcd, 0x65,
            0x43, 0xc0, 0xc);

// {E0F04442-C2ED-4E63-9697-604BA38F461A}
DEFINE_GUID(GUID_PROPERTY_RobotRaconteurNodeID, 0xe0f04442, 0xc2ed, 0x4e63, 0x96, 0x97, 0x60, 0x4b, 0xa3, 0x8f, 0x46,
            0x1a);

// {4798EB75-D914-43EC-89C8-5AD5CA44FD86}
DEFINE_GUID(GUID_PROPERTY_RobotRaconteurNodeName, 0x4798eb75, 0xd914, 0x43ec, 0x89, 0xc8, 0x5a, 0xd5, 0xca, 0x44, 0xfd,
            0x86);

// {3F810FD2-2BCE-4552-98F3-A8AC220AD48D}
DEFINE_GUID(GUID_DEVINTERFACE_RobotRaconteurWinUsbDevice, 0x3f810fd2, 0x2bce, 0x4552, 0x98, 0xf3, 0xa8, 0xac, 0x22, 0xa,
            0xd4, 0x8d);

// {25bb0b62-861a-4974-a1b8-18ed5495aa07}
DEFINE_GUID(GUID_RobotRaconteurBluetoothServiceClassID, 0x25bb0b62, 0x861a, 0x4974, 0xa1, 0xb8, 0x18, 0xed, 0x54, 0x95,
            0xaa, 0x07);

#define USB_DIR_OUT 0
#define USB_DIR_IN 0x80

#define USB_TYPE_VENDOR (0x02 << 5)
#define USB_RECIP_INTERFACE 0x01

enum rr_usb_subpacket_notify
{
    RR_USB_NOTIFICATION_NULL = 0,
    RR_USB_NOTIFICATION_SOCKET_CLOSED = 1,
    RR_USB_NOTIFICATION_PAUSE_REQUEST = 2,
    RR_USB_NOTIFICATION_RESUME_REQUEST = 3
};

#define RR_USB_SUBPACKET_FLAG_COMMAND 0x01
#define RR_USB_SUBPACKET_FLAG_NOTIFICATION 0x02

#define VendorInterfaceRequest ((USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE))
#define VendorInterfaceOutRequest ((USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_INTERFACE))

enum rr_usb_control_requests
{
    RR_USB_CONTROL_NULL = 0,
    RR_USB_CONTROL_GET_INFO,
    RR_USB_CONTROL_CONNECT_STREAM,
    RR_USB_CONTROL_CLOSE_STREAM,
    RR_USB_CONTROL_RESET_ALL_STREAM,
    RR_USB_CONTROL_PAUSE_ALL_STREAM,
    RR_USB_CONTROL_RESUME_ALL_STREAM,
    RR_USB_CONTROL_SUPPORTED_PROTOCOLS,
    RR_USB_CONTROL_CURRENT_PROTOCOL
};

#pragma pack(1)
struct subpacket_header
{
    uint16_t len;
    uint16_t flags;
    int32_t id;
};
#pragma pack()

#define RR_USB_MAX_PACKET_SIZE 1024 * 16

namespace RobotRaconteur
{
SetupApi_Functions::SetupApi_Functions() { hLibModule = NULL; }

bool SetupApi_Functions::LoadFunctions()
{
    hLibModule = LoadLibraryW(L"setupapi.dll");
    if (hLibModule == NULL)
    {
        return false;
    }
    SETUPAPI_FUNCTIONS_INIT(SETUPAPI_FUNCTIONS_PTR_INIT);
    return true;
}

SetupApi_Functions::~SetupApi_Functions()
{
    if (hLibModule != NULL)
    {
        FreeLibrary(hLibModule);
    }
}
} // namespace RobotRaconteur

namespace RobotRaconteur
{
namespace detail
{

WinUsb_Functions::WinUsb_Functions() { hLibModule = NULL; }
bool WinUsb_Functions::LoadFunctions()
{
    hLibModule = LoadLibraryW(L"winusb.dll");
    if (hLibModule == NULL)
        return false;

    WINUSB_FUNCTIONS_INIT(WINUSB_FUNCTIONS_PTR_INIT);
    WINUSB_ISOCH_FUNCTIONS_INIT(WINUSB_FUNCTIONS_PTR_INIT_NOERR);
    return true;
}

WinUsb_Functions::~WinUsb_Functions()
{
    if (hLibModule != NULL)
    {
        FreeLibrary(hLibModule);
    }
}

// WinUsbDeviceManager

WinUsbDeviceManager::WinUsbDeviceManager(RR_SHARED_PTR<HardwareTransport> parent,
                                         RR_SHARED_PTR<SetupApi_Functions> setupapi_f)
    : UsbDeviceManager(parent)
{
    this->setupapi_f = setupapi_f;
}

WinUsbDeviceManager::~WinUsbDeviceManager() {}

bool WinUsbDeviceManager::InitUpdateDevices()
{
    if (f)
    {
        return true;
    }
    else
    {
        RR_SHARED_PTR<WinUsb_Functions> f1 = RR_MAKE_SHARED<WinUsb_Functions>();
        if (!f1->LoadFunctions())
        {
            return false;
        }

        f = f1;
    }

    return true;
}

static void SetupApi_Functions_SetupDiDestroyDeviceInfoList(RR_SHARED_PTR<SetupApi_Functions> f, HDEVINFO deviceInfo)
{
    f->SetupDiDestroyDeviceInfoList(deviceInfo);
}

std::list<UsbDeviceManager_detected_device> WinUsbDeviceManager::GetDetectedDevicesPaths()
{
    HDEVINFO deviceInfo;
    RR_SHARED_PTR<void> deviceInfo_sp;
    SP_DEVICE_INTERFACE_DATA interfaceData;
    BOOL bResult;
    std::list<UsbDeviceManager_detected_device> devices;

    deviceInfo = setupapi_f->SetupDiGetClassDevsW(&GUID_DEVINTERFACE_RobotRaconteurWinUsbDevice, NULL, NULL,
                                                  DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (deviceInfo == INVALID_HANDLE_VALUE)
    {
        return devices;
    }

    deviceInfo_sp = RR_SHARED_PTR<void>(deviceInfo, boost::bind(&SetupApi_Functions_SetupDiDestroyDeviceInfoList,
                                                                setupapi_f, RR_BOOST_PLACEHOLDERS(_1)));

    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    DWORD i = 0;

    while (true)
    {
        bResult = setupapi_f->SetupDiEnumDeviceInterfaces(
            deviceInfo, NULL, &GUID_DEVINTERFACE_RobotRaconteurWinUsbDevice, i, &interfaceData);

        i++;

        if (!bResult)
        {
            if (GetLastError() == ERROR_NO_MORE_ITEMS)
            {
                break;
            }
            else
            {
                continue;
            }
        }

        DWORD requiredLength;
        RR_SHARED_PTR<SP_DEVICE_INTERFACE_DETAIL_DATA_W> detailData;

        //
        // Get the size of the path string
        // We expect to get a failure with insufficient buffer
        //
        bResult =
            setupapi_f->SetupDiGetDeviceInterfaceDetailW(deviceInfo, &interfaceData, NULL, 0, &requiredLength, NULL);

        if (FALSE == bResult && ERROR_INSUFFICIENT_BUFFER != GetLastError())
        {

            continue;
        }

        //
        // Allocate temporary space for SetupDi structure
        //
        detailData.reset((PSP_DEVICE_INTERFACE_DETAIL_DATA_W)LocalAlloc(LMEM_FIXED, requiredLength), LocalFree);

        if (NULL == detailData)
        {
            continue;
        }

        detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        DWORD length = requiredLength;

        //
        // Get the interface's path string
        //
        bResult = setupapi_f->SetupDiGetDeviceInterfaceDetailW(deviceInfo, &interfaceData, detailData.get(), length,
                                                               &requiredLength, NULL);

        if (FALSE == bResult)
        {
            continue;
        }

        UsbDeviceManager_detected_device d;
        d.path = std::wstring(detailData->DevicePath);
        d.interface_ = 0;

        devices.push_back(d);
    }

    return devices;
}

RR_SHARED_PTR<UsbDevice> WinUsbDeviceManager::CreateDevice(const UsbDeviceManager_detected_device& device)
{
    return RR_MAKE_SHARED<WinUsbDevice>(RR_STATIC_POINTER_CAST<WinUsbDeviceManager>(shared_from_this()), f, device);
}

// WinUsbDevice_Handle

static void WinUsbDevice_winusb_free(RR_SHARED_PTR<WinUsb_Functions> f, void* hInterface)
{
    f->WinUsb_Free(hInterface);
}

static UsbDeviceStatus WinUsbDevice_open_device(RR_BOOST_ASIO_IO_CONTEXT& _io_context,
                                                RR_SHARED_PTR<WinUsb_Functions>& f, const std::wstring& path,
                                                RR_SHARED_PTR<void>& dev_h)
{
    HANDLE h1 = CreateFileW(path.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
    if (h1 == INVALID_HANDLE_VALUE)
    {
        DWORD a = GetLastError();
        if (GetLastError() == ERROR_ACCESS_DENIED)
        {
            return Busy;
        }

        return Error;
    }

    RR_SHARED_PTR<boost::asio::windows::stream_handle> device_handle1(
        new boost::asio::windows::stream_handle(_io_context, h1));

    WINUSB_INTERFACE_HANDLE h2;
    if (!f->WinUsb_Initialize(device_handle1->native_handle(), &h2))
    {
        return Error;
    }

    RR_SHARED_PTR<void> hInterface1(h2, boost::bind(&WinUsbDevice_winusb_free, f, RR_BOOST_PLACEHOLDERS(_1)));

    RR_SHARED_PTR<WinUsbDevice_Handle> h = RR_MAKE_SHARED<WinUsbDevice_Handle>();
    h->device_handle = device_handle1;
    h->hInterface = hInterface1;

    dev_h = h;
    return Open;
}

WinUsbDevice_Handle::~WinUsbDevice_Handle()
{
    hInterface.reset();
    device_handle.reset();
}

// WinUsbDevice_Initialize

WinUsbDevice_Initialize::WinUsbDevice_Initialize(RR_SHARED_PTR<UsbDevice> parent, RR_SHARED_PTR<WinUsb_Functions> f,
                                                 const UsbDeviceManager_detected_device& detected_device)
    : UsbDevice_Initialize(parent, detected_device)
{
    this->f = f;
}

UsbDeviceStatus WinUsbDevice_Initialize::OpenDevice(RR_SHARED_PTR<void>& dev_h)
{
    return WinUsbDevice_open_device(GetNode()->GetThreadPool()->get_io_context(), f, detected_device.path, dev_h);
}

UsbDeviceStatus WinUsbDevice_Initialize::ReadInterfaceSettings(RR_SHARED_PTR<void> dev_h,
                                                               RR_SHARED_PTR<UsbDevice_Settings>& settings)
{
    RR_SHARED_PTR<WinUsbDevice_Handle> h = RR_STATIC_POINTER_CAST<WinUsbDevice_Handle>(dev_h);

    UCHAR altSetting = 0;
    if (!f->WinUsb_GetCurrentAlternateSetting(h->hInterface.get(), &altSetting))
    {
        return Error;
    }

    settings->interface_alt_setting = altSetting;

    USB_INTERFACE_DESCRIPTOR d;
    if (!f->WinUsb_QueryInterfaceSettings(h->hInterface.get(), settings->interface_alt_setting, &d))
    {
        return Error;
    }

    settings->interface_number = d.bInterfaceNumber;
    settings->interface_num_endpoints = d.bNumEndpoints;

    ULONG desc_buf_len = 64 * 1024;
    ULONG desc_buf_read = 0;
    boost::scoped_array<uint8_t> desc_buf(new uint8_t[desc_buf_len]);

    if (!f->WinUsb_GetDescriptor(h->hInterface.get(), USB_CONFIGURATION_DESCRIPTOR_TYPE, settings->interface_number, 0,
                                 desc_buf.get(), desc_buf_len, &desc_buf_read))
    {
        return Error;
    }

    boost::asio::mutable_buffer desc(desc_buf.get(), desc_buf_read);

    bool in_desired_interface = false;

    bool cs_interface_found = false;
    uint16_t num_protocols;

    while (boost::asio::buffer_size(desc) >= 2)
    {
        USB_COMMON_DESCRIPTOR* c1 = RR_BOOST_ASIO_BUFFER_CAST(USB_COMMON_DESCRIPTOR*, desc);
        if (c1->bLength > boost::asio::buffer_size(desc))
        {
            return Error;
        }

        if (c1->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE)
        {
            USB_INTERFACE_DESCRIPTOR* c2 = (USB_INTERFACE_DESCRIPTOR*)c1;
            if (c2->bInterfaceNumber == settings->interface_number &&
                c2->bAlternateSetting == settings->interface_alt_setting)
            {
                in_desired_interface = true;
            }
            else
            {
                in_desired_interface = false;
            }
        }

        if (c1->bDescriptorType == USB_ENDPOINT_DESCRIPTOR_TYPE)
        {
            in_desired_interface = false;
        }

        if (in_desired_interface && c1->bDescriptorType == USB_CS_INTERFACE_DESCRIPTOR_TYPE)
        {
            robotraconteur_interface_common_descriptor* c3 = (robotraconteur_interface_common_descriptor*)c1;
            if (c3->bDescriptorSubType == 0)
            {

                if (c3->bLength != sizeof(robotraconteur_interface_descriptor))
                {
                    return Invalid;
                }
                robotraconteur_interface_descriptor* c4 = (robotraconteur_interface_descriptor*)c3;

                if (memcmp(RR_USB_CS_INTERFACE_UUID_DETECT, c4->uuidRobotRaconteurDetect,
                           sizeof(RR_USB_CS_INTERFACE_UUID_DETECT)) != 0)
                {
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
                    return Error;
                }

                robotraconteur_protocol_descriptor* c4 = (robotraconteur_protocol_descriptor*)c3;

                settings->supported_protocols.push_back(c4->wRRProtocol);
            }
        }

        desc = desc + c1->bLength;
    }

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

UsbDeviceStatus WinUsbDevice_Initialize::ReadPipeSettings(RR_SHARED_PTR<void> dev_h,
                                                          RR_SHARED_PTR<UsbDevice_Settings>& settings)
{

    RR_SHARED_PTR<WinUsbDevice_Handle> h = RR_STATIC_POINTER_CAST<WinUsbDevice_Handle>(dev_h);

    bool in_found = false;
    bool out_found = false;

    for (UCHAR i = 0; i < settings->interface_num_endpoints; i++)
    {
        // Bug with WinUsb WINUSB_PIPE_INFORMATION structure?
        uint8_t d_pipe1[sizeof(WINUSB_PIPE_INFORMATION) * 2];

        PWINUSB_PIPE_INFORMATION d_pipe = static_cast<PWINUSB_PIPE_INFORMATION>((void*)&d_pipe1);
        if (f->WinUsb_QueryPipe(h->hInterface.get(), settings->interface_alt_setting, i, d_pipe))
        {
            if (d_pipe->PipeType == UsbdPipeTypeBulk && USB_ENDPOINT_DIRECTION_IN(d_pipe->PipeId) && !in_found)
            {
                in_found = true;
                settings->in_pipe_id = d_pipe->PipeId;
                settings->in_pipe_maxpacket = d_pipe->MaximumPacketSize;
                if (RR_USB_MAX_PACKET_SIZE % settings->in_pipe_maxpacket == 0)
                {
                    settings->in_pipe_buffer_size = RR_USB_MAX_PACKET_SIZE;
                }
                else
                {
                    settings->in_pipe_buffer_size =
                        ((RR_USB_MAX_PACKET_SIZE / settings->in_pipe_maxpacket) + 1) * settings->in_pipe_maxpacket;
                }
            }

            if (d_pipe->PipeType == UsbdPipeTypeBulk && USB_ENDPOINT_DIRECTION_OUT(d_pipe->PipeId) && !out_found)
            {
                out_found = true;
                settings->out_pipe_id = d_pipe->PipeId;
                settings->out_pipe_maxpacket = d_pipe->MaximumPacketSize;
                if (RR_USB_MAX_PACKET_SIZE % settings->out_pipe_maxpacket == 0)
                {
                    settings->out_pipe_buffer_size = RR_USB_MAX_PACKET_SIZE;
                }
                else
                {
                    settings->out_pipe_buffer_size =
                        ((RR_USB_MAX_PACKET_SIZE / settings->in_pipe_maxpacket) + 1) * settings->out_pipe_maxpacket;
                }
            }
        }
    }

    if (!in_found || !out_found)
    {
        return Error;
    }

    return Open;
}

static void WinUsbDevice_async_control_transfer(RR_BOOST_ASIO_IO_CONTEXT& _io_context,
                                                RR_SHARED_PTR<WinUsb_Functions> f, uint8_t bmRequestType,
                                                uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                                boost::asio::mutable_buffer& buf,
                                                boost::function<void(const boost::system::error_code&, size_t)> handler,
                                                RR_SHARED_PTR<void> dev_h = RR_SHARED_PTR<void>())
{
    RR_SHARED_PTR<void> my_hInterface;

    if (!dev_h)
    {
        RR_BOOST_ASIO_POST(_io_context, boost::bind(handler, boost::asio::error::broken_pipe, 0));
        return;
    }

    RR_SHARED_PTR<WinUsbDevice_Handle> h = RR_STATIC_POINTER_CAST<WinUsbDevice_Handle>(dev_h);
    my_hInterface = h->hInterface;

    boost::asio::windows::overlapped_ptr overlapped(_io_context, handler);

    DWORD bytes_transferred = 0;

    WINUSB_SETUP_PACKET setup;
    setup.RequestType = bmRequestType;
    setup.Request = bRequest;
    setup.Value = wValue;
    setup.Index = wIndex;
    setup.Length = (USHORT)boost::asio::buffer_size(buf);

    BOOL ok = f->WinUsb_ControlTransfer(my_hInterface.get(), setup, RR_BOOST_ASIO_BUFFER_CAST(PUCHAR, buf),
                                        boost::asio::buffer_size(buf), &bytes_transferred, overlapped.get());
    DWORD last_error = ::GetLastError();
    if (!ok && last_error != ERROR_IO_PENDING && last_error != ERROR_MORE_DATA)
    {
        boost::system::error_code ec(last_error, boost::asio::error::system_category);
        overlapped.complete(ec, 0);
    }
    else
    {
        overlapped.release();
    }
}

void WinUsbDevice_Initialize::AsyncControlTransfer(
    uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, boost::asio::mutable_buffer& buf,
    boost::function<void(const boost::system::error_code&, size_t)> handler, RR_SHARED_PTR<void> dev_h)
{
    boost::mutex::scoped_lock lock(this_lock);
    WinUsbDevice_async_control_transfer(GetNode()->GetThreadPool()->get_io_context(), f, bmRequestType, bRequest,
                                        wValue, wIndex, buf, handler, dev_h);
}

void WinUsbDevice_Initialize::AsyncControlTransferNoLock(
    uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, boost::asio::mutable_buffer& buf,
    boost::function<void(const boost::system::error_code&, size_t)> handler, RR_SHARED_PTR<void> dev_h)
{
    // boost::mutex::scoped_lock lock(this_lock);
    WinUsbDevice_async_control_transfer(GetNode()->GetThreadPool()->get_io_context(), f, bmRequestType, bRequest,
                                        wValue, wIndex, buf, handler, dev_h);
}

// WinUsbDevice_Claim

WinUsbDevice_Claim::WinUsbDevice_Claim(RR_SHARED_PTR<UsbDevice> parent, RR_SHARED_PTR<WinUsb_Functions> f,
                                       const UsbDeviceManager_detected_device& detected_device)
    : UsbDevice_Claim(parent, detected_device)
{
    this->f = f;
}

void WinUsbDevice_Claim::AsyncControlTransfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                              boost::asio::mutable_buffer& buf,
                                              boost::function<void(const boost::system::error_code&, size_t)> handler,
                                              RR_SHARED_PTR<void> dev_h)
{
    boost::mutex::scoped_lock lock(this_lock);

    if (!dev_h)
    {
        dev_h = device_handle;
    }

    WinUsbDevice_async_control_transfer(GetNode()->GetThreadPool()->get_io_context(), f, bmRequestType, bRequest,
                                        wValue, wIndex, buf, handler, dev_h);
}

void WinUsbDevice_Claim::AsyncControlTransferNoLock(
    uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, boost::asio::mutable_buffer& buf,
    boost::function<void(const boost::system::error_code&, size_t)> handler, RR_SHARED_PTR<void> dev_h)
{
    // boost::mutex::scoped_lock lock(this_lock);

    if (!dev_h)
    {
        dev_h = device_handle;
    }

    WinUsbDevice_async_control_transfer(GetNode()->GetThreadPool()->get_io_context(), f, bmRequestType, bRequest,
                                        wValue, wIndex, buf, handler, dev_h);
}

void WinUsbDevice_Claim::AsyncReadPipe(uint8_t ep, boost::asio::mutable_buffer& buf,
                                       boost::function<void(const boost::system::error_code&, size_t)> handler)
{
    boost::mutex::scoped_lock lock(this_lock);
    AsyncReadPipeNoLock(ep, buf, handler);
}

void WinUsbDevice_Claim::AsyncReadPipeNoLock(uint8_t ep, boost::asio::mutable_buffer& buf,
                                             boost::function<void(const boost::system::error_code&, size_t)> handler)
{
    if (status != Claimed || !device_handle)
    {
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, boost::asio::error::bad_descriptor, 0),
                                                true);
        return;
    }

    RR_BOOST_ASIO_IO_CONTEXT& io = GetNode()->GetThreadPool()->get_io_context();

    boost::asio::windows::overlapped_ptr overlapped(io, handler);

    DWORD bytes_transferred = 0;

    BOOL ok = f->WinUsb_ReadPipe(device_handle->hInterface.get(), ep, RR_BOOST_ASIO_BUFFER_CAST(PUCHAR, buf),
                                 boost::asio::buffer_size(buf), &bytes_transferred, overlapped.get());
    DWORD last_error = ::GetLastError();
    if (!ok && last_error != ERROR_IO_PENDING && last_error != ERROR_MORE_DATA)
    {
        boost::system::error_code ec(last_error, boost::asio::error::system_category);
        overlapped.complete(ec, 0);
    }
    else
    {
        overlapped.release();
    }
}

void WinUsbDevice_Claim::AsyncWritePipe(uint8_t ep, boost::asio::mutable_buffer& buf,
                                        boost::function<void(const boost::system::error_code&, size_t)> handler)
{
    boost::mutex::scoped_lock lock(this_lock);
    AsyncWritePipeNoLock(ep, buf, handler);
}

void WinUsbDevice_Claim::AsyncWritePipeNoLock(uint8_t ep, boost::asio::mutable_buffer& buf,
                                              boost::function<void(const boost::system::error_code&, size_t)> handler)
{
    if (status != Claimed || !device_handle)
    {
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, boost::asio::error::bad_descriptor, 0),
                                                true);
        return;
    }

    RR_BOOST_ASIO_IO_CONTEXT& io = GetNode()->GetThreadPool()->get_io_context();

    boost::asio::windows::overlapped_ptr overlapped(io, handler);

    DWORD bytes_transferred = 0;

    BOOL ok = f->WinUsb_WritePipe(device_handle->hInterface.get(), ep, RR_BOOST_ASIO_BUFFER_CAST(PUCHAR, buf),
                                  boost::asio::buffer_size(buf), &bytes_transferred, overlapped.get());
    DWORD last_error = ::GetLastError();
    if (!ok && last_error != ERROR_IO_PENDING && last_error != ERROR_MORE_DATA)
    {
        boost::system::error_code ec(last_error, boost::asio::error::system_category);
        overlapped.complete(ec, 0);
    }
    else
    {
        overlapped.release();
    }
}

UsbDeviceStatus WinUsbDevice_Claim::ClaimDevice(RR_SHARED_PTR<void>& dev_h)
{
    UsbDeviceStatus res =
        WinUsbDevice_open_device(GetNode()->GetThreadPool()->get_io_context(), f, detected_device.path, dev_h);
    if (res != Open)
    {
        return res;
    }

    RR_SHARED_PTR<WinUsbDevice_Handle> h = RR_STATIC_POINTER_CAST<WinUsbDevice_Handle>(dev_h);

    BOOL b_true = TRUE;
    BOOL b_false = FALSE;
    if (!f->WinUsb_SetPipePolicy(h->hInterface.get(), settings->in_pipe_id, RAW_IO, sizeof(b_true), &b_true)
        /*|| !f->WinUsb_SetPipePolicy(hInterface1.get(), out_pipe_id, RAW_IO, sizeof(b_true), &b_true)*/
        || !f->WinUsb_SetPipePolicy(h->hInterface.get(), settings->in_pipe_id, ALLOW_PARTIAL_READS, sizeof(b_false),
                                    &b_false) ||
        !f->WinUsb_SetPipePolicy(h->hInterface.get(), settings->out_pipe_id, SHORT_PACKET_TERMINATE, sizeof(b_true),
                                 &b_true) ||
        !f->WinUsb_ResetPipe(h->hInterface.get(), settings->in_pipe_id) ||
        !f->WinUsb_ResetPipe(h->hInterface.get(), settings->out_pipe_id))
    {
        return Error;
    }

    dev_h = h;

    device_handle = h;

    return Open;
}

void WinUsbDevice_Claim::ReleaseClaim()
{
    if (!device_handle)
        return;

    device_handle.reset();
}

void WinUsbDevice_Claim::DrawDownRequests(boost::function<void()> handler)
{
    if (!device_handle)
        return;

    f->WinUsb_AbortPipe(device_handle->hInterface.get(), settings->in_pipe_id);
    f->WinUsb_AbortPipe(device_handle->hInterface.get(), settings->out_pipe_id);

    RobotRaconteurNode::TryPostToThreadPool(node, handler, true);
}

void WinUsbDevice_Claim::ClearHalt(uint8_t ep)
{
    boost::mutex::scoped_lock lock(this_lock);

    if (!device_handle)
        return;

    f->WinUsb_ResetPipe(device_handle->hInterface.get(), ep);
}

// WinUsbDevice

WinUsbDevice::WinUsbDevice(RR_SHARED_PTR<WinUsbDeviceManager> parent, RR_SHARED_PTR<WinUsb_Functions> f,
                           const UsbDeviceManager_detected_device& device)
    : UsbDevice(parent, device)
{

    this->f = f;
}

WinUsbDevice::~WinUsbDevice() {}

RR_SHARED_PTR<UsbDevice_Initialize> WinUsbDevice::CreateInitialize()
{
    return RR_MAKE_SHARED<WinUsbDevice_Initialize>(shared_from_this(), f, detected_device);
}
RR_SHARED_PTR<UsbDevice_Claim> WinUsbDevice::CreateClaim()
{
    return RR_MAKE_SHARED<WinUsbDevice_Claim>(shared_from_this(), f, detected_device);
}

// WinsockBluetoothConnector

WinsockBluetoothConnector::WinsockBluetoothConnector(RR_SHARED_PTR<HardwareTransport> parent)
    : BluetoothConnector(parent)
{}

std::list<SOCKADDR_BTH> WinsockBluetoothConnector::GetDeviceAddresses()
{
    std::list<SOCKADDR_BTH> o;
    WSAQUERYSETW wsaq;
    HANDLE hLookup;
    union {
        CHAR buf[5000];
        double __unused; // ensure proper alignment
    };
    LPWSAQUERYSETW pwsaResults = (LPWSAQUERYSETW)buf;
    DWORD dwSize = sizeof(buf);
    ZeroMemory(&wsaq, sizeof(wsaq));
    wsaq.dwSize = sizeof(wsaq);
    wsaq.dwNameSpace = NS_BTH;
    wsaq.lpcsaBuffer = NULL;
    if (ERROR_SUCCESS != WSALookupServiceBeginW(&wsaq, LUP_CONTAINERS, &hLookup))
    {
        return o;
    }
    ZeroMemory(pwsaResults, sizeof(WSAQUERYSET));
    pwsaResults->dwSize = sizeof(WSAQUERYSET);
    pwsaResults->dwNameSpace = NS_BTH;
    pwsaResults->lpBlob = NULL;
    while (ERROR_SUCCESS == WSALookupServiceNextW(hLookup, LUP_RETURN_ADDR, &dwSize, pwsaResults))
    {
        if (pwsaResults->dwNumberOfCsAddrs != 1)
            continue;
        SOCKADDR_BTH b = *((SOCKADDR_BTH*)pwsaResults->lpcsaBuffer->RemoteAddr.lpSockaddr);
        o.push_back(b);
        dwSize = sizeof(buf);
    }
    WSALookupServiceEnd(hLookup);

    return o;
}

std::list<BluetoothConnector<SOCKADDR_BTH, AF_BTH, BTHPROTO_RFCOMM>::device_info> WinsockBluetoothConnector::
    GetDeviceNodes(SOCKADDR_BTH addr)
{
    std::list<device_info> o;

    DWORD flags = LUP_FLUSHCACHE | LUP_RETURN_ALL | LUP_RES_SERVICE;

    WSAQUERYSETW wsaq;
    HANDLE hLookup;
    union {
        CHAR buf[5000];
        double __unused; // ensure proper alignment
    };
    LPWSAQUERYSETW pwsaResults = (LPWSAQUERYSETW)buf;
    DWORD dwSize = sizeof(buf);
    ZeroMemory(&wsaq, sizeof(wsaq));
    wsaq.dwSize = sizeof(wsaq);
    wsaq.dwNameSpace = NS_BTH;
    wsaq.lpcsaBuffer = NULL;

    wchar_t address_str[40];
    DWORD address_str_len = sizeof(address_str) / sizeof(wchar_t);
    if (ERROR_SUCCESS != WSAAddressToStringW((LPSOCKADDR)&addr, sizeof(addr), NULL, address_str, &address_str_len))
    {
        return o;
    }

    GUID guid = GUID_RobotRaconteurBluetoothServiceClassID;

    wsaq.lpszContext = address_str;
    wsaq.lpServiceClassId = &guid;

    if (ERROR_SUCCESS != WSALookupServiceBeginW(&wsaq, flags, &hLookup))
    {
        return o;
    }
    ZeroMemory(pwsaResults, sizeof(WSAQUERYSET));
    pwsaResults->dwSize = sizeof(WSAQUERYSET);
    pwsaResults->dwNameSpace = NS_BTH;
    pwsaResults->lpBlob = NULL;
    while (ERROR_SUCCESS == WSALookupServiceNextW(hLookup, flags, &dwSize, pwsaResults))
    {
        if (pwsaResults->dwNumberOfCsAddrs < 1)
            continue;
        if (pwsaResults->lpcsaBuffer->iProtocol != BTHPROTO_RFCOMM)
            continue;

        SOCKADDR_BTH b = *((SOCKADDR_BTH*)pwsaResults->lpcsaBuffer->RemoteAddr.lpSockaddr);

        boost::asio::mutable_buffer raw_record(pwsaResults->lpBlob->pBlobData, pwsaResults->lpBlob->cbSize);

        device_info info;
        info.addr = b;

        if (!ReadSdpRecord(raw_record, info))
        {
            continue;
        }

        o.push_back(info);

        dwSize = sizeof(buf);
    }
    WSALookupServiceEnd(hLookup);
    return o;
}

// Device driver interface search

boost::optional<std::wstring> HardwareTransport_win_find_deviceinterface(RR_SHARED_PTR<void> f,
                                                                         const GUID* interface_guid,
                                                                         const NodeID& nodeid,
                                                                         boost::string_ref nodename)
{
    if (!f)
    {
        return boost::optional<std::wstring>();
    }

    RR_SHARED_PTR<SetupApi_Functions> f1 = RR_STATIC_POINTER_CAST<SetupApi_Functions>(f);

    HDEVINFO deviceInfo;
    RR_SHARED_PTR<void> deviceInfo_sp;
    SP_DEVICE_INTERFACE_DATA interfaceData;
    BOOL bResult;

    deviceInfo = f1->SetupDiGetClassDevsW(interface_guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (deviceInfo == INVALID_HANDLE_VALUE)
    {
        return boost::optional<std::wstring>();
    }

    deviceInfo_sp = RR_SHARED_PTR<void>(
        deviceInfo, boost::bind(&SetupApi_Functions_SetupDiDestroyDeviceInfoList, f1, RR_BOOST_PLACEHOLDERS(_1)));

    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    DWORD i = 0;

    while (true)
    {
        bResult = f1->SetupDiEnumDeviceInterfaces(deviceInfo, NULL, &GUID_DEVINTERFACE_RobotRaconteurUSBDriver, i,
                                                  &interfaceData);

        i++;

        if (!bResult)
        {
            if (GetLastError() == ERROR_NO_MORE_ITEMS)
            {
                break;
            }
            else
            {
                continue;
            }
        }

        wchar_t property_buffer[256];
        memset(property_buffer, 0, sizeof(property_buffer));
        DEVPROPKEY nodename_key;
        nodename_key.fmtid = GUID_PROPERTY_RobotRaconteurNodeName;
        nodename_key.pid = 2;
        DEVPROPTYPE proptype;
        bResult = f1->SetupDiGetDeviceInterfacePropertyW(deviceInfo, &interfaceData, &nodename_key, &proptype,
                                                         (PBYTE)property_buffer, sizeof(property_buffer), NULL, 0);

        if (FALSE == bResult)
        {
            continue;
        }

        if (proptype != DEVPROP_TYPE_STRING)
        {
            continue;
        }

        std::wstring nodename2(property_buffer);

        memset(property_buffer, 0, sizeof(property_buffer));
        DEVPROPKEY nodeid_key;
        nodeid_key.fmtid = GUID_PROPERTY_RobotRaconteurNodeID;
        nodeid_key.pid = 2;
        DEVPROPTYPE proptype2;
        bResult = f1->SetupDiGetDeviceInterfacePropertyW(deviceInfo, &interfaceData, &nodeid_key, &proptype2,
                                                         (PBYTE)property_buffer, sizeof(property_buffer), NULL, 0);

        if (FALSE == bResult)
        {
            continue;
        }

        if (proptype2 != DEVPROP_TYPE_STRING)
        {
            continue;
        }
        std::wstring nodeid2_str(property_buffer);

        try
        {
            std::string nodeid2_str1 = boost::locale::conv::utf_to_utf<char>(nodeid2_str);
            NodeID nodeid2(nodeid2_str1);
            std::string nodename2_2 = boost::locale::conv::utf_to_utf<char>(nodename2);

            if (!nodeid.IsAnyNode() && !nodename.empty())
            {
                if (nodeid != nodeid2 || nodename != nodename2_2)
                {
                    continue;
                }
            }
            else if (nodeid.IsAnyNode() && !nodename.empty())
            {
                if (nodename != nodename2_2)
                {
                    continue;
                }
            }
            else if (!nodeid.IsAnyNode() && nodename.empty())
            {
                if (nodename != nodename2_2)
                {
                    continue;
                }
            }
            else
            {
                continue;
            }

            // We have found a match if we made it this far...

            DWORD requiredLength;
            RR_SHARED_PTR<SP_DEVICE_INTERFACE_DETAIL_DATA_W> detailData;

            //
            // Get the size of the path string
            // We expect to get a failure with insufficient buffer
            //
            bResult = f1->SetupDiGetDeviceInterfaceDetailW(deviceInfo, &interfaceData, NULL, 0, &requiredLength, NULL);

            if (FALSE == bResult && ERROR_INSUFFICIENT_BUFFER != GetLastError())
            {

                continue;
            }

            //
            // Allocate temporary space for SetupDi structure
            //
            detailData.reset((PSP_DEVICE_INTERFACE_DETAIL_DATA_W)LocalAlloc(LMEM_FIXED, requiredLength), LocalFree);

            if (NULL == detailData)
            {
                continue;
            }

            detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
            DWORD length = requiredLength;

            //
            // Get the interface's path string
            //
            bResult = f1->SetupDiGetDeviceInterfaceDetailW(deviceInfo, &interfaceData, detailData.get(), length,
                                                           &requiredLength, NULL);

            if (FALSE == bResult)
            {
                continue;
            }

            return std::wstring(detailData->DevicePath);
        }
        catch (std::exception&)
        {}
    }

    return boost::optional<std::wstring>();
}

boost::optional<std::wstring> HardwareTransport_win_find_usb(RR_SHARED_PTR<void> f, const NodeID& nodeid,
                                                             boost::string_ref nodename)
{
    return HardwareTransport_win_find_deviceinterface(f, &GUID_DEVINTERFACE_RobotRaconteurUSBDriver, nodeid, nodename);
}

boost::optional<std::wstring> HardwareTransport_win_find_pci(RR_SHARED_PTR<void> f, const NodeID& nodeid,
                                                             boost::string_ref nodename)
{
    return HardwareTransport_win_find_deviceinterface(f, &GUID_DEVINTERFACE_RobotRaconteurPCIDriver, nodeid, nodename);
}

boost::optional<std::wstring> HardwareTransport_win_find_bluetooth(RR_SHARED_PTR<void> f, const NodeID& nodeid,
                                                                   boost::string_ref nodename)
{
    return HardwareTransport_win_find_deviceinterface(f, &GUID_DEVINTERFACE_RobotRaconteurBluetoothDriver, nodeid,
                                                      nodename);
}

HardwareTransport_win_discovery::HardwareTransport_win_discovery(RR_SHARED_PTR<HardwareTransport> parent,
                                                                 const std::vector<std::string>& schemes,
                                                                 RR_SHARED_PTR<WinUsbDeviceManager> usb,
                                                                 RR_SHARED_PTR<WinsockBluetoothConnector> bt,
                                                                 RR_SHARED_PTR<void> f_void)
    : HardwareTransport_discovery(parent, schemes, usb, bt)
{}

void HardwareTransport_win_discovery::Init()
{
    HardwareTransport_discovery::Init();

    running = true;
    window_thread =
        boost::thread(boost::bind(&HardwareTransport_win_discovery::MessageWindowFunc,
                                  RR_STATIC_POINTER_CAST<HardwareTransport_win_discovery>(shared_from_this())));
}

void HardwareTransport_win_discovery::Close()
{
    HardwareTransport_discovery::Close();

    boost::thread t1;

    {
        boost::mutex::scoped_lock lock(this_lock);
        running = false;
        if (m_Wnd)
        {
            ::PostMessageW(m_Wnd, WM_DESTROY, 0, 0);
        }

        t1.swap(window_thread);
    }

    if (t1.joinable())
    {
        t1.join();
    }
}

std::vector<NodeDiscoveryInfo> HardwareTransport_win_discovery::GetDriverDevices()
{
    std::vector<NodeDiscoveryInfo> v1 = GetDriverDevices1(&GUID_DEVINTERFACE_RobotRaconteurUSBDriver, "rr+usb");
    std::vector<NodeDiscoveryInfo> v2 = GetDriverDevices1(&GUID_DEVINTERFACE_RobotRaconteurPCIDriver, "rr+pci");
    std::vector<NodeDiscoveryInfo> v3 =
        GetDriverDevices1(&GUID_DEVINTERFACE_RobotRaconteurBluetoothDriver, "rr+bluetooth");

    std::vector<NodeDiscoveryInfo> o;
    boost::range::copy(v1, std::back_inserter(o));
    boost::range::copy(v2, std::back_inserter(o));
    boost::range::copy(v3, std::back_inserter(o));
    return o;
}

std::vector<NodeDiscoveryInfo> HardwareTransport_win_discovery::GetDriverDevices1(const GUID* guid,
                                                                                  const std::string& scheme)
{
    std::vector<NodeDiscoveryInfo> o;

    try
    {
        RR_SHARED_PTR<RobotRaconteurNode> node = GetParent()->GetNode();

        std::list<boost::tuple<NodeID, std::string> > usb_dev = HardwareTransport_win_find_deviceinterfaces(f, guid);
        typedef boost::tuple<NodeID, std::string> e_type;

        BOOST_FOREACH (e_type& e, usb_dev)
        {
            NodeDiscoveryInfo n;
            n.NodeID = e.get<0>();
            n.NodeName = e.get<1>();
            NodeDiscoveryInfoURL n1;
            n1.URL = scheme + ":///?nodeid=" + e.get<0>().ToString("D") + "&service=RobotRaconteurServiceIndex";
            n1.LastAnnounceTime = node->NowNodeTime();
            n.URLs.push_back(n1);
            o.push_back(n);
        }
    }
    catch (std::exception&)
    {}

    return o;
}

// https://stackoverflow.com/questions/21369256/how-to-use-wndproc-as-a-class-function

LRESULT CALLBACK HardwareTransport_win_discovery::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HardwareTransport_win_discovery* pThis;

    if (msg == WM_NCCREATE)
    {
        pThis = static_cast<HardwareTransport_win_discovery*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

        SetLastError(0);
        if (!SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis)))
        {
            if (GetLastError() != 0)
                return FALSE;
        }

        return TRUE;
    }
    else
    {
        pThis = reinterpret_cast<HardwareTransport_win_discovery*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        switch (msg)
        {
        case WM_DEVICECHANGE:
            switch (wParam)
            {
            case DBT_DEVICEARRIVAL:
            case DBT_DEVICEREMOVECOMPLETE: {
                PDEV_BROADCAST_HDR dev_header = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
                if (dev_header->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
                {
                    PDEV_BROADCAST_DEVICEINTERFACE_W int_header =
                        reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE_W>(lParam);
                    GUID g = int_header->dbcc_classguid;
                    pThis->OnDeviceChanged_win(g, wParam);
                }
                break;
            }
            case DBT_CUSTOMEVENT: {
                PDEV_BROADCAST_HDR dev_header = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
                if (dev_header->dbch_devicetype == DBT_DEVTYP_HANDLE)
                {
                    PDEV_BROADCAST_HANDLE int_header = reinterpret_cast<PDEV_BROADCAST_HANDLE>(lParam);
                    GUID g = int_header->dbch_eventguid;
                    pThis->OnBluetoothChanged_win(g, wParam, int_header->dbch_data);
                }
            }
            default:
                break;
            }

            return TRUE;
        case WM_DESTROY:
            return TRUE;
        default:
            // We don't care about most messages
            break;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static BOOL DoRegisterDeviceInterfaceToHwnd(const GUID* InterfaceClassGuid, HWND hWnd, HDEVNOTIFY* hDeviceNotify)
{
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = *InterfaceClassGuid;

    *hDeviceNotify = RegisterDeviceNotificationW(hWnd,                       // events recipient
                                                 &NotificationFilter,        // type of device
                                                 DEVICE_NOTIFY_WINDOW_HANDLE // type of recipient handle
    );

    if (NULL == *hDeviceNotify)
    {
        return FALSE;
    }

    return TRUE;
}

static BOOL DoRegisterBluetoothRadioToHwd(RR_SHARED_PTR<void> f, HWND hWnd, RR_SHARED_PTR<void>& hDeviceNotify,
                                          RR_SHARED_PTR<void>& hRadio)
{
    if (!f)
        return FALSE;

    RR_SHARED_PTR<SetupApi_Functions> setupapi_f = RR_STATIC_POINTER_CAST<SetupApi_Functions>(f);

    RR_SHARED_PTR<HANDLE> hRadio1;

    HDEVINFO deviceInfo;
    RR_SHARED_PTR<void> deviceInfo_sp;
    SP_DEVICE_INTERFACE_DATA interfaceData;
    BOOL bResult;
    std::list<std::wstring> paths;

    deviceInfo = setupapi_f->SetupDiGetClassDevsW(&GUID_BTHPORT_DEVICE_INTERFACE, NULL, NULL,
                                                  DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (deviceInfo == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    deviceInfo_sp = RR_SHARED_PTR<void>(deviceInfo_sp, setupapi_f->SetupDiDestroyDeviceInfoList);

    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    DWORD i = 0;

    bResult =
        setupapi_f->SetupDiEnumDeviceInterfaces(deviceInfo, NULL, &GUID_BTHPORT_DEVICE_INTERFACE, i, &interfaceData);

    if (!bResult)
    {
        return FALSE;
    }

    DWORD requiredLength;
    RR_SHARED_PTR<SP_DEVICE_INTERFACE_DETAIL_DATA_W> detailData;

    //
    // Get the size of the path string
    // We expect to get a failure with insufficient buffer
    //
    bResult = setupapi_f->SetupDiGetDeviceInterfaceDetailW(deviceInfo, &interfaceData, NULL, 0, &requiredLength, NULL);

    if (FALSE == bResult && ERROR_INSUFFICIENT_BUFFER != GetLastError())
    {

        return FALSE;
    }

    //
    // Allocate temporary space for SetupDi structure
    //
    detailData.reset((PSP_DEVICE_INTERFACE_DETAIL_DATA_W)LocalAlloc(LMEM_FIXED, requiredLength), LocalFree);

    if (NULL == detailData)
    {
        return FALSE;
    }

    detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
    DWORD length = requiredLength;

    //
    // Get the interface's path string
    //
    bResult = setupapi_f->SetupDiGetDeviceInterfaceDetailW(deviceInfo, &interfaceData, detailData.get(), length,
                                                           &requiredLength, NULL);

    if (FALSE == bResult)
    {
        return FALSE;
    }

    HANDLE h = CreateFileW(detailData->DevicePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ,
                           NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
    if (h == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    RR_SHARED_PTR<void> h_sp(h, CloseHandle);

    DEV_BROADCAST_HANDLE NotificationFilter;
    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));

    NotificationFilter.dbch_size = sizeof(NotificationFilter);
    NotificationFilter.dbch_devicetype = DBT_DEVTYP_HANDLE;
    NotificationFilter.dbch_handle = h;

    HDEVINFO hDeviceNotify1 = RegisterDeviceNotificationW(hWnd,                       // events recipient
                                                          &NotificationFilter,        // type of device
                                                          DEVICE_NOTIFY_WINDOW_HANDLE // type of recipient handle
    );

    if (!hDeviceNotify1)
        return FALSE;

    hDeviceNotify.reset(hDeviceNotify1, &UnregisterDeviceNotification);

    hRadio = h_sp;

    return TRUE;
}

// Create a window to receive device change notification
HWND HardwareTransport_win_discovery::CreateMessageWindow()
{
    static const wchar_t* class_name = L"ROBOTRACONTEUR_MESSAGE_WINDOW_CLASS";
    WNDCLASSEXW wx = {};
    wx.cbSize = sizeof(WNDCLASSEXW);
    wx.lpfnWndProc = &HardwareTransport_win_discovery::WndProc; // function which will handle messages
    wx.hInstance = NULL;
    wx.lpszClassName = class_name;
    if (!RegisterClassExW(&wx))
    {
        throw SystemResourceException("Could not initialize device update window");
    }

    HWND w = CreateWindowExW(0, class_name, L"rr_message_window", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, this);

    if (!w)
    {
        throw SystemResourceException("Could not initialize device update window");
    }

    std::list<GUID> guids;
    guids.push_back(GUID_DEVINTERFACE_RobotRaconteurUSBDriver);
    guids.push_back(GUID_DEVINTERFACE_RobotRaconteurPCIDriver);
    guids.push_back(GUID_DEVINTERFACE_RobotRaconteurBluetoothDriver);
    guids.push_back(GUID_DEVINTERFACE_RobotRaconteurWinUsbDevice);
    guids.push_back(GUID_BTHPORT_DEVICE_INTERFACE);

    BOOST_FOREACH (GUID& guid, guids)
    {
        HDEVNOTIFY hDeviceNotify;
        if (!DoRegisterDeviceInterfaceToHwnd(&guid, w, &hDeviceNotify))
        {
            throw SystemResourceException("Could not initialize device update window");
        }

        this->hDeviceNotify.push_back(RR_SHARED_PTR<void>(hDeviceNotify, &UnregisterDeviceNotification));
    }

    RR_SHARED_PTR<void> bt_notify1;
    RR_SHARED_PTR<void> bt_radio1;

    if (DoRegisterBluetoothRadioToHwd(this->f, w, bt_notify1, bt_radio1))
    {
        hBtNotify.push_back(bt_notify1);
        hBtNotify.push_back(bt_radio1);
    }

    return w;
}

void HardwareTransport_win_discovery::OnDeviceChanged_win(GUID guid, DWORD evt)
{
    try
    {
        RR_SHARED_PTR<RobotRaconteurNode> n = GetParent()->GetNode();
        RR_SHARED_PTR<ThreadPool> p = n->GetThreadPool();

        if (IsEqualGUID(guid, GUID_BTHPORT_DEVICE_INTERFACE))
        {
            std::cout << "Got a refresh adapter notification" << std::endl;

            hBtNotify.clear();

            RR_SHARED_PTR<void> bt_notify1;
            RR_SHARED_PTR<void> bt_radio1;

            if (DoRegisterBluetoothRadioToHwd(this->f, m_Wnd, bt_notify1, bt_radio1))
            {
                hBtNotify.push_back(bt_notify1);
                hBtNotify.push_back(bt_radio1);
            }

            return;
        }

        if (IsEqualGUID(guid, GUID_DEVINTERFACE_RobotRaconteurUSBDriver))
        {
            std::vector<NodeDiscoveryInfo> v1 = GetDriverDevices1(&GUID_DEVINTERFACE_RobotRaconteurUSBDriver, "rr+usb");
            BOOST_FOREACH (NodeDiscoveryInfo& v2, v1)
            {
                n->NodeDetected(v2);
            }
            return;
        }

        if (IsEqualGUID(guid, GUID_DEVINTERFACE_RobotRaconteurPCIDriver))
        {
            std::vector<NodeDiscoveryInfo> v1 = GetDriverDevices1(&GUID_DEVINTERFACE_RobotRaconteurPCIDriver, "rr+pci");
            BOOST_FOREACH (NodeDiscoveryInfo& v2, v1)
            {
                n->NodeDetected(v2);
            }
            return;
        }

        if (IsEqualGUID(guid, GUID_DEVINTERFACE_RobotRaconteurBluetoothDriver))
        {
            std::vector<NodeDiscoveryInfo> v1 =
                GetDriverDevices1(&GUID_DEVINTERFACE_RobotRaconteurBluetoothDriver, "rr+bluetooth");
            BOOST_FOREACH (NodeDiscoveryInfo& v2, v1)
            {
                n->NodeDetected(v2);
            }
            return;
        }

        if (IsEqualGUID(guid, GUID_DEVINTERFACE_RobotRaconteurWinUsbDevice))
        {
            GetUsbDevices(boost::bind(&HardwareTransport_win_discovery::OnDeviceChanged, shared_from_this(),
                                      RR_BOOST_PLACEHOLDERS(_1)));
            return;
        }
    }
    catch (std::exception&)
    {}
}

void HardwareTransport_win_discovery::OnBluetoothChanged_win(GUID guid, DWORD evt, PBYTE buf)
{
    try
    {
        if (IsEqualGUID(guid, GUID_BLUETOOTH_RADIO_IN_RANGE))
        {
            SOCKADDR_BTH addr;
            ZeroMemory(&addr, sizeof(addr));
            addr.addressFamily = AF_BTH;
            addr.btAddr = reinterpret_cast<PBTH_RADIO_IN_RANGE>(buf)->deviceInfo.address;

            OnBluetoothChanged(addr);

            return;
        }
        if (IsEqualGUID(guid, GUID_BLUETOOTH_RADIO_OUT_OF_RANGE))
        {
            SOCKADDR_BTH addr;
            ZeroMemory(&addr, sizeof(addr));
            addr.addressFamily = AF_BTH;
            addr.btAddr = *reinterpret_cast<PBTH_ADDR>(buf);

            return;
        }
        if (IsEqualGUID(guid, GUID_BLUETOOTH_HCI_EVENT))
        {
            SOCKADDR_BTH addr;
            ZeroMemory(&addr, sizeof(addr));
            addr.addressFamily = AF_BTH;
            addr.btAddr = reinterpret_cast<PBTH_HCI_EVENT_INFO>(buf)->bthAddress;

            OnBluetoothChanged(addr);

            return;
        }
    }
    catch (std::exception&)
    {}
}

void HardwareTransport_win_discovery::MessageWindowFunc()
{
    try
    {
        boost::mutex::scoped_lock lock(this_lock);

        if (!running)
            return;

        m_Wnd = CreateMessageWindow();

        MSG msg;
        BOOL bRet;

        while (true)
        {
            lock.unlock();
            bRet = ::GetMessageW(&msg, m_Wnd, 0, 0);
            lock.lock();

            if (bRet == 0 || !running)
            {
                DestroyWindow(m_Wnd);
                m_Wnd = NULL;
                return;
            }

            if (bRet == -1)
            {
                throw SystemResourceException("Internal error in device update window");
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    catch (std::exception& err)
    {
        try
        {
            GetParent()->GetNode()->HandleException(&err);
        }
        catch (std::exception&)
        {}
        return;
    }
}

std::list<boost::tuple<NodeID, std::string> > HardwareTransport_win_find_deviceinterfaces(RR_SHARED_PTR<void> f,
                                                                                          const GUID* interface_guid)
{

    std::list<boost::tuple<NodeID, std::string> > o;

    if (!f)
    {
        return o;
    }

    HDEVINFO deviceInfo;
    RR_SHARED_PTR<void> deviceInfo_sp;
    SP_DEVICE_INTERFACE_DATA interfaceData;
    BOOL bResult;

    RR_SHARED_PTR<SetupApi_Functions> f1 = RR_STATIC_POINTER_CAST<SetupApi_Functions>(f);

    deviceInfo = f1->SetupDiGetClassDevsW(interface_guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (deviceInfo == INVALID_HANDLE_VALUE)
    {
        return o;
    }

    deviceInfo_sp = RR_SHARED_PTR<void>(deviceInfo_sp, f1->SetupDiDestroyDeviceInfoList);

    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    DWORD i = 0;

    while (true)
    {
        bResult = f1->SetupDiEnumDeviceInterfaces(deviceInfo, NULL, &GUID_DEVINTERFACE_RobotRaconteurUSBDriver, i,
                                                  &interfaceData);

        i++;

        if (!bResult)
        {
            if (GetLastError() == ERROR_NO_MORE_ITEMS)
            {
                break;
            }
            else
            {
                continue;
            }
        }

        wchar_t property_buffer[256];
        memset(property_buffer, 0, sizeof(property_buffer));
        DEVPROPKEY nodename_key;
        nodename_key.fmtid = GUID_PROPERTY_RobotRaconteurNodeName;
        nodename_key.pid = 2;
        DEVPROPTYPE proptype;
        bResult = f1->SetupDiGetDeviceInterfacePropertyW(deviceInfo, &interfaceData, &nodename_key, &proptype,
                                                         (PBYTE)property_buffer, sizeof(property_buffer), NULL, 0);

        if (FALSE == bResult)
        {
            continue;
        }

        if (proptype != DEVPROP_TYPE_STRING)
        {
            continue;
        }

        std::wstring nodename2(property_buffer);

        memset(property_buffer, 0, sizeof(property_buffer));
        DEVPROPKEY nodeid_key;
        nodeid_key.fmtid = GUID_PROPERTY_RobotRaconteurNodeID;
        nodeid_key.pid = 2;
        DEVPROPTYPE proptype2;
        bResult = f1->SetupDiGetDeviceInterfacePropertyW(deviceInfo, &interfaceData, &nodeid_key, &proptype2,
                                                         (PBYTE)property_buffer, sizeof(property_buffer), NULL, 0);

        if (FALSE == bResult)
        {
            continue;
        }

        if (proptype2 != DEVPROP_TYPE_STRING)
        {
            continue;
        }
        std::wstring nodeid2_str(property_buffer);

        try
        {
            std::string nodeid2_str1 = boost::locale::conv::utf_to_utf<char>(nodeid2_str);
            NodeID nodeid2(nodeid2_str1);
            std::string nodename2_2 = boost::locale::conv::utf_to_utf<char>(nodename2);

            o.push_back(boost::make_tuple(nodeid2, nodename2_2));
        }
        catch (std::exception&)
        {}
    }
    return o;
}

} // namespace detail

} // namespace RobotRaconteur