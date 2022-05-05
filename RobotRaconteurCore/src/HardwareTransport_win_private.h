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

#include "RobotRaconteur/HardwareTransport.h"
#include "RobotRaconteur/ASIOStreamBaseTransport.h"

#include "HardwareTransport_usbcommon_private.h"
#include "HardwareTransport_bluetoothcommon_private.h"
#include "HardwareTransport_discoverycommon_private.h"

#include <Ws2bth.h>
#include <windows.h>
#include <Devpropdef.h>
#pragma once

// Begin SetupApi dynamic loading

#ifdef __cplusplus
extern "C"
{
#endif

#pragma pack(1)
    typedef PVOID HDEVINFO;

    typedef struct _SP_DEVICE_INTERFACE_DATA
    {
        DWORD cbSize;
        GUID InterfaceClassGuid;
        DWORD Flags;
        ULONG_PTR Reserved;
    } SP_DEVICE_INTERFACE_DATA, *PSP_DEVICE_INTERFACE_DATA;

    typedef struct _SP_DEVICE_INTERFACE_DETAIL_DATA_W
    {
        DWORD cbSize;
        WCHAR DevicePath[ANYSIZE_ARRAY];
    } SP_DEVICE_INTERFACE_DETAIL_DATA_W, *PSP_DEVICE_INTERFACE_DETAIL_DATA_W;

    typedef struct _SP_DEVINFO_DATA
    {
        DWORD cbSize;
        GUID ClassGuid;
        DWORD DevInst; // DEVINST handle
        ULONG_PTR Reserved;
    } SP_DEVINFO_DATA, *PSP_DEVINFO_DATA;

    /*typedef GUID  DEVPROPGUID, *PDEVPROPGUID;
    typedef ULONG DEVPROPID, *PDEVPROPID;

    typedef struct _DEVPROPKEY {
        DEVPROPGUID fmtid;
        DEVPROPID   pid;
    } DEVPROPKEY, *PDEVPROPKEY;

    typedef ULONG DEVPROPTYPE, *PDEVPROPTYPE;*/

#pragma pack()

#define DIGCF_DEFAULT 0x00000001
#define DIGCF_PRESENT 0x00000002
#define DIGCF_ALLCLASSES 0x00000004
#define DIGCF_PROFILE 0x00000008
#define DIGCF_DEVICEINTERFACE 0x00000010

#define DEVPROP_TYPE_STRING 0x00000012

    typedef BOOL(WINAPI* SetupDiGetDeviceInterfacePropertyW_t)(HDEVINFO DeviceInfoSet,
                                                               PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,
                                                               CONST DEVPROPKEY* PropertyKey, DEVPROPTYPE* PropertyType,
                                                               PBYTE PropertyBuffer, DWORD PropertyBufferSize,
                                                               PDWORD RequiredSize, DWORD Flags);
    typedef HDEVINFO(WINAPI* SetupDiGetClassDevsW_t)(CONST GUID* ClassGuid, PCWSTR Enumerator, HWND hwndParent,
                                                     DWORD Flags);
    typedef BOOL(WINAPI* SetupDiDestroyDeviceInfoList_t)(HDEVINFO DeviceInfoSet);
    typedef BOOL(WINAPI* SetupDiEnumDeviceInterfaces_t)(HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData,
                                                        CONST GUID* InterfaceClassGuid, DWORD MemberIndex,
                                                        PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData);
    typedef BOOL(WINAPI* SetupDiGetDeviceInterfaceDetailW_t)(
        HDEVINFO DeviceInfoSet, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,
        PSP_DEVICE_INTERFACE_DETAIL_DATA_W DeviceInterfaceDetailData, DWORD DeviceInterfaceDetailDataSize,
        PDWORD RequiredSize, PSP_DEVINFO_DATA DeviceInfoData);

#ifdef __cplusplus
}
#endif

#define SETUPAPI_FUNCTIONS_INIT(M)                                                                                     \
    M(SetupDiGetDeviceInterfacePropertyW)                                                                              \
    M(SetupDiGetClassDevsW)                                                                                            \
    M(SetupDiDestroyDeviceInfoList)                                                                                    \
    M(SetupDiEnumDeviceInterfaces)                                                                                     \
    M(SetupDiGetDeviceInterfaceDetailW)

