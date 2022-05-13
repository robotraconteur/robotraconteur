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

#include "HardwareTransport_private.h"

#pragma once

namespace RobotRaconteur
{
namespace detail
{

// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)
static const uint8_t RR_USB_CS_INTERFACE_UUID_DETECT[] = {0x3F, 0x81, 0x0F, 0xD2, 0x2B, 0xCE, 0x45, 0x52,
                                                          0x98, 0xF3, 0xA8, 0xAC, 0x22, 0x0A, 0xD4, 0x8D};

static const size_t RR_USB_MAX_PACKET_SIZE = (16384);
// NOLINTEND(cppcoreguidelines-avoid-c-arrays)

static const uint8_t USB_DIR_OUT = 0;
static const uint8_t USB_DIR_IN = 0x80;

static const uint8_t USB_TYPE_VENDOR = (0x02 << 5);
static const uint8_t USB_RECIP_INTERFACE = 0x01;

enum rr_usb_subpacket_notify
{
    RR_USB_NOTIFICATION_NULL = 0,
    RR_USB_NOTIFICATION_SOCKET_CLOSED = 1,
    RR_USB_NOTIFICATION_PAUSE_REQUEST = 2,
    RR_USB_NOTIFICATION_RESUME_REQUEST = 3
};

static const uint16_t RR_USB_SUBPACKET_FLAG_COMMAND = 0x01;
static const uint16_t RR_USB_SUBPACKET_FLAG_NOTIFICATION = 0x02;

static const uint8_t VendorInterfaceRequest = ((USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE));
static const uint8_t VendorInterfaceOutRequest = ((USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_INTERFACE));

#define USB_ENDPOINT_DIRECTION_MASK 0x80
#define USB_ENDPOINT_DIRECTION_OUT(addr) (!((addr)&USB_ENDPOINT_DIRECTION_MASK))
#define USB_ENDPOINT_DIRECTION_IN(addr) ((addr)&USB_ENDPOINT_DIRECTION_MASK)
#define USB_ENDPOINT_TYPE_BULK(attr) (((attr)&0x03) == LIBUSB_TRANSFER_TYPE_BULK)

enum rr_usb_control_requests
{
    RR_USB_CONTROL_NULL = 0,
    RR_USB_CONTROL_CONNECT_STREAM,
    RR_USB_CONTROL_CLOSE_STREAM,
    RR_USB_CONTROL_RESET_ALL_STREAM,
    RR_USB_CONTROL_PAUSE_ALL_STREAM,
    RR_USB_CONTROL_RESUME_ALL_STREAM,
    RR_USB_CONTROL_CURRENT_PROTOCOL
};

#define RR_USB_CS_INTERFACE_DESCRIPTOR_TYPE 0x24

#ifdef ROBOTRACONTEUR_WINDOWS
#pragma pack(1)
#define RR_ATTR_PACKED
#else
#define RR_ATTR_PACKED __attribute__((packed))
#endif
struct subpacket_header
{
    uint16_t len;
    uint16_t flags;
    int32_t id;

} RR_ATTR_PACKED;

struct robotraconteur_usb_common_descriptor
{
    uint8_t bLength;
    uint8_t bDescriptorType;
} RR_ATTR_PACKED;

struct robotraconteur_interface_common_descriptor
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;
} RR_ATTR_PACKED;

struct robotraconteur_interface_descriptor
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;
    uint8_t uuidRobotRaconteurDetect[16]; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    int16_t wVersion;
    uint8_t iNodeID;
    uint8_t iNodeName;
    uint16_t wNumProtocols;
} RR_ATTR_PACKED;

struct robotraconteur_protocol_descriptor
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;
    uint16_t wRRProtocol;
} RR_ATTR_PACKED;

#ifdef ROBOTRACONTEUR_WINDOWS
#pragma pack()
#endif

enum UsbDeviceStatus
{
    NotInitialized,
    Initializing,
    Busy,
    Ready,
    Open,
    Claiming,
    Claimed,
    Error,
    Cleanup,
    Closing,
    Closed,
    Invalid,
    Unauthorized,
    Shutdown
};

// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
class UsbDeviceManager_detected_device
{
  public:
    std::wstring path;
    RR_SHARED_PTR<void> handle;
    uint8_t interface_;
};
// NOLINTEND(cppcoreguidelines-pro-type-member-init)

class UsbDevice;
class UsbDeviceTransportConnection;

