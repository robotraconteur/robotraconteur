// Copyright 2011-2018 Wason Technology, LLC
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
#include "HardwareTransport_android_private.h"
#endif

namespace RobotRaconteur
{

	HardwareTransport::HardwareTransport(RR_SHARED_PTR<RobotRaconteurNode> node)
		: Transport(node)
	{
		if (!node) throw InvalidArgumentException("Node cannot be null");

		transportopen = false;
		this->node = node;
#ifndef ROBOTRACONTEUR_DISABLE_MESSAGE3
		disable_message3 = false;
#else
		disable_message3 = true;
#endif
#ifndef ROBOTRACONTEUR_DISABLE_STRINGTABLE
		disable_string_table = false;
#else
		disable_string_table = true;
#endif
		disable_async_message_io = false;

		closed = false;

#ifdef ROBOTRACONTEUR_WINDOWS
		RR_SHARED_PTR<SetupApi_Functions> f = RR_MAKE_SHARED<SetupApi_Functions>();
		if (f->LoadFunctions())
		{
			internal1 = f;
		}
#endif

#if defined(ROBOTRACONTEUR_LINUX) && !defined(ROBOTRACONTEUR_ANDROID)
		RR_SHARED_PTR<detail::DBus_Functions> f1=RR_MAKE_SHARED<detail::DBus_Functions>();
		if (f1->LoadFunctions())
		{
			internal1=f1;
		}

		RR_SHARED_PTR<detail::Sdp_Functions> f4=RR_MAKE_SHARED<detail::Sdp_Functions>();
		if (f4->LoadFunctions())
		{
			internal4=f4;
		}

#endif

	}

	HardwareTransport::~HardwareTransport()
	{

	}
	
	bool HardwareTransport::IsServer() const
	{
		return false;
	}

	bool HardwareTransport::IsClient() const
	{
		return true;
	}

	std::string HardwareTransport::GetUrlSchemeString() const
	{
		return "rr+usb";
	}

	bool HardwareTransport::CanConnectService(const std::string& url)
	{		

		if (boost::starts_with(url,"rr+usb://"))
			return true;

		if (boost::starts_with(url, "rr+pci://"))
			return true;

		if (boost::starts_with(url, "rr+industrial://"))
			return true;

		if (boost::starts_with(url, "rr+bluetooth://"))
			return true;

		return false;
	}


	void HardwareTransport::AsyncCreateTransportConnection(const std::string& url, RR_SHARED_PTR<Endpoint> ep, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) >& callback)
	{
		ParseConnectionURLResult url_res = ParseConnectionURL(url);
		if (url_res.nodename.empty() && url_res.nodeid.IsAnyNode())
		{
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
		if (url_res.port != -1) throw ConnectionException("Invalid URL for hardware transport");
		if (url_res.path != "" && url_res.path!= "/") throw ConnectionException("Invalid URL for hardware transport");
				
		if (host!= "localhost" && host!="") throw ConnectionException("Invalid host for hardware transport");
						
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
			throw ConnectionException("Invalid connection transport");
		}

		RR_SHARED_PTR <HardwareTransportConnection_driver::socket_type> socket;

#ifdef ROBOTRACONTEUR_WINDOWS

		if (transport == "usb")
		{
			boost::optional<std::wstring> win_path = detail::HardwareTransport_win_find_usb(internal1, url_res.nodeid, url_res.nodename);
			if (!win_path)
			{
				//If we don't find the USB device using driver interface, attempt to find it using WinUSB.
				RR_SHARED_PTR < detail::WinUsbDeviceManager> m;
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

					boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > h = boost::bind(&HardwareTransport::AsyncCreateTransportConnection2, shared_from_this(), noden, _1, _2, callback);
					m->AsyncCreateTransportConnection(url_res, ep->GetLocalEndpoint(), noden, h);
					return;
				}
			}

			HANDLE h = CreateFileW(win_path->c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION, NULL);
			if (h != INVALID_HANDLE_VALUE)
			{
				socket = RR_MAKE_SHARED<LocalTransport::socket_type>(boost::ref(GetNode()->GetThreadPool()->get_io_service()), h);
			}			
		}
		else if (transport == "pci")
		{
			boost::optional<std::wstring> win_path = detail::HardwareTransport_win_find_pci(internal1, url_res.nodeid, url_res.nodename);
			if (win_path)
			{
				HANDLE h = CreateFileW(win_path->c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION, NULL);
				if (h != INVALID_HANDLE_VALUE)
				{
					socket = RR_MAKE_SHARED<HardwareTransportConnection_driver::socket_type>(boost::ref(GetNode()->GetThreadPool()->get_io_service()), h);
				}
			}
		}
		else if (transport == "bluetooth")
		{
			boost::optional<std::wstring> win_path = detail::HardwareTransport_win_find_bluetooth(internal1, url_res.nodeid, url_res.nodename);
			if (!win_path)
			{
				RR_SHARED_PTR<detail::WinsockBluetoothConnector> bt_connector = RR_MAKE_SHARED<detail::WinsockBluetoothConnector>(shared_from_this());
				bt_connector->Connect(url_res, noden, ep->GetLocalEndpoint(), boost::bind(&HardwareTransport::AsyncCreateTransportConnection2, shared_from_this(), noden, _1, _2, callback));
				return;

			}
			
			HANDLE h = CreateFileW(win_path->c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION, NULL);
			if (h != INVALID_HANDLE_VALUE)
			{
				socket = RR_MAKE_SHARED<HardwareTransportConnection_driver::socket_type>(boost::ref(GetNode()->GetThreadPool()->get_io_service()), h);
			}
			
		}
		else
		{
			throw ConnectionException("Invalid connection transport");
		}

		
