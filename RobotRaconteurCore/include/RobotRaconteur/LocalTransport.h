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


	namespace detail
	{
#ifdef ROBOTRACONTEUR_WINDOWS
		namespace windows
		{
			// Boost ASIO doesn't have a named pipe acceptor :/
			class named_pipe_acceptor;
		}
#endif					
		class LocalTransportFDs;
		class LocalTransportDiscovery;
	}
		
	class ROBOTRACONTEUR_CORE_API LocalTransport : public Transport, public RR_ENABLE_SHARED_FROM_THIS<LocalTransport>
	{
		friend class LocalTransportConnection;
		
	private:
		std::string socket_file_name;

		bool transportopen;
		
	public:

#ifdef ROBOTRACONTEUR_WINDOWS
		typedef boost::asio::windows::stream_handle socket_type;
		typedef detail::windows::named_pipe_acceptor socket_acceptor_type;
#else
		typedef boost::asio::local::stream_protocol::socket socket_type;
		typedef boost::asio::local::stream_protocol::acceptor socket_acceptor_type;
#endif

		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> > TransportConnections;
		boost::mutex TransportConnections_lock;
		

		LocalTransport(RR_SHARED_PTR<RobotRaconteurNode> node=RobotRaconteurNode::sp());

		virtual ~LocalTransport();
		
		
	public:
		virtual bool IsServer() const;
		
		virtual bool IsClient() const;

								
		virtual std::string GetUrlSchemeString() const;

		virtual void SendMessage(RR_INTRUSIVE_PTR<Message> m);

		virtual void AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )>& callback);

		virtual void AsyncCreateTransportConnection(const std::string& url, RR_SHARED_PTR<Endpoint> e, boost::function<void (RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException> ) >& callback);

	 
		virtual RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(const std::string& url, RR_SHARED_PTR<Endpoint> e);

		virtual void CloseTransportConnection(RR_SHARED_PTR<Endpoint> e);

	protected:

		virtual void AsyncCreateTransportConnection2(RR_SHARED_PTR<LocalTransport::socket_type> socket , const std::string& noden, RR_SHARED_PTR<ITransportConnection> transport, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void (RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException> ) >& callback);

		virtual void CloseTransportConnection_timed(const boost::system::error_code& err, RR_SHARED_PTR<Endpoint> e, RR_SHARED_PTR<void> timer);
		
	public:

		virtual void StartClientAsNodeName(const std::string& name);

		virtual void StartServerAsNodeName(const std::string& name, bool public_ =false);

		virtual void StartServerAsNodeID(const NodeID& nodeid, bool public_ = false);
		
		virtual bool CanConnectService(const std::string& url);
		
		virtual void Close();
		
		virtual void CheckConnection(uint32_t endpoint);

		virtual void PeriodicCleanupTask();
						
		uint32_t TransportCapability(const std::string& name);
		
		virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m);

		virtual void AsyncGetDetectedNodes(const std::vector<std::string>& schemes, boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)>& handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		virtual bool GetDisableMessage3();
		virtual void SetDisableMessage3(bool d);

		virtual bool GetDisableStringTable();
		virtual void SetDisableStringTable(bool d);

		virtual bool GetDisableAsyncMessageIO();
		virtual void SetDisableAsyncMessageIO(bool d);

		virtual void EnableNodeDiscoveryListening();
		virtual void DisableNodeDiscoveryListening();

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

		virtual void LocalNodeServicesChanged();

	protected:

		RR_SHARED_PTR<socket_acceptor_type> acceptor;
		
		boost::mutex acceptor_lock;

		static void handle_accept(RR_SHARED_PTR<LocalTransport> parent,RR_SHARED_PTR<socket_acceptor_type> acceptor, RR_SHARED_PTR<socket_type> socket, const boost::system::error_code& error);
		
		virtual void register_transport(RR_SHARED_PTR<ITransportConnection> connection);
		virtual void erase_transport(RR_SHARED_PTR<ITransportConnection> connection);
		
		boost::mutex fds_lock;
		RR_SHARED_PTR<detail::LocalTransportFDs> fds;

		boost::mutex parameter_lock;		
		bool disable_message3;
		bool disable_string_table;
		bool disable_async_message_io;

		RR_SHARED_PTR<detail::LocalTransportDiscovery> discovery;
		boost::mutex discovery_lock;

		bool closed;
		boost::mutex closed_lock;
		boost::signals2::signal<void()> close_signal;
		
	};

	class ROBOTRACONTEUR_CORE_API NodeNameAlreadyInUse : public std::runtime_error
	{
	public:
		NodeNameAlreadyInUse() : runtime_error("Could not start server: NodeName already in use") {};
	};

	class ROBOTRACONTEUR_CORE_API NodeIDAlreadyInUse : public std::runtime_error
	{
	public:
		NodeIDAlreadyInUse() : runtime_error("Could not start server: NodeID already in use") {};
	};
}