class UsbDeviceManager : public RR_ENABLE_SHARED_FROM_THIS<UsbDeviceManager>
{
  public:
    UsbDeviceManager(const RR_SHARED_PTR<HardwareTransport>& parent);
    virtual ~UsbDeviceManager() {}

    RR_SHARED_PTR<HardwareTransport> GetParent();

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

    virtual void UpdateDevices(boost::function<void()> handler);

    virtual void AsyncCreateTransportConnection(
        const ParseConnectionURLResult& url_res, uint32_t endpoint, boost::string_ref noden,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);

    virtual void Shutdown();

    virtual std::list<boost::tuple<NodeID, std::string> > GetDetectedDevices();

  protected:
    virtual RR_SHARED_PTR<UsbDevice> CreateDevice(const UsbDeviceManager_detected_device& device) = 0;

    virtual std::list<UsbDeviceManager_detected_device> GetDetectedDevicesPaths() = 0;

    // Call with lock
    virtual bool InitUpdateDevices() = 0;

    virtual void UpdateDevices1(boost::function<void()> handler);
    virtual void UpdateDevices2(UsbDeviceStatus status, const RR_SHARED_PTR<UsbDevice>& dev,
                                const RR_SHARED_PTR<std::list<RR_SHARED_PTR<UsbDevice> > >& l,
                                boost::function<void()> handler);

    virtual void AsyncCreateTransportConnection1(
        const ParseConnectionURLResult& url_res, uint32_t endpoint, const std::string& noden,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);

    RR_WEAK_PTR<HardwareTransport> parent;
    RR_WEAK_PTR<RobotRaconteurNode> node;

    boost::mutex this_lock;

    std::map<std::wstring, RR_SHARED_PTR<UsbDevice> > devices;

    bool is_shutdown;
};

class UsbDeviceClaim_create_request
{
  public:
    ParseConnectionURLResult url_res;
    uint32_t endpoint;
    std::string noden;
    boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
        handler;

    UsbDeviceClaim_create_request(const ParseConnectionURLResult& url_res, uint32_t endpoint, boost::string_ref noden,
                                  boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                                                       const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);
};

class UsbDevice_Claim;

class UsbDevice_Claim_Lock : public RR_ENABLE_SHARED_FROM_THIS<UsbDevice_Claim_Lock>
{
    RR_WEAK_PTR<UsbDevice_Claim> parent;

  public:
    UsbDevice_Claim_Lock(const RR_SHARED_PTR<UsbDevice_Claim>& parent);
    virtual ~UsbDevice_Claim_Lock(); // NOLINT(bugprone-exception-escape)
};

class UsbDevice_Settings
{
  public:
    RR_SHARED_PTR<void> path;
    boost::optional<NodeID> nodeid;
    boost::optional<std::string> nodename;

    uint8_t in_pipe_id;
    uint16_t in_pipe_maxpacket;
    uint16_t in_pipe_buffer_size;
    uint8_t out_pipe_id;
    uint16_t out_pipe_maxpacket;
    uint16_t out_pipe_buffer_size;
    uint8_t interface_number;
    uint8_t interface_alt_setting;
    uint8_t interface_num_endpoints;
    int device_desired_config;
    uint8_t string_lang_index;
    uint8_t string_nodeid_index;
    uint8_t string_nodename_index;
    std::vector<uint16_t> supported_protocols;

    UsbDevice_Settings();
};

class UsbDevice_Initialize : public RR_ENABLE_SHARED_FROM_THIS<UsbDevice_Initialize>
{
  public:
    friend class UsbDeviceTransportConnection;

    UsbDevice_Initialize(const RR_SHARED_PTR<UsbDevice>& parent,
                         const UsbDeviceManager_detected_device& detected_device);
    virtual ~UsbDevice_Initialize() {}

    RR_SHARED_PTR<UsbDevice> GetParent();

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

    virtual void InitializeDevice(boost::function<void(const UsbDeviceStatus&)> handler);

  protected:
    void InitializeDevice1(
        uint32_t attempt, boost::function<void(const UsbDeviceStatus&)> handler,
        const RR_SHARED_PTR<boost::asio::deadline_timer>& timer = RR_SHARED_PTR<boost::asio::deadline_timer>());

    void InitializeDevice2(const boost::system::error_code& ec, const std::string& device_nodeid,
                           boost::function<void(const UsbDeviceStatus&)> handler, const RR_SHARED_PTR<void>& dev_h,
                           const RR_SHARED_PTR<UsbDevice_Settings>& settings);