#elif defined(ROBOTRACONTEUR_LINUX) && !defined(ROBOTRACONTEUR_ANDROID)
		if (transport=="usb")
		{
			boost::optional<std::string> dev_path = detail::HardwareTransport_linux_find_usb(url_res.nodeid, url_res.nodename);
			if (!dev_path)
			{
				//If we don't find the USB device using driver interface, attempt to find it using WinUSB.
				RR_SHARED_PTR < detail::LibUsbDeviceManager> m;
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

					boost::function<void(RR_SHARED_PTR<ITransportConnection>,RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&HardwareTransport::AsyncCreateTransportConnection2, shared_from_this(), noden, _1, _2, callback);
					m->AsyncCreateTransportConnection(url_res, ep->GetLocalEndpoint(), noden, h);
					return;
				}
			}

			int fd=open(dev_path->c_str(), O_RDWR);
			if (fd > 0)
			{
				RR_SHARED_PTR<HardwareTransportConnection_driver::socket_type> socket1=RR_MAKE_SHARED<HardwareTransportConnection_driver::socket_type>(boost::ref(GetNode()->GetThreadPool()->get_io_service()), fd);
				socket=socket1;
			}
		}
		else if (transport=="pci")
		{
			boost::optional<std::string> dev_path = detail::HardwareTransport_linux_find_pci(url_res.nodeid, url_res.nodename);
			if (dev_path)
			{
				int fd=open(dev_path->c_str(), O_RDWR);
				if (fd > 0)
				{
					RR_SHARED_PTR<HardwareTransportConnection_driver::socket_type> socket1=RR_MAKE_SHARED<HardwareTransportConnection_driver::socket_type>(boost::ref(GetNode()->GetThreadPool()->get_io_service()), fd);
					socket=socket1;
				}
			}
		}
		else if (transport=="bluetooth")
		{

			boost::optional<std::string> dev_path = detail::HardwareTransport_linux_find_bluetooth(url_res.nodeid, url_res.nodename);
			if (!dev_path && internal1 && internal4)
			{
				RR_SHARED_PTR<detail::BluezBluetoothConnector> bt_connector = RR_MAKE_SHARED<detail::BluezBluetoothConnector>(shared_from_this(), internal1, internal4);
				bt_connector->Connect(url_res, noden, ep->GetLocalEndpoint(), boost::bind(&HardwareTransport::AsyncCreateTransportConnection2, shared_from_this(), noden, _1, _2, callback));
				return;
			}

			if (dev_path)
			{
				int fd=open(dev_path->c_str(), O_RDWR);
				if (fd > 0)
				{
					RR_SHARED_PTR<HardwareTransportConnection_driver::socket_type> socket1=RR_MAKE_SHARED<HardwareTransportConnection_driver::socket_type>(boost::ref(GetNode()->GetThreadPool()->get_io_service()), fd);
					socket=socket1;
				}
			}

		}
		else
		{
			throw ConnectionException("Invalid connection transport");
		}

