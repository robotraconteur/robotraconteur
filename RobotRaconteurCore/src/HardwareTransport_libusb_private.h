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

#include "HardwareTransport_usbcommon_private.h"
#include <boost/intrusive/list.hpp>
#include <boost/intrusive_ptr.hpp>

#include <libusb.h>

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    // LibUsb Function Types

    typedef int (*libusb_init_t)(libusb_context** ctx);
    typedef void (*libusb_exit_t)(libusb_context* ctx);
    typedef void (*libusb_set_debug_t)(libusb_context* ctx, int level);
    typedef const struct libusb_version* (*libusb_get_version_t)(void);
    typedef int (*libusb_has_capability_t)(uint32_t capability);
    typedef const char* (*libusb_error_name_t)(int errcode);
    typedef int (*libusb_setlocale_t)(const char* locale);
    typedef const char* (*libusb_strerror_t)(enum libusb_error errcode);
    typedef ssize_t (*libusb_get_device_list_t)(libusb_context* ctx, libusb_device*** list);
    typedef void (*libusb_free_device_list_t)(libusb_device** list, int unref_devices);
    typedef libusb_device* (*libusb_ref_device_t)(libusb_device* dev);
    typedef void (*libusb_unref_device_t)(libusb_device* dev);
    typedef int (*libusb_get_configuration_t)(libusb_device_handle* dev, int* config);
    typedef int (*libusb_get_device_descriptor_t)(libusb_device* dev, struct libusb_device_descriptor* desc);
    typedef int (*libusb_get_active_config_descriptor_t)(libusb_device* dev, struct libusb_config_descriptor** config);
    typedef int (*libusb_get_config_descriptor_t)(libusb_device* dev, uint8_t config_index,
                                                  struct libusb_config_descriptor** config);
    typedef int (*libusb_get_config_descriptor_by_value_t)(libusb_device* dev, uint8_t bConfigurationValue,
                                                           struct libusb_config_descriptor** config);
    typedef void (*libusb_free_config_descriptor_t)(struct libusb_config_descriptor* config);
    typedef int (*libusb_get_ss_endpoint_companion_descriptor_t)(
        struct libusb_context* ctx, const struct libusb_endpoint_descriptor* endpoint,
        struct libusb_ss_endpoint_companion_descriptor** ep_comp);
    typedef void (*libusb_free_ss_endpoint_companion_descriptor_t)(
        struct libusb_ss_endpoint_companion_descriptor* ep_comp);
    typedef int (*libusb_get_bos_descriptor_t)(libusb_device_handle* handle, struct libusb_bos_descriptor** bos);
    typedef void (*libusb_free_bos_descriptor_t)(struct libusb_bos_descriptor* bos);
    typedef int (*libusb_get_usb_2_0_extension_descriptor_t)(
        struct libusb_context* ctx, struct libusb_bos_dev_capability_descriptor* dev_cap,
        struct libusb_usb_2_0_extension_descriptor** usb_2_0_extension);
    typedef void (*libusb_free_usb_2_0_extension_descriptor_t)(
        struct libusb_usb_2_0_extension_descriptor* usb_2_0_extension);
    typedef int (*libusb_get_ss_usb_device_capability_descriptor_t)(
        struct libusb_context* ctx, struct libusb_bos_dev_capability_descriptor* dev_cap,
        struct libusb_ss_usb_device_capability_descriptor** ss_usb_device_cap);
    typedef void (*libusb_free_ss_usb_device_capability_descriptor_t)(
        struct libusb_ss_usb_device_capability_descriptor* ss_usb_device_cap);
    typedef int (*libusb_get_container_id_descriptor_t)(struct libusb_context* ctx,
                                                        struct libusb_bos_dev_capability_descriptor* dev_cap,
                                                        struct libusb_container_id_descriptor** container_id);
    typedef void (*libusb_free_container_id_descriptor_t)(struct libusb_container_id_descriptor* container_id);
    typedef uint8_t (*libusb_get_bus_number_t)(libusb_device* dev);
    typedef uint8_t (*libusb_get_port_number_t)(libusb_device* dev);
    typedef int (*libusb_get_port_numbers_t)(libusb_device* dev, uint8_t* port_numbers, int port_numbers_len);
    typedef libusb_device* (*libusb_get_parent_t)(libusb_device* dev);
    typedef uint8_t (*libusb_get_device_address_t)(libusb_device* dev);
    typedef int (*libusb_get_device_speed_t)(libusb_device* dev);
    typedef int (*libusb_get_max_packet_size_t)(libusb_device* dev, unsigned char endpoint);
    typedef int (*libusb_get_max_iso_packet_size_t)(libusb_device* dev, unsigned char endpoint);

    typedef int (*libusb_open_t)(libusb_device* dev, libusb_device_handle** handle);
    typedef void (*libusb_close_t)(libusb_device_handle* dev_handle);
    typedef libusb_device* (*libusb_get_device_t)(libusb_device_handle* dev_handle);

    typedef int (*libusb_set_configuration_t)(libusb_device_handle* dev, int configuration);
    typedef int (*libusb_claim_interface_t)(libusb_device_handle* dev, int interface_number);
    typedef int (*libusb_release_interface_t)(libusb_device_handle* dev, int interface_number);

    typedef libusb_device_handle* (*libusb_open_device_with_vid_pid_t)(libusb_context* ctx, uint16_t vendor_id,
                                                                       uint16_t product_id);

    typedef int (*libusb_set_interface_alt_setting_t)(libusb_device_handle* dev, int interface_number,
                                                      int alternate_setting);
    typedef int (*libusb_clear_halt_t)(libusb_device_handle* dev, unsigned char endpoint);
    typedef int (*libusb_reset_device_t)(libusb_device_handle* dev);

    typedef int (*libusb_alloc_streams_t)(libusb_device_handle* dev, uint32_t num_streams, unsigned char* endpoints,
                                          int num_endpoints);
    typedef int (*libusb_free_streams_t)(libusb_device_handle* dev, unsigned char* endpoints, int num_endpoints);
    typedef int (*libusb_kernel_driver_active_t)(libusb_device_handle* dev, int interface_number);
    typedef int (*libusb_detach_kernel_driver_t)(libusb_device_handle* dev, int interface_number);
    typedef int (*libusb_attach_kernel_driver_t)(libusb_device_handle* dev, int interface_number);
    typedef int (*libusb_set_auto_detach_kernel_driver_t)(libusb_device_handle* dev, int enable);

    // LibUsb Async Functions

    typedef struct libusb_transfer* (*libusb_alloc_transfer_t)(int iso_packets);
    typedef int (*libusb_submit_transfer_t)(struct libusb_transfer* transfer);
    typedef int (*libusb_cancel_transfer_t)(struct libusb_transfer* transfer);
    typedef void (*libusb_free_transfer_t)(struct libusb_transfer* transfer);
    typedef void (*libusb_transfer_set_stream_id_t)(struct libusb_transfer* transfer, uint32_t stream_id);
    typedef uint32_t (*libusb_transfer_get_stream_id_t)(struct libusb_transfer* transfer);

    // LibUsb polling and timeouts

    typedef int (*libusb_try_lock_events_t)(libusb_context* ctx);
    typedef void (*libusb_lock_events_t)(libusb_context* ctx);
    typedef void (*libusb_unlock_events_t)(libusb_context* ctx);
    typedef int (*libusb_event_handling_ok_t)(libusb_context* ctx);
    typedef int (*libusb_event_handler_active_t)(libusb_context* ctx);
    typedef void (*libusb_lock_event_waiters_t)(libusb_context* ctx);
    typedef void (*libusb_unlock_event_waiters_t)(libusb_context* ctx);
    typedef int (*libusb_wait_for_event_t)(libusb_context* ctx, struct timeval* tv);

    typedef int (*libusb_handle_events_timeout_t)(libusb_context* ctx, struct timeval* tv);
    typedef int (*libusb_handle_events_timeout_completed_t)(libusb_context* ctx, struct timeval* tv, int* completed);
    typedef int (*libusb_handle_events_t)(libusb_context* ctx);
    typedef int (*libusb_handle_events_completed_t)(libusb_context* ctx, int* completed);
    typedef int (*libusb_handle_events_locked_t)(libusb_context* ctx, struct timeval* tv);
    typedef int (*libusb_pollfds_handle_timeouts_t)(libusb_context* ctx);
    typedef int (*libusb_get_next_timeout_t)(libusb_context* ctx, struct timeval* tv);

    typedef const struct libusb_pollfd** (*libusb_get_pollfds_t)(libusb_context* ctx);
    typedef void (*libusb_free_pollfds_t)(const struct libusb_pollfd** pollfds);
    typedef void (*libusb_set_pollfd_notifiers_t)(libusb_context* ctx, libusb_pollfd_added_cb added_cb,
                                                  libusb_pollfd_removed_cb removed_cb, void* user_data);

    // LibUsb hotplug

    typedef int (*libusb_hotplug_register_callback_t)(libusb_context* ctx, libusb_hotplug_event events,
                                                      libusb_hotplug_flag flags, int vendor_id, int product_id,
                                                      int dev_class, libusb_hotplug_callback_fn cb_fn, void* user_data,
                                                      libusb_hotplug_callback_handle* handle);

    typedef void (*libusb_hotplug_deregister_callback_t)(libusb_context* ctx, libusb_hotplug_callback_handle handle);