    void InitializeDevice3(const boost::system::error_code& ec, const std::string& device_nodename,
                           boost::function<void(const UsbDeviceStatus&)> handler, const RR_SHARED_PTR<void>& dev_h,
                           const RR_SHARED_PTR<UsbDevice_Settings>& settings);

    void InitializeDevice_err(const boost::function<void(const UsbDeviceStatus&)>& handler,
                              UsbDeviceStatus status = Error);

    virtual void AsyncControlTransfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                      boost::asio::mutable_buffer& buf,
                                      boost::function<void(const boost::system::error_code&, size_t)> handler,
                                      const RR_SHARED_PTR<void>& dev_h = RR_SHARED_PTR<void>()) = 0;

    // Call with lock
    virtual void AsyncControlTransferNoLock(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                            boost::asio::mutable_buffer& buf,
                                            boost::function<void(const boost::system::error_code&, size_t)> handler,
                                            const RR_SHARED_PTR<void>& dev_h = RR_SHARED_PTR<void>()) = 0;

    // Call with lock
    virtual UsbDeviceStatus OpenDevice(RR_SHARED_PTR<void>& dev_h) = 0;

    // Call with lock
    virtual UsbDeviceStatus ReadPipeSettings(const RR_SHARED_PTR<void>& dev_h,
                                             RR_SHARED_PTR<UsbDevice_Settings>& settings) = 0;

    // Call with lock
    virtual UsbDeviceStatus ReadInterfaceSettings(const RR_SHARED_PTR<void>& dev_h,
                                                  RR_SHARED_PTR<UsbDevice_Settings>& settings) = 0;

    void ReadRRDeviceString(uint8_t interface_number, uint8_t property_index,
                            boost::function<void(const boost::system::error_code&, const std::string&)> handler,
                            const RR_SHARED_PTR<void>& dev_h);

    void ReadRRDeviceString1(const boost::system::error_code& ec, size_t bytes_transferred, uint8_t interface_number,
                             uint8_t property_index, const boost::shared_array<uint8_t>& buf,
                             boost::function<void(const boost::system::error_code&, const std::string&)> handler,
                             const RR_SHARED_PTR<void>& dev_h);

    void ReadRRDeviceString2(const boost::system::error_code& ec, size_t bytes_transferred, uint8_t interface_number,
                             uint8_t property_index, const boost::shared_array<uint8_t>& buf,
                             boost::function<void(const boost::system::error_code&, const std::string&)> handler,
                             const RR_SHARED_PTR<void>& dev_h);

    boost::mutex this_lock;

    std::list<boost::function<void(const UsbDeviceStatus&)> > initialize_handlers;

    RR_WEAK_PTR<RobotRaconteurNode> node;
    RR_WEAK_PTR<UsbDevice> parent;

    UsbDeviceStatus status;

    UsbDeviceManager_detected_device detected_device;
};

class UsbDevice_Claim : public RR_ENABLE_SHARED_FROM_THIS<UsbDevice_Claim>
{
  public:
    friend class UsbDeviceTransportConnection;
    friend class UsbDevice_Claim_Lock;

    UsbDevice_Claim(const RR_SHARED_PTR<UsbDevice>& parent, const UsbDeviceManager_detected_device& detected_device);
    virtual ~UsbDevice_Claim() {}

    RR_SHARED_PTR<UsbDevice> GetParent();

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

    void AsyncCreateTransportConnection(
        const ParseConnectionURLResult& url_res, uint32_t endpoint, boost::string_ref noden,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);

    virtual void Close();

    virtual RR_SHARED_PTR<void> RequestClaimLock();

    UsbDeviceStatus GetStatus();

  protected:
    void AsyncCreateTransportConnection1(
        const ParseConnectionURLResult& url_res, uint32_t endpoint, const std::string& noden,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler,
        uint32_t attempt);

    void AsyncCreateTransportConnection2(
        const boost::system::error_code& ec, size_t bytes_transferred, const boost::shared_array<uint8_t>& buf,
        const ParseConnectionURLResult& url_res, uint32_t endpoint, const std::string& noden,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);

    void AsyncCreateTransportConnection3(
        const boost::system::error_code& ec, size_t bytes_transferred, const boost::shared_array<uint8_t>& buf,
        const ParseConnectionURLResult& url_res, uint32_t endpoint, const std::string& noden,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);