#define SETUPAPI_FUNCTIONS_FIELD_INIT(t) t##_t t;
#define SETUPAPI_FUNCTIONS_PTR_INIT(t)                                                                                 \
    t = (t##_t)GetProcAddress(hLibModule, #t);                                                                         \
    if (t == NULL)                                                                                                     \
        return false;

namespace RobotRaconteur
{
class SetupApi_Functions
{
  private:
    HMODULE hLibModule;

  public:
    SETUPAPI_FUNCTIONS_INIT(SETUPAPI_FUNCTIONS_FIELD_INIT)

    SetupApi_Functions();
    bool LoadFunctions();
    virtual ~SetupApi_Functions();
};
} // namespace RobotRaconteur

// End SetupApi dynamic loading

// Begin WinUsb dynamic loading

#ifdef __cplusplus
extern "C"
{
#endif

// Pipe policy types
#define SHORT_PACKET_TERMINATE 0x01
#define AUTO_CLEAR_STALL 0x02
#define PIPE_TRANSFER_TIMEOUT 0x03
#define IGNORE_SHORT_PACKETS 0x04
#define ALLOW_PARTIAL_READS 0x05
#define AUTO_FLUSH 0x06
#define RAW_IO 0x07
#define MAXIMUM_TRANSFER_SIZE 0x08
#define RESET_PIPE_ON_RESUME 0x09

// Power policy types
//
// Add 0x80 for Power policy types in order to prevent overlap with
// Pipe policy types to prevent "accidentally" setting the wrong value for the
// wrong type.
//
#define AUTO_SUSPEND 0x81
#define SUSPEND_DELAY 0x83

// Device Information types
#define DEVICE_SPEED 0x01

// Device Speeds
#define LowSpeed 0x01
#define FullSpeed 0x02
#define HighSpeed 0x03

#define USB_ENDPOINT_DIRECTION_MASK 0x80
#define USB_ENDPOINT_DIRECTION_OUT(addr) (!((addr)&USB_ENDPOINT_DIRECTION_MASK))
#define USB_ENDPOINT_DIRECTION_IN(addr) ((addr)&USB_ENDPOINT_DIRECTION_MASK)

//
// USB 1.1: 9.4 Standard Device Requests, Table 9-5. Descriptor Types
//
#define USB_DEVICE_DESCRIPTOR_TYPE 0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE 0x02
#define USB_STRING_DESCRIPTOR_TYPE 0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE 0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE 0x05
#define USB_CS_INTERFACE_DESCRIPTOR_TYPE 0x24

    typedef PVOID WINUSB_INTERFACE_HANDLE, *PWINUSB_INTERFACE_HANDLE;

    typedef PVOID WINUSB_ISOCH_BUFFER_HANDLE, *PWINUSB_ISOCH_BUFFER_HANDLE;

#pragma pack(1)

    typedef struct _WINUSB_SETUP_PACKET
    {
        UCHAR RequestType;
        UCHAR Request;
        USHORT Value;
        USHORT Index;
        USHORT Length;
    } WINUSB_SETUP_PACKET, *PWINUSB_SETUP_PACKET;

    typedef enum _USBD_PIPE_TYPE
    {
        UsbdPipeTypeControl,
        UsbdPipeTypeIsochronous,
        UsbdPipeTypeBulk,
        UsbdPipeTypeInterrupt
    } USBD_PIPE_TYPE;

    typedef struct _WINUSB_PIPE_INFORMATION
    {
        USBD_PIPE_TYPE PipeType;
        UCHAR PipeId;
        USHORT MaximumPacketSize;
        UCHAR Interval;
        UCHAR pad[8];
    } WINUSB_PIPE_INFORMATION, *PWINUSB_PIPE_INFORMATION;

    typedef struct _WINUSB_PIPE_INFORMATION_EX
    {
        USBD_PIPE_TYPE PipeType;
        UCHAR PipeId;
        USHORT MaximumPacketSize;
        UCHAR Interval;
        ULONG MaximumBytesPerInterval;
    } WINUSB_PIPE_INFORMATION_EX, *PWINUSB_PIPE_INFORMATION_EX;

    //
    // USB 1.1: 9.6.3 Interface, Table 9-9. Standard Interface Descriptor
    // USB 2.0: 9.6.5 Interface, Table 9-12. Standard Interface Descriptor
    // USB 3.0: 9.6.5 Interface, Table 9-17. Standard Interface Descriptor
    //
    typedef struct _USB_INTERFACE_DESCRIPTOR
    {
        UCHAR bLength;
        UCHAR bDescriptorType;
        UCHAR bInterfaceNumber;
        UCHAR bAlternateSetting;
        UCHAR bNumEndpoints;
        UCHAR bInterfaceClass;
        UCHAR bInterfaceSubClass;
        UCHAR bInterfaceProtocol;
        UCHAR iInterface;
    } USB_INTERFACE_DESCRIPTOR, *PUSB_INTERFACE_DESCRIPTOR;

    C_ASSERT(sizeof(USB_INTERFACE_DESCRIPTOR) == 9);

    //
    // USB 1.1: 9.6.2 Configuration, Table 9-8. Standard Configuration Descriptor
    // USB 2.0: 9.6.3 Configuration, Table 9-10. Standard Configuration Descriptor
    // USB 3.0: 9.6.3 Configuration, Table 9-15. Standard Configuration Descriptor
    //
    typedef struct _USB_CONFIGURATION_DESCRIPTOR
    {
        UCHAR bLength;
        UCHAR bDescriptorType;
        USHORT wTotalLength;
        UCHAR bNumInterfaces;
        UCHAR bConfigurationValue;
        UCHAR iConfiguration;
        UCHAR bmAttributes;
        UCHAR MaxPower;
    } USB_CONFIGURATION_DESCRIPTOR, *PUSB_CONFIGURATION_DESCRIPTOR;

    C_ASSERT(sizeof(USB_CONFIGURATION_DESCRIPTOR) == 9);

    //
    // USB 1.1: 9.6.4 Endpoint, Table 9-10. Standard Endpoint Descriptor
    // USB 2.0: 9.6.6 Endpoint, Table 9-13. Standard Endpoint Descriptor
    // USB 3.0: 9.6.6 Endpoint, Table 9-18. Standard Endpoint Descriptor
    //
    typedef struct _USB_ENDPOINT_DESCRIPTOR
    {
        UCHAR bLength;
        UCHAR bDescriptorType;
        UCHAR bEndpointAddress;
        UCHAR bmAttributes;
        USHORT wMaxPacketSize;
        UCHAR bInterval;
    } USB_ENDPOINT_DESCRIPTOR, *PUSB_ENDPOINT_DESCRIPTOR;

    C_ASSERT(sizeof(USB_ENDPOINT_DESCRIPTOR) == 7);

    typedef struct _USB_COMMON_DESCRIPTOR
    {
        UCHAR bLength;
        UCHAR bDescriptorType;
    } USB_COMMON_DESCRIPTOR, *PUSB_COMMON_DESCRIPTOR;

    typedef struct _USBD_ISO_PACKET_DESCRIPTOR
    {
        ULONG Offset;
        ULONG Length;
        LONG Status;
    } USBD_ISO_PACKET_DESCRIPTOR, *PUSBD_ISO_PACKET_DESCRIPTOR;

#pragma pack()

    typedef BOOL(__stdcall* WinUsb_Initialize_t)(HANDLE DeviceHandle, PWINUSB_INTERFACE_HANDLE InterfaceHandle);
    typedef BOOL(__stdcall* WinUsb_Free_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle);
    typedef BOOL(__stdcall* WinUsb_GetAssociatedInterface_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle,
                                                             UCHAR AssociatedInterfaceIndex,
                                                             PWINUSB_INTERFACE_HANDLE AssociatedInterfaceHandle);
    typedef BOOL(__stdcall* WinUsb_GetDescriptor_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR DescriptorType,
                                                    UCHAR Index, USHORT LanguageID, PUCHAR Buffer, ULONG BufferLength,
                                                    PULONG LengthTransferred);
    typedef BOOL(__stdcall* WinUsb_QueryInterfaceSettings_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle,
                                                             UCHAR AlternateInterfaceNumber,
                                                             PUSB_INTERFACE_DESCRIPTOR UsbAltInterfaceDescriptor);
    typedef BOOL(__stdcall* WinUsb_QueryDeviceInformation_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle,
                                                             ULONG InformationType, PULONG BufferLength, PVOID Buffer);
    typedef BOOL(__stdcall* WinUsb_SetCurrentAlternateSetting_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle,
                                                                 UCHAR SettingNumber);
    typedef BOOL(__stdcall* WinUsb_GetCurrentAlternateSetting_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle,
                                                                 PUCHAR SettingNumber);
    typedef BOOL(__stdcall* WinUsb_QueryPipe_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR AlternateInterfaceNumber,
                                                UCHAR PipeIndex, PWINUSB_PIPE_INFORMATION PipeInformation);
    typedef BOOL(__stdcall* WinUsb_QueryPipeEx_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR AlternateSettingNumber,
                                                  UCHAR PipeIndex, PWINUSB_PIPE_INFORMATION_EX PipeInformationEx);
    typedef BOOL(__stdcall* WinUsb_SetPipePolicy_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID,
                                                    ULONG PolicyType, ULONG ValueLength, PVOID Value);
    typedef BOOL(__stdcall* WinUsb_GetPipePolicy_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID,
                                                    ULONG PolicyType, PULONG ValueLength, PVOID Value);
    typedef BOOL(__stdcall* WinUsb_ReadPipe_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID, PUCHAR Buffer,
                                               ULONG BufferLength, PULONG LengthTransferred, LPOVERLAPPED Overlapped);
    typedef BOOL(__stdcall* WinUsb_WritePipe_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID, PUCHAR Buffer,
                                                ULONG BufferLength, PULONG LengthTransferred, LPOVERLAPPED Overlapped);
    typedef BOOL(__stdcall* WinUsb_ControlTransfer_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle,
                                                      WINUSB_SETUP_PACKET SetupPacket, PUCHAR Buffer,
                                                      ULONG BufferLength, PULONG LengthTransferred,
                                                      LPOVERLAPPED Overlapped);
    typedef BOOL(__stdcall* WinUsb_ResetPipe_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID);
    typedef BOOL(__stdcall* WinUsb_AbortPipe_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID);
    typedef BOOL(__stdcall* WinUsb_FlushPipe_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID);
    typedef BOOL(__stdcall* WinUsb_SetPowerPolicy_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle, ULONG PolicyType,
                                                     ULONG ValueLength, PVOID Value);
    typedef BOOL(__stdcall* WinUsb_GetPowerPolicy_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle, ULONG PolicyType,
                                                     PULONG ValueLength, PVOID Value);
    typedef BOOL(__stdcall* WinUsb_GetOverlappedResult_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle,
                                                          LPOVERLAPPED lpOverlapped, LPDWORD lpNumberOfBytesTransferred,
                                                          BOOL bWait);
    typedef BOOL(__stdcall* WinUsb_GetCurrentFrameNumber_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle,
                                                            PULONG CurrentFrameNumber, LARGE_INTEGER* TimeStamp);
    typedef BOOL(__stdcall* WinUsb_GetAdjustedFrameNumber_t)(PULONG CurrentFrameNumber, LARGE_INTEGER TimeStamp);
    typedef BOOL(__stdcall* WinUsb_RegisterIsochBuffer_t)(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID,
                                                          PUCHAR Buffer, ULONG BufferLength,
                                                          PWINUSB_ISOCH_BUFFER_HANDLE IsochBufferHandle);
    typedef BOOL(__stdcall* WinUsb_UnregisterIsochBuffer_t)(WINUSB_ISOCH_BUFFER_HANDLE IsochBufferHandle);
    typedef BOOL(__stdcall* WinUsb_WriteIsochPipe_t)(WINUSB_ISOCH_BUFFER_HANDLE BufferHandle, ULONG Offset,
                                                     ULONG Length, PULONG FrameNumber, LPOVERLAPPED Overlapped);
    typedef BOOL(__stdcall* WinUsb_ReadIsochPipe_t)(WINUSB_ISOCH_BUFFER_HANDLE BufferHandle, ULONG Offset, ULONG Length,
                                                    PULONG FrameNumber, ULONG NumberOfPackets,
                                                    PUSBD_ISO_PACKET_DESCRIPTOR IsoPacketDescriptors,
                                                    LPOVERLAPPED Overlapped);
    typedef BOOL(__stdcall* WinUsb_WriteIsochPipeAsap_t)(WINUSB_ISOCH_BUFFER_HANDLE BufferHandle, ULONG Offset,
                                                         ULONG Length, BOOL ContinueStream, LPOVERLAPPED Overlapped);
    typedef BOOL(__stdcall* WinUsb_ReadIsochPipeAsap_t)(WINUSB_ISOCH_BUFFER_HANDLE BufferHandle, ULONG Offset,
                                                        ULONG Length, BOOL ContinueStream, ULONG NumberOfPackets,
                                                        PUSBD_ISO_PACKET_DESCRIPTOR IsoPacketDescriptors,
                                                        LPOVERLAPPED Overlapped);

#ifdef __cplusplus
}
#endif

// Begin Bluetooth structures
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

// End Bluetooth structures

namespace RobotRaconteur
{

class HardwareTransport;
class RobotRaconteurNode;

class SetupApi_Functions;

namespace detail
{

#define WINUSB_FUNCTIONS_INIT(M)                                                                                       \
    M(WinUsb_Initialize)                                                                                               \
    M(WinUsb_Free)                                                                                                     \
    M(WinUsb_GetAssociatedInterface)                                                                                   \
    M(WinUsb_GetDescriptor)                                                                                            \
    M(WinUsb_QueryInterfaceSettings)                                                                                   \
    M(WinUsb_QueryDeviceInformation)                                                                                   \
    M(WinUsb_SetCurrentAlternateSetting)                                                                               \
    M(WinUsb_GetCurrentAlternateSetting)                                                                               \
    M(WinUsb_QueryPipe)                                                                                                \
    M(WinUsb_QueryPipeEx)                                                                                              \
    M(WinUsb_SetPipePolicy)                                                                                            \
    M(WinUsb_GetPipePolicy)                                                                                            \
    M(WinUsb_ReadPipe)                                                                                                 \
    M(WinUsb_WritePipe)                                                                                                \
    M(WinUsb_ControlTransfer)                                                                                          \
    M(WinUsb_ResetPipe)                                                                                                \
    M(WinUsb_AbortPipe)                                                                                                \
    M(WinUsb_FlushPipe)                                                                                                \
    M(WinUsb_SetPowerPolicy)                                                                                           \
    M(WinUsb_GetPowerPolicy)                                                                                           \
    M(WinUsb_GetOverlappedResult)
#define WINUSB_ISOCH_FUNCTIONS_INIT(M)                                                                                 \
    M(WinUsb_GetCurrentFrameNumber)                                                                                    \
    M(WinUsb_GetAdjustedFrameNumber)                                                                                   \
    M(WinUsb_RegisterIsochBuffer)                                                                                      \
    M(WinUsb_UnregisterIsochBuffer)                                                                                    \
    M(WinUsb_WriteIsochPipe)                                                                                           \
    M(WinUsb_ReadIsochPipe)                                                                                            \
    M(WinUsb_WriteIsochPipeAsap)                                                                                       \
    M(WinUsb_ReadIsochPipeAsap)

#define WINUSB_FUNCTIONS_FIELD_INIT(t) t##_t t;
#define WINUSB_FUNCTIONS_PTR_INIT(t)                                                                                   \
    t = (t##_t)GetProcAddress(hLibModule, #t);                                                                         \
    if (t == NULL)                                                                                                     \
        return false;
#define WINUSB_FUNCTIONS_PTR_INIT_NOERR(t) t = (t##_t)GetProcAddress(hLibModule, #t);

class WinUsb_Functions : public boost::noncopyable
{
  private:
    HMODULE hLibModule;

  public:
    WINUSB_FUNCTIONS_INIT(WINUSB_FUNCTIONS_FIELD_INIT);
    WINUSB_ISOCH_FUNCTIONS_INIT(WINUSB_FUNCTIONS_FIELD_INIT);

    WinUsb_Functions();
    bool LoadFunctions();
    virtual ~WinUsb_Functions();
};

// End WinUsb dynamic loading stuff

class WinUsbDeviceManager;
class WinUsbDevice;
class WinUsbDevice_Claim;
class WinUsbDevice_Initialize;

class WinUsbDeviceManager : public UsbDeviceManager
{
  protected:
    RR_SHARED_PTR<WinUsb_Functions> f;
    RR_SHARED_PTR<SetupApi_Functions> setupapi_f;

  public:
    WinUsbDeviceManager(RR_SHARED_PTR<HardwareTransport> parent, RR_SHARED_PTR<SetupApi_Functions> setupapi_f);
    virtual ~WinUsbDeviceManager();

  protected:
    virtual RR_SHARED_PTR<UsbDevice> CreateDevice(const UsbDeviceManager_detected_device& device);

    virtual std::list<UsbDeviceManager_detected_device> GetDetectedDevicesPaths();

    // Call with lock
    virtual bool InitUpdateDevices();
};

class WinUsbDevice_Handle
{
  public:
    RR_SHARED_PTR<boost::asio::windows::stream_handle> device_handle;
    RR_SHARED_PTR<void> hInterface;

    virtual ~WinUsbDevice_Handle();
};

class WinUsbDevice_Initialize : public UsbDevice_Initialize
{
  public:
    WinUsbDevice_Initialize(RR_SHARED_PTR<UsbDevice> parent, RR_SHARED_PTR<WinUsb_Functions> f,
                            const UsbDeviceManager_detected_device& detected_device);
    virtual ~WinUsbDevice_Initialize() {}

    virtual void AsyncControlTransfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                      boost::asio::mutable_buffer& buf,
                                      boost::function<void(const boost::system::error_code&, size_t)> handler,
                                      RR_SHARED_PTR<void> dev_h = RR_SHARED_PTR<void>());

    // Call with lock
    virtual void AsyncControlTransferNoLock(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                            boost::asio::mutable_buffer& buf,
                                            boost::function<void(const boost::system::error_code&, size_t)> handler,
                                            RR_SHARED_PTR<void> dev_h = RR_SHARED_PTR<void>());

    // Call with lock
    virtual UsbDeviceStatus OpenDevice(RR_SHARED_PTR<void>& dev_h);

    // Call with lock
    virtual UsbDeviceStatus ReadPipeSettings(RR_SHARED_PTR<void> dev_h, RR_SHARED_PTR<UsbDevice_Settings>& settings);

    // Call with lock
    virtual UsbDeviceStatus ReadInterfaceSettings(RR_SHARED_PTR<void> dev_h,
                                                  RR_SHARED_PTR<UsbDevice_Settings>& settings);

  protected:
    RR_SHARED_PTR<WinUsb_Functions> f;
};

class WinUsbDevice_Claim : public UsbDevice_Claim
{
  public:
    WinUsbDevice_Claim(RR_SHARED_PTR<UsbDevice> parent, RR_SHARED_PTR<WinUsb_Functions> f,
                       const UsbDeviceManager_detected_device& detected_device);
    virtual ~WinUsbDevice_Claim() {}

  protected:
    virtual void AsyncControlTransfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                      boost::asio::mutable_buffer& buf,
                                      boost::function<void(const boost::system::error_code&, size_t)> handler,
                                      RR_SHARED_PTR<void> dev_h = RR_SHARED_PTR<void>());

    virtual void AsyncControlTransferNoLock(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                            boost::asio::mutable_buffer& buf,
                                            boost::function<void(const boost::system::error_code&, size_t)> handler,
                                            RR_SHARED_PTR<void> dev_h = RR_SHARED_PTR<void>());

    virtual void AsyncReadPipe(uint8_t ep, boost::asio::mutable_buffer& buf,
                               boost::function<void(const boost::system::error_code&, size_t)> handler);

    virtual void AsyncReadPipeNoLock(uint8_t ep, boost::asio::mutable_buffer& buf,
                                     boost::function<void(const boost::system::error_code&, size_t)> handler);

    virtual void AsyncWritePipe(uint8_t ep, boost::asio::mutable_buffer& buf,
                                boost::function<void(const boost::system::error_code&, size_t)> handler);

    virtual void AsyncWritePipeNoLock(uint8_t ep, boost::asio::mutable_buffer& buf,
                                      boost::function<void(const boost::system::error_code&, size_t)> handler);

    // Call with lock
    virtual UsbDeviceStatus ClaimDevice(RR_SHARED_PTR<void>& dev_h);

    // Call with lock
    virtual void ReleaseClaim();

    virtual void DrawDownRequests(boost::function<void()> handler);

    virtual void ClearHalt(uint8_t ep);

    RR_SHARED_PTR<WinUsb_Functions> f;

    RR_SHARED_PTR<WinUsbDevice_Handle> device_handle;
};

class WinUsbDevice : public UsbDevice
{
  protected:
    RR_SHARED_PTR<boost::asio::windows::stream_handle> device_handle;
    RR_SHARED_PTR<void> hInterface;

    RR_SHARED_PTR<WinUsb_Functions> f;

  public:
    WinUsbDevice(RR_SHARED_PTR<WinUsbDeviceManager> parent, RR_SHARED_PTR<WinUsb_Functions> f,
                 const UsbDeviceManager_detected_device& device);
    virtual ~WinUsbDevice();

    virtual RR_SHARED_PTR<UsbDevice_Initialize> CreateInitialize();
    virtual RR_SHARED_PTR<UsbDevice_Claim> CreateClaim();
};

class WinsockBluetoothConnector : public BluetoothConnector<SOCKADDR_BTH, AF_BTH, BTHPROTO_RFCOMM>
{
  public:
    WinsockBluetoothConnector(RR_SHARED_PTR<HardwareTransport> parent);

    virtual std::list<SOCKADDR_BTH> GetDeviceAddresses();

    virtual std::list<device_info> GetDeviceNodes(SOCKADDR_BTH addr);
};

class HardwareTransport_win_discovery
    : public HardwareTransport_discovery<WinUsbDeviceManager, WinsockBluetoothConnector>
{
  public:
    HardwareTransport_win_discovery(RR_SHARED_PTR<HardwareTransport> parent, const std::vector<std::string>& schemes,
                                    RR_SHARED_PTR<WinUsbDeviceManager> usb, RR_SHARED_PTR<WinsockBluetoothConnector> bt,
                                    RR_SHARED_PTR<void> f_void);
    virtual ~HardwareTransport_win_discovery() {}

    virtual void Init();

    virtual void Close();

    virtual std::vector<NodeDiscoveryInfo> GetDriverDevices();

  protected:
    std::vector<NodeDiscoveryInfo> GetDriverDevices1(const GUID* guid, const std::string& scheme);

    HWND m_Wnd;
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND CreateMessageWindow();
    void MessageWindowFunc();

    void OnDeviceChanged_win(GUID guid, DWORD evt);
    void OnBluetoothChanged_win(GUID guid, DWORD evt, PBYTE buf);

    bool running;
    boost::mutex this_lock;

    std::list<boost::shared_ptr<void> > hDeviceNotify;
    std::list<boost::shared_ptr<void> > hBtNotify;

    RR_SHARED_PTR<void> f;

    boost::thread window_thread;
};

boost::optional<std::wstring> HardwareTransport_win_find_deviceinterface(RR_SHARED_PTR<void> f,
                                                                         const GUID* interface_guid,
                                                                         const NodeID& nodeid,
                                                                         boost::string_ref nodename);

boost::optional<std::wstring> HardwareTransport_win_find_usb(RR_SHARED_PTR<void> f, const NodeID& nodeid,
                                                             boost::string_ref nodename);

boost::optional<std::wstring> HardwareTransport_win_find_pci(RR_SHARED_PTR<void> f, const NodeID& nodeid,
                                                             boost::string_ref nodename);

boost::optional<std::wstring> HardwareTransport_win_find_bluetooth(RR_SHARED_PTR<void> f, const NodeID& nodeid,
                                                                   boost::string_ref nodename);

std::list<boost::tuple<NodeID, std::string> > HardwareTransport_win_find_deviceinterfaces(RR_SHARED_PTR<void> f,
                                                                                          const GUID* interface_guid);

} // namespace detail
} // namespace RobotRaconteur