#ifdef __cplusplus
}
#endif

namespace RobotRaconteur
{
namespace detail
{

#define LIBUSB_FUNCTIONS_INIT(M)                                                                                       \
    M(libusb_init)                                                                                                     \
    M(libusb_exit)                                                                                                     \
    M(libusb_set_debug)                                                                                                \
    M(libusb_get_version)                                                                                              \
    M(libusb_has_capability)                                                                                           \
    M(libusb_error_name)                                                                                               \
    M(libusb_setlocale)                                                                                                \
    M(libusb_strerror)                                                                                                 \
    M(libusb_get_device_list)                                                                                          \
    M(libusb_free_device_list)                                                                                         \
    M(libusb_ref_device)                                                                                               \
    M(libusb_unref_device)                                                                                             \
    M(libusb_get_configuration)                                                                                        \
    M(libusb_get_device_descriptor)                                                                                    \
    M(libusb_get_active_config_descriptor)                                                                             \
    M(libusb_get_config_descriptor)                                                                                    \
    M(libusb_get_config_descriptor_by_value)                                                                           \
    M(libusb_free_config_descriptor)                                                                                   \
    M(libusb_get_ss_endpoint_companion_descriptor)                                                                     \
    M(libusb_free_ss_endpoint_companion_descriptor)                                                                    \
    M(libusb_get_bos_descriptor)                                                                                       \
    M(libusb_free_bos_descriptor)                                                                                      \
    M(libusb_get_usb_2_0_extension_descriptor)                                                                         \
    M(libusb_free_usb_2_0_extension_descriptor)                                                                        \
    M(libusb_get_ss_usb_device_capability_descriptor)                                                                  \
    M(libusb_free_ss_usb_device_capability_descriptor)                                                                 \
    M(libusb_get_container_id_descriptor)                                                                              \
    M(libusb_free_container_id_descriptor)                                                                             \
    M(libusb_get_bus_number)                                                                                           \
    M(libusb_get_port_number)                                                                                          \
    M(libusb_get_port_numbers)                                                                                         \
    M(libusb_get_parent)                                                                                               \
    M(libusb_get_device_address)                                                                                       \
    M(libusb_get_device_speed)                                                                                         \
    M(libusb_get_max_packet_size)                                                                                      \
    M(libusb_get_max_iso_packet_size)                                                                                  \
    M(libusb_open)                                                                                                     \
    M(libusb_close)                                                                                                    \
    M(libusb_get_device)                                                                                               \
    M(libusb_set_configuration)                                                                                        \
    M(libusb_claim_interface)                                                                                          \
    M(libusb_release_interface)                                                                                        \
    M(libusb_open_device_with_vid_pid)                                                                                 \
    M(libusb_set_interface_alt_setting)                                                                                \
    M(libusb_clear_halt)                                                                                               \
    M(libusb_reset_device)                                                                                             \
    /*M(libusb_alloc_streams)*/                                                                                        \
    /*M(libusb_free_streams) */                                                                                        \
    M(libusb_kernel_driver_active)                                                                                     \
    M(libusb_detach_kernel_driver)                                                                                     \
    M(libusb_attach_kernel_driver)                                                                                     \
    M(libusb_set_auto_detach_kernel_driver)                                                                            \
    M(libusb_alloc_transfer)                                                                                           \
    M(libusb_submit_transfer)                                                                                          \
    M(libusb_cancel_transfer)                                                                                          \
    M(libusb_free_transfer)                                                                                            \
    /*M(libusb_transfer_set_stream_id)*/                                                                               \
    /*M(libusb_transfer_get_stream_id)*/                                                                               \
    M(libusb_try_lock_events)                                                                                          \
    M(libusb_lock_events)                                                                                              \
    M(libusb_unlock_events)                                                                                            \
    M(libusb_event_handling_ok)                                                                                        \
    M(libusb_event_handler_active)                                                                                     \
    M(libusb_lock_event_waiters)                                                                                       \
    M(libusb_unlock_event_waiters)                                                                                     \
    M(libusb_wait_for_event)                                                                                           \
    M(libusb_handle_events_timeout)                                                                                    \
    M(libusb_handle_events_timeout_completed)                                                                          \
    M(libusb_handle_events)                                                                                            \
    M(libusb_handle_events_completed)                                                                                  \
    M(libusb_handle_events_locked)                                                                                     \
    M(libusb_pollfds_handle_timeouts)                                                                                  \
    M(libusb_get_next_timeout)                                                                                         \
    /*M(libusb_get_pollfds)*/                                                                                          \
    /*M(libusb_free_pollfds)*/                                                                                         \
    /*M(libusb_set_pollfd_notifiers)*/                                                                                 \
    M(libusb_hotplug_register_callback)                                                                                \
    M(libusb_hotplug_deregister_callback)

#define LIBUSB_FUNCTIONS_FIELD_INIT(t) t##_t t;
#define LIBUSB_FUNCTIONS_PTR_VOID(t) t = NULL;
#define LIBUSB_FUNCTIONS_PTR_INIT(t)                                                                                   \
    t = reinterpret_cast<t##_t>(dlsym(lib_handle, #t));                                                                \
    if (t == NULL)                                                                                                     \
        return false;

class LibUsb_Functions : public boost::noncopyable
{
  private:
    void* lib_handle;