    void AsyncCreateTransportConnection4(
        const boost::system::error_code& ec, size_t bytes_transferred, const boost::shared_array<uint8_t>& buf,
        const ParseConnectionURLResult& url_res, uint32_t endpoint, const std::string& noden,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);

    void AsyncCreateTransportConnection5(
        const ParseConnectionURLResult& url_res, uint32_t endpoint, const std::string& noden,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);

    void AsyncCreateTransportConnection6(
        const boost::system::error_code& ec, size_t bytes_transferred, const boost::shared_array<uint8_t>& buf,
        const ParseConnectionURLResult& url_res, uint32_t endpoint, const std::string& noden,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);

    void AsyncCreateTransportConnection_err(
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
        UsbDeviceStatus status = Error);

    virtual void AsyncControlTransfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                      boost::asio::mutable_buffer& buf,
                                      boost::function<void(const boost::system::error_code&, size_t)> handler,
                                      const RR_SHARED_PTR<void>& dev_h = RR_SHARED_PTR<void>()) = 0;

    virtual void AsyncControlTransferNoLock(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                            boost::asio::mutable_buffer& buf,
                                            boost::function<void(const boost::system::error_code&, size_t)> handler,
                                            const RR_SHARED_PTR<void>& dev_h = RR_SHARED_PTR<void>()) = 0;

    virtual void AsyncReadPipe(uint8_t ep, boost::asio::mutable_buffer& buf,
                               boost::function<void(const boost::system::error_code&, size_t)> handler) = 0;

    virtual void AsyncReadPipeNoLock(uint8_t ep, boost::asio::mutable_buffer& buf,
                                     boost::function<void(const boost::system::error_code&, size_t)> handler) = 0;

    virtual void AsyncWritePipe(uint8_t ep, boost::asio::mutable_buffer& buf,
                                boost::function<void(const boost::system::error_code&, size_t)> handler) = 0;

    virtual void AsyncWritePipeNoLock(uint8_t ep, boost::asio::mutable_buffer& buf,
                                      boost::function<void(const boost::system::error_code&, size_t)> handler) = 0;

    void CleanupConnections();

    void ConnectionClosed(const RR_SHARED_PTR<UsbDeviceTransportConnection>& connection);

    void ConnectionClosed1(const boost::system::error_code& ec, size_t bytes_transferred,
                           const boost::shared_array<uint8_t>& buf);

    void TransportCanRead();

    void TransportCanWrite();

    void DoRead();

    void DoWrite();

    void EndRead(const boost::system::error_code& ec, size_t bytes_transferred, const boost::shared_array<uint8_t>& buf,
                 uint64_t c);

    void EndRead2(size_t bytes_transferred, const boost::shared_array<uint8_t>& buf, uint64_t c);

    void EndWrite(const boost::system::error_code& ec, size_t bytes_transferred,
                  const boost::shared_array<uint8_t>& buf);

    void DeviceError(const boost::system::error_code& ec);

    // Call with lock
    virtual UsbDeviceStatus ClaimDevice(RR_SHARED_PTR<void>& dev_h) = 0;

    // Call with lock
    virtual void ReleaseClaim() = 0;

    virtual void DrawDownRequests(boost::function<void()> handler) = 0;

    virtual void Closed1(const boost::system::error_code& ec, size_t bytes_transferred,
                         const boost::shared_array<uint8_t>& buf);

    virtual void Closed2();

    virtual void ClaimLockReleased(UsbDevice_Claim_Lock* claim_lock);

    virtual void ClearHalt(uint8_t ep) = 0;

    UsbDeviceManager_detected_device detected_device;

    boost::mutex this_lock;

    RR_SHARED_PTR<UsbDevice_Settings> settings;

    std::map<int32_t, RR_SHARED_PTR<UsbDeviceTransportConnection> > transport_connections;
    size_t stream_connection_in_progress;

    std::list<UsbDeviceClaim_create_request> create_requests;

    RR_WEAK_PTR<RobotRaconteurNode> node;
    RR_WEAK_PTR<UsbDevice> parent;

    UsbDeviceStatus status;
    std::list<UsbDevice_Claim_Lock*> claim_locks;
    // RR_SHARED_PTR<void> device_handle;

    uint64_t read_count;
    uint64_t read_last_complete;
    std::list<boost::tuple<size_t, boost::shared_array<uint8_t>, uint64_t> > read_out_of_order;
    std::list<boost::shared_array<uint8_t> > read_buf;
    std::list<boost::shared_array<uint8_t> > read_buf_use;

    std::list<boost::shared_array<uint8_t> > write_buf;
    std::list<boost::shared_array<uint8_t> > write_buf_use;

    std::list<RR_WEAK_PTR<UsbDeviceTransportConnection> > transport_write_idle;
    std::list<RR_WEAK_PTR<UsbDeviceTransportConnection> > transport_write;

    void transport_write_remove(std::list<RR_WEAK_PTR<UsbDeviceTransportConnection> >& l, int32_t id);
};

