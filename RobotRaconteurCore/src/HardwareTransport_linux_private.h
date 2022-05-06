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

#include "HardwareTransport_libusb_private.h"
#include "HardwareTransport_bluetoothcommon_private.h"
#include "HardwareTransport_discoverycommon_private.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include <dbus/dbus.h>

#pragma once

typedef void (*dbus_error_init_t)(DBusError* error);
typedef void (*dbus_error_free_t)(DBusError* error);
typedef dbus_bool_t (*dbus_error_is_set_t)(const DBusError* error);
typedef DBusConnection* (*dbus_bus_get_t)(DBusBusType type, DBusError* error);
typedef void (*dbus_connection_set_exit_on_disconnect_t)(DBusConnection* connection, dbus_bool_t exit_on_disconnect);
typedef dbus_bool_t (*dbus_bus_name_has_owner_t)(DBusConnection* connection, const char* name, DBusError* error);
typedef DBusMessage* (*dbus_connection_send_with_reply_and_block_t)(DBusConnection* connection, DBusMessage* message,
                                                                    int timeout_milliseconds, DBusError* error);
typedef void (*dbus_connection_unref_t)(DBusConnection* connection);
typedef DBusMessage* (*dbus_message_new_method_call_t)(const char* bus_name, const char* path, const char* iface,
                                                       const char* method);
typedef void (*dbus_message_unref_t)(DBusMessage* message);
typedef dbus_bool_t (*dbus_message_iter_init_t)(DBusMessage* message, DBusMessageIter* iter);
typedef dbus_bool_t (*dbus_message_iter_next_t)(DBusMessageIter* iter);
typedef int (*dbus_message_iter_get_arg_type_t)(DBusMessageIter* iter);
typedef void (*dbus_message_iter_recurse_t)(DBusMessageIter* iter, DBusMessageIter* sub);
typedef void (*dbus_message_iter_get_basic_t)(DBusMessageIter* iter, void* value);

typedef int (*str2ba_t)(const char* str, bdaddr_t* ba);
typedef sdp_session_t* (*sdp_connect_t)(const bdaddr_t* src, const bdaddr_t* dst, uint32_t flags);
typedef int (*sdp_close_t)(sdp_session_t* session);
typedef sdp_list_t* (*sdp_list_append_t)(sdp_list_t* list, void* d);
typedef void (*sdp_list_free_t)(sdp_list_t* list, sdp_free_func_t f);
typedef uuid_t* (*sdp_uuid128_create_t)(uuid_t* uuid, const void* data);
typedef int (*sdp_uuid2strn_t)(const uuid_t* uuid, char* str, size_t n);
typedef int (*sdp_uuid128_cmp_t)(const void* p1, const void* p2);
typedef int (*sdp_service_search_attr_req_t)(sdp_session_t* session, const sdp_list_t* search,
                                             sdp_attrreq_type_t reqtype, const sdp_list_t* attrid_list,
                                             sdp_list_t** rsp_list);
typedef void (*sdp_record_free_t)(sdp_record_t* rec);
typedef int (*sdp_get_access_protos_t)(const sdp_record_t* rec, sdp_list_t** protos);
typedef int (*sdp_get_proto_port_t)(const sdp_list_t* list, int proto);

namespace RobotRaconteur
{
namespace detail
{

#define DBUS_FUNCTIONS_INIT(M)                                                                                         \
    M(dbus_error_init)                                                                                                 \
    M(dbus_error_free)                                                                                                 \
    M(dbus_error_is_set)                                                                                               \
    M(dbus_bus_get)                                                                                                    \
    M(dbus_connection_set_exit_on_disconnect)                                                                          \
    M(dbus_bus_name_has_owner)                                                                                         \
    M(dbus_connection_send_with_reply_and_block)                                                                       \
    M(dbus_connection_unref)                                                                                           \
    M(dbus_message_new_method_call)                                                                                    \
    M(dbus_message_unref)                                                                                              \
    M(dbus_message_iter_init)                                                                                          \
    M(dbus_message_iter_next)                                                                                          \
    M(dbus_message_iter_get_arg_type)                                                                                  \
    M(dbus_message_iter_recurse)                                                                                       \
    M(dbus_message_iter_get_basic)

#define DBUS_FUNCTIONS_FIELD_INIT(t) t##_t t;
#define DBUS_FUNCTIONS_PTR_VOID(t) t = NULL;
#define DBUS_FUNCTIONS_PTR_INIT(t)                                                                                     \
    t = reinterpret_cast<t##_t>(dlsym(lib_handle, #t));                                                                \
    if (t == NULL)                                                                                                     \
        return false;

class DBus_Functions : public boost::noncopyable
{
  private:
    void* lib_handle;