  public:
    LIBUSB_FUNCTIONS_INIT(LIBUSB_FUNCTIONS_FIELD_INIT);

    LibUsb_Functions();
    bool LoadFunctions();
    virtual ~LibUsb_Functions();
};

class LibUsbDeviceManager;

class LibUsb_Transfer
{
  public:
    RR_SHARED_PTR<LibUsb_Functions> f;
    RR_SHARED_PTR<libusb_device_handle> device_handle;
    RR_WEAK_PTR<LibUsbDeviceManager> device_manager;
    RR_WEAK_PTR<ThreadPool> thread_pool;

    libusb_transfer* transfer;

    boost::intrusive::list_member_hook<> manager_transfer_list_hook;
    boost::atomic<size_t> ref_count;

    typedef boost::intrusive::list<LibUsb_Transfer,
                                   boost::intrusive::member_hook<LibUsb_Transfer, boost::intrusive::list_member_hook<>,
                                                                 &LibUsb_Transfer::manager_transfer_list_hook> >
        manager_transfer_list_t;

    virtual void CompleteTransfer() = 0;

    LibUsb_Transfer(RR_SHARED_PTR<LibUsb_Functions> f, RR_SHARED_PTR<libusb_device_handle> device_handle,
                    RR_SHARED_PTR<LibUsbDeviceManager> device_manager);
    virtual ~LibUsb_Transfer();
};

void intrusive_ptr_release(LibUsb_Transfer* p);
void intrusive_ptr_add_ref(LibUsb_Transfer* p);

class LibUsb_Transfer_control : public LibUsb_Transfer
{
  public:
    uint8_t* temp_buf;
    boost::asio::mutable_buffer data_buf;
    boost::function<void(const boost::system::error_code&, size_t)> handler;