class UsbDevice : public RR_ENABLE_SHARED_FROM_THIS<UsbDevice>
{
  public:
  protected:
    RR_WEAK_PTR<UsbDeviceManager> parent;
    RR_WEAK_PTR<RobotRaconteurNode> node;

    UsbDeviceStatus status;
    boost::mutex this_lock;

    RR_SHARED_PTR<UsbDevice_Settings> settings;
    UsbDeviceManager_detected_device detected_device;

    RR_WEAK_PTR<UsbDevice_Claim> claim;
    RR_WEAK_PTR<UsbDevice_Initialize> init;

  public:
    friend class UsbDeviceTransportConnection;
    friend class UsbDeviceManager;
    friend class UsbDevice_Initialize;
    friend class UsbDevice_Claim;

    UsbDevice(const RR_SHARED_PTR<UsbDeviceManager>& parent, const UsbDeviceManager_detected_device& detected_device);

    virtual ~UsbDevice();

    RR_SHARED_PTR<UsbDeviceManager> GetParent();
    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

    UsbDeviceStatus GetDeviceStatus();

    void InitializeDevice(boost::function<void(const UsbDeviceStatus&)> handler);

    boost::tuple<NodeID, std::string> GetNodeInfo();

    void AsyncCreateTransportConnection(
        const ParseConnectionURLResult& url_res, uint32_t endpoint, boost::string_ref noden,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);

    void Close();

    void Shutdown();

  protected:
    virtual RR_SHARED_PTR<UsbDevice_Initialize> CreateInitialize() = 0;
    virtual RR_SHARED_PTR<UsbDevice_Claim> CreateClaim() = 0;

    virtual void DeviceInitialized(const RR_SHARED_PTR<UsbDevice_Settings>& settings, UsbDeviceStatus status = Ready);

    virtual void DeviceClaimed(const RR_SHARED_PTR<UsbDevice_Claim>& claim, UsbDeviceStatus status = Claimed);

    virtual void DeviceClaimError(const RR_SHARED_PTR<UsbDevice_Claim>& claim, UsbDeviceStatus status = Error);

    virtual void DeviceClaimReleased(const RR_SHARED_PTR<UsbDevice_Claim>& claim);
};

class UsbDeviceTransportConnection : public HardwareTransportConnection
{
  public:
    friend class UsbDevice_Claim;

    UsbDeviceTransportConnection(const RR_SHARED_PTR<HardwareTransport>& parent, uint32_t local_endpoint,
                                 const RR_SHARED_PTR<UsbDevice_Claim>& device, int32_t stream_id);
    RR_OVIRTUAL ~UsbDeviceTransportConnection() RR_OVERRIDE {}

    void AsyncAttachSocket(boost::string_ref noden,
                           const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

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

    RR_OVIRTUAL void Close1() RR_OVERRIDE {}

  public:
    RR_OVIRTUAL void Close() RR_OVERRIDE;

  protected:
    RR_WEAK_PTR<UsbDevice_Claim> device;
    int32_t stream_id;

    size_t CanDoRead();
    size_t CanDoWrite();

    void DoRead(boost::asio::const_buffer& buf);
    size_t DoWrite(boost::asio::mutable_buffer& buf);

    std::list<boost::tuple<mutable_buffers, boost::function<void(const boost::system::error_code&, size_t)> > >
        read_ops;
    std::list<boost::tuple<const_buffers, boost::function<void(const boost::system::error_code&, size_t)> > > write_ops;
    std::list<boost::tuple<boost::asio::mutable_buffer, boost::shared_array<uint8_t> > > read_bufs;

    boost::mutex do_read_lock;
    boost::mutex do_write_lock;
    bool remote_closed;
    bool send_paused;
};

} // namespace detail

} // namespace RobotRaconteur