#endif

#ifdef ROBOTRACONTEUR_ANDROID
		if (transport=="bluetooth")
		{
			detail::AndroidHardwareDirector::ConnectBluetooth(shared_from_this(), url_res, noden, ep->GetLocalEndpoint(), boost::bind(&HardwareTransport::AsyncCreateTransportConnection2, shared_from_this(), noden, _1, _2, boost::protect(callback)));
			return;
		}

#endif

		//TODO: test this
		if (!socket) throw ConnectionException("Could not connect to service");		
		boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&HardwareTransport::AsyncCreateTransportConnection2, shared_from_this(), noden, _1, _2, callback);
		HardwareTransport_attach_transport(shared_from_this(), socket, false, ep->GetLocalEndpoint(), noden, url_res.scheme, h);
	}

	void HardwareTransport::AsyncCreateTransportConnection2(const std::string& noden, RR_SHARED_PTR<ITransportConnection> transport, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) >& callback)
	{
		if (err)
		{
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

		callback(transport, RR_SHARED_PTR<RobotRaconteurException>());


	}

	RR_SHARED_PTR<ITransportConnection> HardwareTransport::CreateTransportConnection(const std::string& url, RR_SHARED_PTR<Endpoint> e)
	{
		RR_SHARED_PTR<detail::sync_async_handler<ITransportConnection> > d = RR_MAKE_SHARED<detail::sync_async_handler<ITransportConnection> >(RR_MAKE_SHARED<ConnectionException>("Timeout exception"));

		boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > h
			= boost::bind(&detail::sync_async_handler<ITransportConnection>::operator(), d, _1, _2);
		AsyncCreateTransportConnection(url, e, h);

		return d->end();

	}

	void HardwareTransport::CloseTransportConnection(RR_SHARED_PTR<Endpoint> e)
	{
		RR_SHARED_PTR<ServerEndpoint> e2 = boost::dynamic_pointer_cast<ServerEndpoint>(e);
		if (e2)
		{
			RR_SHARED_PTR<boost::asio::deadline_timer> timer = RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));
			timer->expires_from_now(boost::posix_time::milliseconds(1000));
			RobotRaconteurNode::asio_async_wait(node, timer, boost::bind(&HardwareTransport::CloseTransportConnection_timed, shared_from_this(), boost::asio::placeholders::error, e, timer));
			return;
		}


		RR_SHARED_PTR<ITransportConnection> t;	
		{
			boost::mutex::scoped_lock lock(TransportConnections_lock);
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(e->GetLocalEndpoint());
			if (e1 == TransportConnections.end()) return;			
			t = e1->second;
			TransportConnections.erase(e1);	
		}

		if (t)
		{
			try
			{
				t->Close();
			}
			catch (std::exception&) {}
		}
	}

	void HardwareTransport::CloseTransportConnection_timed(const boost::system::error_code& err, RR_SHARED_PTR<Endpoint> e, RR_SHARED_PTR<void> timer)
	{
		if (err) return;

		RR_SHARED_PTR<ITransportConnection> t;
		{
			boost::mutex::scoped_lock lock(TransportConnections_lock);
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(e->GetLocalEndpoint());
			if (e1 == TransportConnections.end()) return;
			t = e1->second;
		}
		
		if (t)
		{
			try
			{
				t->Close();
			}
			catch (std::exception&) {}
		}
	}

	void HardwareTransport::SendMessage(RR_INTRUSIVE_PTR<Message> m)
	{

		RR_SHARED_PTR<ITransportConnection> t;		
		{
			boost::mutex::scoped_lock lock(TransportConnections_lock);
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(m->header->SenderEndpoint);
			if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
			t = e1->second;
		}
		t->SendMessage(m);
	}

	uint32_t HardwareTransport::TransportCapability(const std::string& name)
	{
		return 0;
	}

	void HardwareTransport::PeriodicCleanupTask()
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		for (RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e = TransportConnections.begin(); e != TransportConnections.end();)
		{
			try
			{
				RR_SHARED_PTR<HardwareTransportConnection> e2 = RR_DYNAMIC_POINTER_CAST<HardwareTransportConnection>(e->second);
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
			catch (std::exception&) {}
		}
	}

	void HardwareTransport::AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{

		RR_SHARED_PTR<ITransportConnection> t;		
		{
			boost::mutex::scoped_lock lock(TransportConnections_lock);
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(m->header->SenderEndpoint);
			if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
			t = e1->second;

		}
		t->AsyncSendMessage(m, handler);
	}

	void HardwareTransport::MessageReceived(RR_INTRUSIVE_PTR<Message> m)
	{
		GetNode()->MessageReceived(m);
	}

	void HardwareTransport::AsyncGetDetectedNodes(const std::vector<std::string>& schemes, boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)>& handler, int32_t timeout)
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

			RR_SHARED_PTR<detail::WinsockBluetoothConnector> bt = RR_MAKE_SHARED<detail::WinsockBluetoothConnector>(shared_from_this());

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
			RR_SHARED_PTR < detail::LibUsbDeviceManager> m;
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
		RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >()), true);
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
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(connection->GetLocalEndpoint());
			if (e1 == TransportConnections.end()) return;			
			if (e1->second == connection)
			{
				TransportConnections.erase(e1);
			}			
		}
		catch (std::exception&) {}

		TransportConnectionClosed(connection->GetLocalEndpoint());
	}

	bool HardwareTransport::GetDisableMessage3()
	{
		boost::mutex::scoped_lock lock(parameter_lock);
		return disable_message3;
	}
	void HardwareTransport::SetDisableMessage3(bool d)
	{
		boost::mutex::scoped_lock lock(parameter_lock);
		disable_message3 = d;
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
	}

	void HardwareTransport::AddUsbDevice(uint16_t vid, uint16_t pid, uint8_t interface_)
	{
#ifdef ROBOTRACONTEUR_WINDOWS
		throw InvalidOperationException("Use WinUsb INF file on Windows to add USB device");
#endif

		boost::mutex::scoped_lock lock(parameter_lock);
		boost::tuple<uint16_t,uint16_t,uint8_t> u=boost::make_tuple<uint16_t,uint16_t,uint8_t>(vid,pid,interface_);
		if (boost::range::find(usb_devices, u) == usb_devices.end())
		{
			usb_devices.push_back(u);
		}

	}

	void HardwareTransport::RemoveUsbDevice(uint16_t vid, uint16_t pid, uint8_t interface_)
	{
#ifdef ROBOTRACONTEUR_WINDOWS
		throw InvalidOperationException("Use WinUsb INF file on Windows to add USB device");
#endif
		boost::mutex::scoped_lock lock(parameter_lock);
		boost::tuple<uint16_t,uint16_t,uint8_t> u=boost::make_tuple<uint16_t,uint16_t,uint8_t>(vid,pid,interface_);

		usb_devices.remove(u);
	}

	bool HardwareTransport::IsValidUsbDevice(uint16_t vid, uint16_t pid, uint8_t interface_)
	{
		boost::mutex::scoped_lock lock(parameter_lock);
		boost::tuple<uint16_t,uint16_t,uint8_t> u=boost::make_tuple<uint16_t,uint16_t,uint8_t>(vid,pid,interface_);
		return (boost::range::find(usb_devices, u) != usb_devices.end());
	}

	void HardwareTransport::CheckConnection(uint32_t endpoint)
	{
		RR_SHARED_PTR<ITransportConnection> t;
		{
			boost::mutex::scoped_lock lock(TransportConnections_lock);
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e = TransportConnections.find(endpoint);
			if (e==TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
			t = e->second;
		}
		t->CheckConnection(endpoint);
	}

	void HardwareTransport::Close()
	{
		{
			boost::mutex::scoped_lock lock(closed_lock);
			if (closed) return;
			closed = true;
		}

		std::list<RR_SHARED_PTR<ITransportConnection> > c;
		{
			boost::mutex::scoped_lock lock(TransportConnections_lock);
			boost::range::copy(this->TransportConnections | boost::adaptors::map_values, std::back_inserter(c));
			TransportConnections.clear();
		}

		BOOST_FOREACH(RR_SHARED_PTR<ITransportConnection> c1, c)
		{
			try
			{
				c1->Close();
			}
			catch (std::exception&) {}
		}

#ifdef ROBOTRACONTEUR_WINDOWS
		{
			boost::mutex::scoped_lock lock(discovery_lock);
			if (internal3)
			{
				RR_SHARED_PTR<detail::HardwareTransport_win_discovery> d = RR_STATIC_POINTER_CAST<detail::HardwareTransport_win_discovery>(internal3);
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
				RR_SHARED_PTR<detail::HardwareTransport_linux_discovery> d = RR_STATIC_POINTER_CAST<detail::HardwareTransport_linux_discovery>(internal3);
				d->Close();
				internal3.reset();
			}
		}
#endif

		close_signal();
	}


	HardwareTransportConnection::HardwareTransportConnection(RR_SHARED_PTR<HardwareTransport> parent, bool server, uint32_t local_endpoint) : ASIOStreamBaseTransport(parent->GetNode())
	{
		this->parent = parent;
		this->server = server;
		this->m_LocalEndpoint = local_endpoint;
		this->m_RemoteEndpoint = 0;
		
		this->HeartbeatPeriod = 30000;
		this->ReceiveTimeout = 600000;

		this->disable_message3 = parent->GetDisableMessage3();
		this->disable_string_table = parent->GetDisableStringTable();
		this->disable_async_io = parent->GetDisableAsyncMessageIO();
	}


	void HardwareTransportConnection::AsyncAttachSocket1(std::string noden, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
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
		if (!p) return;

		RR_INTRUSIVE_PTR<Message> ret = p->SpecialRequest(m, shared_from_this());
		if (ret != 0)
		{
			try
			{
				if ((m->entries.at(0)->EntryType == MessageEntryType_ConnectionTest || m->entries.at(0)->EntryType == MessageEntryType_ConnectionTestRet))
				{
					if (m->entries.at(0)->Error != MessageErrorType_None)
					{
						Close();
						return;
					}
				}

				if ((ret->entries.at(0)->EntryType == MessageEntryType_ConnectClientRet || ret->entries.at(0)->EntryType == MessageEntryType_ReconnectClient) && ret->entries.at(0)->Error == MessageErrorType_None)
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
					}

				}

				boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h
					= boost::bind(&HardwareTransportConnection::SimpleAsyncEndSendMessage, RR_STATIC_POINTER_CAST<HardwareTransportConnection>(shared_from_this()), _1);
				AsyncSendMessage(ret, h);
			}
			catch (std::exception&)
			{
				Close();
			}

			return;
		}




		try
		{

			//TODO: fix this (maybe??)...

			/*boost::asio::ip::address addr=socket->local_endpoint().address();
			uint16_t port=socket->local_endpoint().port();

			std::string connecturl;
			if (addr.is_v4())
			{
			connecturl="local://" + addr.to_string() + ":" + boost::lexical_cast<std::string>(port) + "/";
			}
			else
			{
			boost::asio::ip::address_v6 addr2=addr.to_v6();
			addr2.scope_id(0);
			connecturl="tcp://[" + addr2.to_string() + "]:" + boost::lexical_cast<std::string>(port) + "/";
			}
			*/

			std::string connecturl = scheme + ":///";
			Transport::m_CurrentThreadTransportConnectionURL.reset(new std::string(connecturl));
			Transport::m_CurrentThreadTransport.reset(new RR_SHARED_PTR<ITransportConnection>(RR_STATIC_POINTER_CAST<HardwareTransportConnection>(shared_from_this())));
			p->MessageReceived(m);
		}
		catch (std::exception& exp)
		{
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

		try
		{
			RR_SHARED_PTR<HardwareTransport> p = parent.lock();
			if (p) p->erase_transport(RR_STATIC_POINTER_CAST<HardwareTransportConnection>(shared_from_this()));
		}
		catch (std::exception&) {}
		
		ASIOStreamBaseTransport::Close();

	}	

	uint32_t HardwareTransportConnection::GetLocalEndpoint()
	{
		return m_LocalEndpoint;
	}

	uint32_t HardwareTransportConnection::GetRemoteEndpoint()
	{
		return m_RemoteEndpoint;
	}

	void HardwareTransportConnection::CheckConnection(uint32_t endpoint)
	{
		if (endpoint != m_LocalEndpoint || !connected.load()) throw ConnectionException("Connection lost");
	}

	
	void HardwareTransport_attach_transport(RR_SHARED_PTR<HardwareTransport> parent, RR_SHARED_PTR<HardwareTransportConnection_driver::socket_type> socket, bool server, uint32_t endpoint, std::string noden, const std::string& scheme, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& callback)
	{
		try
		{
			RR_SHARED_PTR<HardwareTransportConnection_driver> t = RR_MAKE_SHARED<HardwareTransportConnection_driver>(parent, server, endpoint, scheme);
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(callback, t, _1);
			t->AsyncAttachSocket(socket, noden, h);
			parent->AddCloseListener(t, &HardwareTransportConnection_driver::Close);
		}
		catch (std::exception&)
		{
			RobotRaconteurNode::TryPostToThreadPool(parent->GetNode(),boost::bind(callback, RR_SHARED_PTR<HardwareTransportConnection>(), RR_MAKE_SHARED<ConnectionException>("Could not connect to service")));
		}

	}

	//HardwareTransportConnection_driver

	HardwareTransportConnection_driver::HardwareTransportConnection_driver(RR_SHARED_PTR<HardwareTransport> parent, bool server, uint32_t local_endpoint, const std::string& scheme)
		: HardwareTransportConnection(parent, server, local_endpoint)
	{
		this->scheme = scheme;
	}

	void HardwareTransportConnection_driver::async_write_some(const_buffers& b, boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
	{
		boost::mutex::scoped_lock lock(socket_lock);
		RobotRaconteurNode::asio_async_write_some(node,socket,b, handler);
	}


	void HardwareTransportConnection_driver::async_read_some(mutable_buffers& b, boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
	{
		boost::mutex::scoped_lock lock(socket_lock);
		RobotRaconteurNode::asio_async_read_some(node,socket,b, handler);
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

	void HardwareTransportConnection_driver::AsyncAttachSocket(RR_SHARED_PTR<HardwareTransportConnection_driver::socket_type> socket, std::string noden, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
	{
		this->socket = socket;

		AsyncAttachSocket1(noden, callback);
	}

	void HardwareTransportConnection_driver::Close1()
	{
		boost::mutex::scoped_lock lock(socket_lock);
		socket->close();
	}

	//HardwareTransportConnection_bluetooth

	HardwareTransportConnection_bluetooth::HardwareTransportConnection_bluetooth(RR_SHARED_PTR<HardwareTransport> parent, bool server, uint32_t local_endpoint)
		: HardwareTransportConnection(parent, server, local_endpoint)
	{
		scheme = "rr+bluetooth";
	}

	void HardwareTransportConnection_bluetooth::async_write_some(const_buffers& b, boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
	{
		boost::mutex::scoped_lock lock(socket_lock);
		RobotRaconteurNode::asio_async_write_some(node, socket, b, handler);
	}


	void HardwareTransportConnection_bluetooth::async_read_some(mutable_buffers& b, boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
	{
		boost::mutex::scoped_lock lock(socket_lock);
		RobotRaconteurNode::asio_async_read_some(node, socket, b, handler);
	}

	size_t HardwareTransportConnection_bluetooth::available()
	{
		boost::mutex::scoped_lock lock(socket_lock);
		return socket->available();
	}

	void HardwareTransportConnection_bluetooth::AsyncAttachSocket(RR_SHARED_PTR<boost::asio::generic::stream_protocol::socket> socket, std::string noden, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
	{
		this->socket = socket;

		AsyncAttachSocket1(noden, callback);
	}

	void HardwareTransportConnection_bluetooth::Close1()
	{
		boost::mutex::scoped_lock lock(socket_lock);
		socket->close();
	}

	void HardwareTransport_attach_transport_bluetooth(RR_SHARED_PTR<HardwareTransport> parent, RR_SHARED_PTR<boost::asio::generic::stream_protocol::socket> socket, bool server, uint32_t endpoint, std::string noden, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& callback)
	{
		try
		{
			RR_SHARED_PTR<HardwareTransportConnection_bluetooth> t = RR_MAKE_SHARED<HardwareTransportConnection_bluetooth>(parent, server, endpoint);
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(callback, t, _1);
			t->AsyncAttachSocket(socket, noden, h);
			parent->AddCloseListener(t, &HardwareTransportConnection_bluetooth::Close);
		}
		catch (std::exception&)
		{
			RobotRaconteurNode::TryPostToThreadPool(parent->GetNode(), boost::bind(callback, RR_SHARED_PTR<HardwareTransportConnection>(), RR_MAKE_SHARED<ConnectionException>("Could not connect to service")));
		}

	}
}

