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

#include "HardwareTransport_linux_private.h"
#include "HardwareTransport_private.h"
#include <boost/foreach.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/locale.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <dlfcn.h>
#include <linux/netlink.h>
#include <boost/algorithm/string.hpp>
#include <boost/range/as_array.hpp>

namespace RobotRaconteur
{
namespace detail
{

//LibUsb_Functions
DBus_Functions::DBus_Functions()
{
	lib_handle=NULL;
	DBUS_FUNCTIONS_INIT(DBUS_FUNCTIONS_PTR_VOID);
}

bool DBus_Functions::LoadFunctions()
{
	if (lib_handle) throw InvalidOperationException("dbus functions already loaded");
	lib_handle=dlopen("libdbus-1.so", RTLD_LAZY);
	if (!lib_handle) return false;

	DBUS_FUNCTIONS_INIT(DBUS_FUNCTIONS_PTR_INIT);

	return true;
}

DBus_Functions::~DBus_Functions()
{
	if (lib_handle)
	{
		dlclose(lib_handle);
		lib_handle=NULL;
	}
}
//End LibUsb_Functions

//Sdp_Functions
Sdp_Functions::Sdp_Functions()
{
	lib_handle=NULL;
	SDP_FUNCTIONS_INIT(SDP_FUNCTIONS_PTR_VOID);
}

bool Sdp_Functions::LoadFunctions()
{
	if (lib_handle) throw InvalidOperationException("sdp functions already loaded");
	lib_handle=dlopen("libbluetooth.so", RTLD_LAZY);
	if (!lib_handle) return false;

	SDP_FUNCTIONS_INIT(SDP_FUNCTIONS_PTR_INIT);

	return true;
}

Sdp_Functions::~Sdp_Functions()
{
	if (lib_handle)
	{
		dlclose(lib_handle);
		lib_handle=NULL;
	}
}
//End Sdp_Functions

//BluezBluetoothConnector

BluezBluetoothConnector::BluezBluetoothConnector(RR_SHARED_PTR<HardwareTransport> parent, RR_SHARED_PTR<void> dbus_f, RR_SHARED_PTR<void> sdp_f)
	: BluetoothConnector(parent)
{
	this->dbus_f = RR_STATIC_POINTER_CAST<DBus_Functions>(dbus_f);
	this->sdp_f = RR_STATIC_POINTER_CAST<Sdp_Functions>(sdp_f);

}

std::list<sockaddr_rc> BluezBluetoothConnector::GetDeviceAddresses()
{
	std::list<sockaddr_rc> o;

	DBusConnection* conn;
	DBusError err;

	dbus_f->dbus_error_init(&err);
	conn = dbus_f->dbus_bus_get(DBUS_BUS_SYSTEM, &err);
	if (dbus_f->dbus_error_is_set(&err))
	{
		dbus_f->dbus_error_free(&err);
	   return o;
	}

	dbus_f->dbus_connection_set_exit_on_disconnect(conn, FALSE);

	if (conn == NULL)
	{
		return o;
	}

	dbus_bool_t hcid_exists;
	dbus_f->dbus_error_init(&err);
	hcid_exists = dbus_f->dbus_bus_name_has_owner(conn, "org.bluez", &err);
	if (dbus_f->dbus_error_is_set(&err)) {
		dbus_f->dbus_error_free(&err);

		return o;
	}

	if (!hcid_exists)
		return o;

	DBusMessage* message = dbus_f->dbus_message_new_method_call("org.bluez", "/", "org.bluez.Manager", "DefaultAdapter");
	if (message == NULL) {
		dbus_f->dbus_connection_unref(conn);
		return o;
	}

	DBusMessage* reply = dbus_f->dbus_connection_send_with_reply_and_block(conn, message, -1, &err);

	dbus_f->dbus_message_unref(message);

	if (dbus_f->dbus_error_is_set(&err)) {
		dbus_f->dbus_error_free(&err);
		dbus_f->dbus_connection_unref(conn);
		/* No adapter */
		return o;
	}

	DBusMessageIter reply_iter;
	dbus_f->dbus_message_iter_init(reply, &reply_iter);
	if (dbus_f->dbus_message_iter_get_arg_type(&reply_iter) !=
						DBUS_TYPE_OBJECT_PATH) {
		dbus_f->dbus_message_unref(reply);
		dbus_f->dbus_connection_unref(conn);
		return o;
	}


	char* adapter1;

	dbus_f->dbus_message_iter_get_basic(&reply_iter, &adapter1);

	std::string adapter(adapter1);

	dbus_f->dbus_message_unref(reply);

	message = dbus_f->dbus_message_new_method_call("org.bluez", adapter.c_str(), "org.bluez.Adapter",	"ListDevices");
	if (message == NULL)
		return o;

	reply = dbus_f->dbus_connection_send_with_reply_and_block(conn, message, -1, &err);

	dbus_f->dbus_message_unref(message);

	if (dbus_f->dbus_error_is_set(&err)) {
		dbus_f->dbus_error_free(&err);
		return o;
	}

	dbus_f->dbus_message_iter_init(reply, &reply_iter);
	if (dbus_f->dbus_message_iter_get_arg_type(&reply_iter) != DBUS_TYPE_ARRAY) {
		dbus_f->dbus_message_unref(reply);
		return o;
	}

	dbus_f->dbus_message_iter_init(reply, &reply_iter);
	if (dbus_f->dbus_message_iter_get_arg_type(&reply_iter) != DBUS_TYPE_ARRAY) {
		dbus_f->dbus_message_unref(reply);
		return o;
	}

	DBusMessageIter iter_array;
	dbus_f->dbus_message_iter_recurse(&reply_iter, &iter_array);
	while (dbus_f->dbus_message_iter_get_arg_type(&iter_array) == DBUS_TYPE_OBJECT_PATH)
	{
		const char *object_path;
		char *name = NULL;
		char *bdaddr = NULL;

		dbus_f->dbus_message_iter_get_basic(&iter_array, &object_path);

		DBusMessage* message1;
		DBusMessage* reply1;

		message1 = dbus_f->dbus_message_new_method_call("org.bluez", object_path, "org.bluez.Device", "GetProperties");
		reply1 = dbus_f->dbus_connection_send_with_reply_and_block(conn,	message1, -1, &err);
		if (dbus_f->dbus_error_is_set(&err))
		{
			dbus_f->dbus_error_free(&err);
			dbus_f->dbus_error_init(&err);
			continue;
		}
		dbus_f->dbus_message_unref(message1);

		DBusMessageIter reply_iter1;
		dbus_f->dbus_message_iter_init(reply1, &reply_iter1);

		if (dbus_f->dbus_message_iter_get_arg_type(&reply_iter1) != DBUS_TYPE_ARRAY)
				continue;

		DBusMessageIter reply_iter_entry1;
		dbus_f->dbus_message_iter_recurse(&reply_iter1, &reply_iter_entry1);

		bool address_found=false;
		bool service_uuid_invalid=false;
		sockaddr_rc a;
		memset(&a, 0, sizeof(a));
		a.rc_family=AF_BLUETOOTH;

		while (dbus_f->dbus_message_iter_get_arg_type(&reply_iter_entry1) == DBUS_TYPE_DICT_ENTRY) {
			const char *key;
			DBusMessageIter dict_entry, iter_dict_val;

			dbus_f->dbus_message_iter_recurse(&reply_iter_entry1, &dict_entry);

			/* Key == Class ? */
			dbus_f->dbus_message_iter_get_basic(&dict_entry, &key);
			if (!key) {
				dbus_f->dbus_message_iter_next(&reply_iter_entry1);
				continue;
			}

			if (!dbus_f->dbus_message_iter_next(&dict_entry))
			{
				continue;
			}

			if (strcmp(key,"Address") == 0)
			{
				dbus_f->dbus_message_iter_recurse(&dict_entry, &iter_dict_val);
				const char* value;
				if(DBUS_TYPE_STRING == dbus_f->dbus_message_iter_get_arg_type(&iter_dict_val))
				{
					dbus_f->dbus_message_iter_get_basic(&iter_dict_val, &value);

					sdp_f->str2ba(value, &a.rc_bdaddr);
					address_found=true;
				}
			}

			if (strcmp(key, "UUIDs")==0)
			{
				try
				{
					boost::uuids::uuid svc_uuid={0x25, 0xbb, 0x0b, 0x62, 0x86, 0x1a, 0x49, 0x74, 0xa1, 0xb8, 0x18, 0xed, 0x54, 0x95, 0xaa, 0x07};

					dbus_f->dbus_message_iter_recurse(&dict_entry, &iter_dict_val);

					if(DBUS_TYPE_ARRAY == dbus_f->dbus_message_iter_get_arg_type(&iter_dict_val))
					{
						service_uuid_invalid=true;
						DBusMessageIter iter_uuid_val;
						dbus_f->dbus_message_iter_recurse(&iter_dict_val, &iter_uuid_val);

						while (DBUS_TYPE_STRING == dbus_f->dbus_message_iter_get_arg_type(&iter_uuid_val))
						{
							try
							{
								const char* value1;
								dbus_f->dbus_message_iter_get_basic(&iter_uuid_val, &value1);
								std::string value(value1);
								boost::uuids::string_generator gen;
								boost::uuids::uuid uuid1=gen(value);

								if (uuid1==svc_uuid)
								{
									service_uuid_invalid=false;
								}

							}
							catch (boost::bad_lexical_cast&) {}

							dbus_f->dbus_message_iter_next(&iter_uuid_val);
						}
					}
				}
				catch (boost::bad_lexical_cast&) {}

			}

			dbus_f->dbus_message_iter_next(&reply_iter_entry1);
		}

		if (address_found && !service_uuid_invalid)
		{
			o.push_back(a);
		}

		dbus_f->dbus_message_unref(reply1);
		dbus_f->dbus_message_iter_next(&iter_array);
	}

	dbus_f->dbus_message_unref(reply);

	return o;
}

std::list<BluezBluetoothConnector::device_info> BluezBluetoothConnector::GetDeviceNodes(sockaddr_rc addr)
{
	std::list<BluezBluetoothConnector::device_info> o;

	const uint8_t svc_uuid_int[] = {0x25, 0xbb, 0x0b, 0x62, 0x86, 0x1a, 0x49, 0x74, 0xa1, 0xb8, 0x18, 0xed, 0x54, 0x95, 0xaa, 0x07};
	uuid_t svc_uuid;

	bdaddr_t target=addr.rc_bdaddr;

	bdaddr_t bdaddr_any1;
	memset(&bdaddr_any1, 0, sizeof(bdaddr_any1));
	sdp_session_t* session = sdp_f->sdp_connect(&bdaddr_any1 , &target, SDP_RETRY_IF_BUSY );
	if (!session)
	{
		return o;
	}



	sdp_f->sdp_uuid128_create( &svc_uuid, &svc_uuid_int );
	sdp_list_t* search_list = sdp_f->sdp_list_append( NULL, &svc_uuid );

	uint32_t range = 0x0000ffff;
	sdp_list_t* attrid_list = sdp_f->sdp_list_append( NULL, &range );

	sdp_list_t* r;

	if(sdp_f->sdp_service_search_attr_req( session, search_list, SDP_ATTR_REQ_RANGE, attrid_list, &r))
	{
		sdp_f->sdp_list_free(search_list,NULL);
		sdp_f->sdp_list_free(attrid_list,NULL);
		sdp_f->sdp_close(session);
		return o;
	}

	sdp_f->sdp_list_free(search_list,NULL);
	sdp_f->sdp_list_free(attrid_list,NULL);

	for (; r; r = r->next ) {

		device_info o1;
		o1.addr.rc_family=AF_BLUETOOTH;
		o1.addr.rc_bdaddr=addr.rc_bdaddr;

		sdp_record_t *rec = reinterpret_cast<sdp_record_t*>(r->data);

		sdp_list_t* attrlist=rec->attrlist;

		for (; attrlist; attrlist = attrlist->next)
		{
			sdp_data_t* a=reinterpret_cast<sdp_data_t*>(attrlist->data);
			if(a->attrId == 0xF001 && a->dtd == SDP_TEXT_STR8)
			{
				o1.nodeid_str=std::string(a->val.str);
			}
			if(a->attrId == 0xF002 && a->dtd == SDP_TEXT_STR8)
			{
				o1.nodename_str=std::string(a->val.str);
			}
		}

		sdp_list_t* protos=NULL;
		if (!sdp_f->sdp_get_access_protos(rec, &protos)) {
			uint8_t ch = sdp_f->sdp_get_proto_port(protos, RFCOMM_UUID);
			sdp_list_foreach(protos, reinterpret_cast<sdp_list_func_t>(sdp_f->sdp_list_free), NULL);
			sdp_f->sdp_list_free(protos, NULL);
			protos = NULL;
			o1.addr.rc_channel=ch;
			o.push_back(o1);
		}

		sdp_f->sdp_record_free( rec );
	}

	sdp_f->sdp_close(session);
	return o;
}

//End BluezBluetoothConnector

//HardwareTransport_linux_discovery

HardwareTransport_linux_discovery::HardwareTransport_linux_discovery(RR_SHARED_PTR<HardwareTransport> parent, const std::vector<std::string>& schemes, RR_SHARED_PTR<LibUsbDeviceManager> usb, RR_SHARED_PTR<BluezBluetoothConnector> bt)
: HardwareTransport_discovery(parent, schemes, usb, bt)
{	
	running=false;
}

void HardwareTransport_linux_discovery::Init()
{
	boost::mutex::scoped_lock lock(this_lock);

	int nl_socket;
	struct sockaddr_nl src_addr;
	msg.reset(new uint8_t[NL_MAX_PAYLOAD]);
	int ret;

	// Prepare source address
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid();
	src_addr.nl_groups = -1;

	nl_socket = socket(AF_NETLINK, (SOCK_DGRAM | SOCK_CLOEXEC),      NETLINK_KOBJECT_UEVENT);
	if (nl_socket < 0) {
		return;
	}

	ret = bind(nl_socket, reinterpret_cast<struct sockaddr*>(&src_addr), sizeof(src_addr));
	if (ret) {
		close(nl_socket);
		return;
	}

	running=true;

	boost::asio::generic::datagram_protocol p(AF_NETLINK, NETLINK_KOBJECT_UEVENT);
	RR_SHARED_PTR<boost::asio::generic::datagram_protocol::socket> nl_socket1(new boost::asio::generic::datagram_protocol::socket(GetParent()->GetNode()->GetThreadPool()->get_io_context(), p, nl_socket));
	netlink_sock=nl_socket1;
	netlink_sock->async_receive(boost::asio::buffer(msg.get(), NL_MAX_PAYLOAD), boost::bind(&HardwareTransport_linux_discovery::NetlinkMessageReceived, RR_STATIC_POINTER_CAST<HardwareTransport_linux_discovery>(shared_from_this()), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

}

void HardwareTransport_linux_discovery::Close()
{
	boost::mutex::scoped_lock lock(this_lock);
	running=false;
	if (netlink_sock)
	{
		netlink_sock->close();
	}
}

std::vector<NodeDiscoveryInfo> HardwareTransport_linux_discovery::GetDriverDevices()
{
	std::vector<NodeDiscoveryInfo> v1=GetDriverDevices1("usb", "rr+usb");
	std::vector<NodeDiscoveryInfo> v2=GetDriverDevices1("pci", "rr+pci");
	std::vector<NodeDiscoveryInfo> v3=GetDriverDevices1("bluetooth", "rr+bluetooth");

	std::vector<NodeDiscoveryInfo> o;
	boost::range::copy(v1, std::back_inserter(o));
	boost::range::copy(v2, std::back_inserter(o));
	boost::range::copy(v3, std::back_inserter(o));
	return o;

}

std::vector<NodeDiscoveryInfo> HardwareTransport_linux_discovery::GetDriverDevices1(const std::string& transport, const std::string& scheme)
{
	std::vector<NodeDiscoveryInfo> o;

	try
	{
		RR_SHARED_PTR<RobotRaconteurNode> node = GetParent()->GetNode();

		std::list<boost::tuple<NodeID, std::string> > usb_dev = HardwareTransport_linux_find_deviceinterfaces(transport);
		typedef boost::tuple<NodeID, std::string> e_type;

		BOOST_FOREACH(e_type& e, usb_dev)
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
	catch (std::exception&) {}

	return o;
}

void HardwareTransport_linux_discovery::NetlinkMessageReceived(const boost::system::error_code& ec, size_t bytes_transferred)
{
	boost::mutex::scoped_lock lock(this_lock);

	if (ec || bytes_transferred==0)
	{
		if (running)
		{
			throw OperationFailedException("Failure reading for device changes");
		}
		return;
	}

	boost::iterator_range<char*> evt(reinterpret_cast<char*>(msg.get()), reinterpret_cast<char*>(msg.get())+bytes_transferred);

	std::vector<std::string> evt1;
	boost::split(evt1, evt, boost::is_any_of(boost::as_array("\0")));
	BOOST_FOREACH(std::string& s1, evt1)
	{
		std::vector<std::string> l;
		boost::split(l, s1, boost::is_any_of("="));
		if (l.size() >= 2)
		{
			if (l[0] == "SUBSYSTEM")
			{

				if(l[1] == "usb")
				{
					GetUsbDevices(boost::bind(&HardwareTransport_linux_discovery::OnDeviceChanged, RR_STATIC_POINTER_CAST<HardwareTransport_linux_discovery>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1)));
					return;
				}

				if (l[1] == "robotraconteur")
				{
					std::vector<NodeDiscoveryInfo> devices=GetDriverDevices();
					RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > devices1=RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >(devices);
					RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&HardwareTransport_linux_discovery::OnDeviceChanged, RR_STATIC_POINTER_CAST<HardwareTransport_linux_discovery>(shared_from_this()), devices1));
					return;
				}


			}
		}
	}

	//printf("Got uevent message: %u\n", (uint32_t)bytes_transferred);

	//TODO: process uevent message

	if (running)
	{
		netlink_sock->async_receive(boost::asio::buffer(msg.get(), NL_MAX_PAYLOAD), boost::bind(&HardwareTransport_linux_discovery::NetlinkMessageReceived, RR_STATIC_POINTER_CAST<HardwareTransport_linux_discovery>(shared_from_this()), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
}

//End HardwareTransport_linux_discovery

//Driver find functions

static std::string HardwareTransport_read_sysfs_attr(const boost::filesystem::path& p)
{
	int rv;
	char attr_buf[256];
	int f1=open(p.c_str(), O_RDONLY);
	if (f1 < 0) throw ConnectionException("Attribute not found");
	rv = pread(f1, attr_buf, sizeof(attr_buf), 0);
	close(f1);
	if (rv< 0) throw ConnectionException("Could not read attribute");
	return std::string(attr_buf, strnlen(attr_buf, sizeof(attr_buf)));

}

boost::optional<std::string> HardwareTransport_linux_find_deviceinterface(boost::string_ref transport_type, const NodeID& nodeid, boost::string_ref nodename)
{
	boost::filesystem::path rr_class="/sys/class/robotraconteur";

	if (!boost::filesystem::is_directory(rr_class))
	{
		return boost::optional<std::string>();
	}

	boost::filesystem::directory_iterator end_iter;
	for (boost::filesystem::directory_iterator dir_itr(rr_class); dir_itr != end_iter; dir_itr++)
	{
		std::string dev_name="/dev/" + dir_itr->path().filename().string();
		NodeID dev_nodeid;
		std::string dev_nodename;

		try
		{
			std::string dev_transport=HardwareTransport_read_sysfs_attr((dir_itr->path() / "transport"));
			size_t n1=transport_type.size();
			size_t n2=dev_transport.size();
			if (dev_transport != transport_type)
			{
				continue;

			}
		}
		catch (std::exception&)
		{
			continue;
		}

		try
		{
			std::string dev_nodeid_str=HardwareTransport_read_sysfs_attr((dir_itr->path() / "nodeid"));
			dev_nodeid=NodeID(dev_nodeid_str);
		}
		catch (std::exception&) {}

		try
		{
			dev_nodename=HardwareTransport_read_sysfs_attr((dir_itr->path() / "nodename"));

			boost::trim(dev_nodename);
		}
		catch (std::exception&) {}

		if (dev_nodeid.IsAnyNode() && dev_nodename.empty())
		{
			continue;
		}

		bool match=false;
		if (!nodeid.IsAnyNode() && !nodename.empty())
		{
			if (nodeid == dev_nodeid && nodename == dev_nodename)
			{
				match=true;
			}
		}
		else if (!nodename.empty())
		{
			if (nodename == dev_nodename)
			{
				match=true;
			}
		}
		else if (!nodeid.IsAnyNode())
		{
			if (nodeid == dev_nodeid)
			{
				match=true;
			}
		}

		if (match)
		{
			return dev_name;
		}
	}

	return boost::optional<std::string>();
}

boost::optional<std::string> HardwareTransport_linux_find_usb(const NodeID& nodeid, boost::string_ref nodename)
{
	return HardwareTransport_linux_find_deviceinterface("usb", nodeid, nodename);
}

boost::optional<std::string> HardwareTransport_linux_find_pci(const NodeID& nodeid, boost::string_ref nodename)
{
	return HardwareTransport_linux_find_deviceinterface("pci", nodeid, nodename);
}

boost::optional<std::string> HardwareTransport_linux_find_bluetooth(const NodeID& nodeid, boost::string_ref nodename)
{
	return HardwareTransport_linux_find_deviceinterface("bluetooth", nodeid, nodename);
}

std::list<boost::tuple<NodeID,std::string> > HardwareTransport_linux_find_deviceinterfaces(boost::string_ref transport_type)
{
	std::list<boost::tuple<NodeID,std::string> > o;

	boost::filesystem::path rr_class="/sys/class/robotraconteur";

	if (boost::filesystem::is_directory(rr_class))
	{
		boost::filesystem::directory_iterator end_iter;
		for (boost::filesystem::directory_iterator dir_itr(rr_class); dir_itr != end_iter; dir_itr++)
		{
			std::string dev_name="/dev/" + dir_itr->path().filename().string();
			NodeID dev_nodeid;
			std::string dev_nodename;

			try
			{
				std::string dev_transport=HardwareTransport_read_sysfs_attr((dir_itr->path() / "transport"));
				size_t n1=transport_type.size();
				size_t n2=dev_transport.size();
			}
			catch (std::exception&)
			{
				continue;
			}

			try
			{
				std::string dev_nodeid_str=HardwareTransport_read_sysfs_attr((dir_itr->path() / "nodeid"));
				dev_nodeid=NodeID(dev_nodeid_str);
			}
			catch (std::exception&) {}

			try
			{
				dev_nodename=HardwareTransport_read_sysfs_attr((dir_itr->path() / "nodename"));

				boost::trim(dev_nodename);
			}
			catch (std::exception&) {}

			if (dev_nodeid.IsAnyNode() && dev_nodename.empty())
			{
				continue;
			}

			o.push_back(boost::make_tuple(dev_nodeid, dev_nodename));
		}
	}

	return o;
}


}
}