  public:
    DBUS_FUNCTIONS_INIT(DBUS_FUNCTIONS_FIELD_INIT);

    DBus_Functions();
    bool LoadFunctions();
    virtual ~DBus_Functions();
};

#define SDP_FUNCTIONS_INIT(M)                                                                                          \
    M(str2ba)                                                                                                          \
    M(sdp_connect)                                                                                                     \
    M(sdp_close)                                                                                                       \
    M(sdp_list_append)                                                                                                 \
    M(sdp_list_free)                                                                                                   \
    M(sdp_uuid128_create)                                                                                              \
    M(sdp_uuid2strn)                                                                                                   \
    M(sdp_uuid128_cmp)                                                                                                 \
    M(sdp_service_search_attr_req)                                                                                     \
    M(sdp_record_free)                                                                                                 \
    M(sdp_get_access_protos)                                                                                           \
    M(sdp_get_proto_port)

#define SDP_FUNCTIONS_FIELD_INIT(t) t##_t t;
#define SDP_FUNCTIONS_PTR_VOID(t) t = NULL;
#define SDP_FUNCTIONS_PTR_INIT(t)                                                                                      \
    t = reinterpret_cast<t##_t>(dlsym(lib_handle, #t));                                                                \
    if (t == NULL)                                                                                                     \
        return false;

class Sdp_Functions : public boost::noncopyable
{
  private:
    void* lib_handle;

  public:
    SDP_FUNCTIONS_INIT(SDP_FUNCTIONS_FIELD_INIT);

    Sdp_Functions();
    bool LoadFunctions();
    virtual ~Sdp_Functions();
};

class BluezBluetoothConnector : public BluetoothConnector<sockaddr_rc, AF_BLUETOOTH, BTPROTO_RFCOMM>
{
  public:
    BluezBluetoothConnector(RR_SHARED_PTR<HardwareTransport> parent, RR_SHARED_PTR<void> dbus_f,
                            RR_SHARED_PTR<void> sdp_f);

    virtual ~BluezBluetoothConnector() {}

    virtual std::list<sockaddr_rc> GetDeviceAddresses();

    virtual std::list<device_info> GetDeviceNodes(sockaddr_rc addr);

    class mysock
    {
      public:
        int sock;
        mysock(int sock) { this->sock = sock; }
        ~mysock()
        {
            if (sock > 0)
                close(sock);
        }
    };

    RR_SHARED_PTR<DBus_Functions> dbus_f;
    RR_SHARED_PTR<Sdp_Functions> sdp_f;
};

class HardwareTransport_linux_discovery
    : public HardwareTransport_discovery<LibUsbDeviceManager, BluezBluetoothConnector>
{
  public:
    HardwareTransport_linux_discovery(RR_SHARED_PTR<HardwareTransport> parent, const std::vector<std::string>& schemes,
                                      RR_SHARED_PTR<LibUsbDeviceManager> usb,
                                      RR_SHARED_PTR<BluezBluetoothConnector> bt);
    virtual ~HardwareTransport_linux_discovery() {}

    virtual void Init();

    virtual void Close();

    virtual std::vector<NodeDiscoveryInfo> GetDriverDevices();

  protected:
    std::vector<NodeDiscoveryInfo> GetDriverDevices1(const std::string& transport, const std::string& scheme);

    void NetlinkMessageReceived(const boost::system::error_code& ec, size_t bytes_transferred);

    RR_SHARED_PTR<boost::asio::generic::datagram_protocol::socket> netlink_sock;

    bool running;
    boost::mutex this_lock;
    boost::shared_array<uint8_t> msg;

    static const size_t NL_MAX_PAYLOAD = 8192;
};

boost::optional<std::string> HardwareTransport_linux_find_deviceinterface(boost::string_ref transport_type,
                                                                          const NodeID& nodeid,
                                                                          boost::string_ref nodename);

boost::optional<std::string> HardwareTransport_linux_find_usb(const NodeID& nodeid, boost::string_ref nodename);

boost::optional<std::string> HardwareTransport_linux_find_pci(const NodeID& nodeid, boost::string_ref nodename);

boost::optional<std::string> HardwareTransport_linux_find_bluetooth(const NodeID& nodeid, boost::string_ref nodename);

std::list<boost::tuple<NodeID, std::string> > HardwareTransport_linux_find_deviceinterfaces(
    boost::string_ref transport_type);

} // namespace detail
} // namespace RobotRaconteur