    void FillTransfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                      boost::asio::mutable_buffer& buf,
                      boost::function<void(const boost::system::error_code&, size_t)> handler);

    virtual void CompleteTransfer();

    LibUsb_Transfer_control(RR_SHARED_PTR<LibUsb_Functions> f, RR_SHARED_PTR<libusb_device_handle> device_handle,
                            RR_SHARED_PTR<LibUsbDeviceManager> device_manager);
    virtual ~LibUsb_Transfer_control();
};

class LibUsb_Transfer_bulk : public LibUsb_Transfer
{
  public:
    boost::asio::mutable_buffer data_buf;

    boost::function<void(const boost::system::error_code&, size_t)> handler;

    void FillTransfer(uint8_t ep, boost::asio::mutable_buffer& buf,
                      boost::function<void(const boost::system::error_code&, size_t)> handler);

    virtual void CompleteTransfer();

    LibUsb_Transfer_bulk(RR_SHARED_PTR<LibUsb_Functions> f, RR_SHARED_PTR<libusb_device_handle> device_handle,
                         RR_SHARED_PTR<LibUsbDeviceManager> device_manager);
    virtual ~LibUsb_Transfer_bulk();
};

class LibUsbDeviceManager : public UsbDeviceManager
{
  public:
    friend class LibUsbDevice;
    friend class LibUsbDevice_Initialize;
    friend class LibUsbDevice_Claim;
    friend class LibUsb_Transfer_control;
    friend class LibUsb_Transfer_bulk;

