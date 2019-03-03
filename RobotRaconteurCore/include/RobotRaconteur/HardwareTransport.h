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

#include "RobotRaconteur/RobotRaconteurNode.h"
#include <boost/shared_array.hpp>
#include <boost/asio/windows/stream_handle.hpp>

#pragma once



namespace RobotRaconteur
{
	class ROBOTRACONTEUR_CORE_API LocalTransportConnection;
	class ROBOTRACONTEUR_CORE_API HardwareTransport : public Transport, public RR_ENABLE_SHARED_FROM_THIS<HardwareTransport>
	{
		friend class HardwareTransportConnection;

	private:
		
		bool transportopen;		

	public:

		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> > TransportConnections;
		boost::mutex TransportConnections_lock;
		
		HardwareTransport(RR_SHARED_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp());

		virtual ~HardwareTransport();


	public:
		virtual bool IsServer() const;

		virtual bool IsClient() const;
		
		virtual std::string GetUrlSchemeString() const;

		virtual void SendMessage(RR_INTRUSIVE_PTR<Message> m);

		virtual void AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

		virtual void AsyncCreateTransportConnection(const std::string& url, RR_SHARED_PTR<Endpoint> e, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) >& callback);
		
		virtual RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(const std::string& url, RR_SHARED_PTR<Endpoint> e);

		virtual void CloseTransportConnection(RR_SHARED_PTR<Endpoint> e);

	protected:

		virtual void AsyncCreateTransportConnection2(const std::string& noden, RR_SHARED_PTR<ITransportConnection> transport, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) >& callback);

		virtual void CloseTransportConnection_timed(const boost::system::error_code& err, RR_SHARED_PTR<Endpoint> e, RR_SHARED_PTR<void> timer);

	public:
		
		virtual bool CanConnectService(const std::string& url);

		virtual void Close();

		virtual void CheckConnection(uint32_t endpoint);

		virtual void PeriodicCleanupTask();

		uint32_t TransportCapability(const std::string& name);

		virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m);

		virtual void AsyncGetDetectedNodes(const std::vector<std::string>& schemes, boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)>& handler, int32_t timeout = RR_TIMEOUT_INFINITE);

		virtual bool GetDisableMessage3();
		virtual void SetDisableMessage3(bool d);

		virtual bool GetDisableStringTable();
		virtual void SetDisableStringTable(bool d);

		virtual bool GetDisableAsyncMessageIO();
		virtual void SetDisableAsyncMessageIO(bool d);

		virtual void AddUsbDevice(uint16_t vid, uint16_t pid, uint8_t interface_);
		virtual void RemoveUsbDevice(uint16_t vid, uint16_t pid, uint8_t interface_);
	
		virtual bool IsValidUsbDevice(uint16_t vid, uint16_t pid, uint8_t interface_);
		
		virtual void register_transport(RR_SHARED_PTR<ITransportConnection> connection);
		virtual void erase_transport(RR_SHARED_PTR<ITransportConnection> connection);
	
		template<typename T, typename F>
		boost::signals2::connection AddCloseListener(RR_SHARED_PTR<T> t, const F& f)
		{
			boost::mutex::scoped_lock lock(closed_lock);
			if (closed)
			{
				lock.unlock();
				boost::bind(f, t) ();
				return boost::signals2::connection();
			}

			return close_signal.connect(boost::signals2::signal<void()>::slot_type(
				boost::bind(f, t.get())
			).track(t));
		}

	protected:

		boost::mutex parameter_lock;
		bool disable_message3;
		bool disable_string_table;
		bool disable_async_message_io;

		boost::mutex discovery_lock;

		RR_SHARED_PTR<void> internal1;
		RR_SHARED_PTR<void> internal2;
		RR_SHARED_PTR<void> internal3;
		RR_SHARED_PTR<void> internal4;

		std::list<boost::tuple<uint16_t,uint16_t,uint8_t> > usb_devices;

		bool closed;
		boost::mutex closed_lock;
		boost::signals2::signal<void()> close_signal;

	};
	
}