    LibUsbDeviceManager(RR_SHARED_PTR<HardwareTransport> parent);
    virtual ~LibUsbDeviceManager();

    virtual void Shutdown();

  protected:
    virtual RR_SHARED_PTR<UsbDevice> CreateDevice(const UsbDeviceManager_detected_device& device);

    virtual std::list<UsbDeviceManager_detected_device> GetDetectedDevicesPaths();

    // Call with lock
    virtual bool InitUpdateDevices();

    RR_SHARED_PTR<LibUsb_Functions> f;
    RR_SHARED_PTR<libusb_context> context;
    bool running;
    libusb_hotplug_callback_handle hotplug_cb_handle;
    boost::thread usb_thread;

    void UsbThread();

    static int OnUsbHotplugEvent(libusb_context* ctx, libusb_device* device, libusb_hotplug_event event,
                                 void* user_data);

  public:
    static void LibUsbCloseDevice(RR_WEAK_PTR<LibUsbDeviceManager> d, RR_SHARED_PTR<LibUsb_Functions> f,
                                  libusb_device_handle* h);

  protected:
    void submit_transfer(boost::intrusive_ptr<LibUsb_Transfer>& transfer);

    static void transfer_complete(libusb_transfer* transfer);

    boost::mutex manager_transfer_lock;
    LibUsb_Transfer::manager_transfer_list_t manager_transfer_list;

    std::map<libusb_device_handle*, boost::function<void()> > closing_device_handles;

    void DrawDownRequests(RR_SHARED_PTR<libusb_device_handle> h, boost::function<void()> handler);
};

class LibUsbDevice_Initialize : public UsbDevice_Initialize
{
  public:
    LibUsbDevice_Initialize(RR_SHARED_PTR<UsbDevice> parent, RR_SHARED_PTR<LibUsb_Functions> f,
                            const UsbDeviceManager_detected_device& detected_device);
    virtual ~LibUsbDevice_Initialize() {}

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
    RR_SHARED_PTR<LibUsb_Functions> f;
    RR_WEAK_PTR<LibUsbDeviceManager> m;
};

class LibUsbDevice_Claim : public UsbDevice_Claim
{
  public:
    LibUsbDevice_Claim(RR_SHARED_PTR<UsbDevice> parent, RR_SHARED_PTR<LibUsb_Functions> f,
                       const UsbDeviceManager_detected_device& detected_device);
    virtual ~LibUsbDevice_Claim();

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

    void AsyncPipeOp(uint8_t ep, boost::asio::mutable_buffer& buf,
                     boost::function<void(const boost::system::error_code&, size_t)> handler);

    // Call with lock
    virtual UsbDeviceStatus ClaimDevice(RR_SHARED_PTR<void>& dev_h);

    // Call with lock
    virtual void ReleaseClaim();

    virtual void DrawDownRequests(boost::function<void()> handler);

    virtual void ClearHalt(uint8_t ep);

    RR_SHARED_PTR<LibUsb_Functions> f;

    RR_SHARED_PTR<libusb_device_handle> device_handle;

    RR_WEAK_PTR<LibUsbDeviceManager> m;
};

class LibUsbDevice : public UsbDevice
{
  protected:
    RR_SHARED_PTR<LibUsb_Functions> f;

  public:
    LibUsbDevice(RR_SHARED_PTR<LibUsbDeviceManager> parent, RR_SHARED_PTR<LibUsb_Functions> f,
                 const UsbDeviceManager_detected_device& device);
    virtual ~LibUsbDevice();

    virtual RR_SHARED_PTR<UsbDevice_Initialize> CreateInitialize();
    virtual RR_SHARED_PTR<UsbDevice_Claim> CreateClaim();
};
} // namespace detail
} // namespace RobotRaconteur
