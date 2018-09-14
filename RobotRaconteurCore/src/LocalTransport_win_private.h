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

#include "RobotRaconteur/LocalTransport.h"
#include "ASIOStreamBaseTransport.h"
#include "LocalTransport_discovery_private.h"

#include <boost/filesystem.hpp>

#include <boost/system/windows_error.hpp>
#include <Sddl.h>
#include <io.h>
#include <FCNTL.h>

#pragma once

//Workaround for missing definition in MinGW.
#ifdef __MINGW32__

BOOL
WINAPI
ConvertStringSecurityDescriptorToSecurityDescriptorA(
	_In_  LPCSTR StringSecurityDescriptor,
	_In_  DWORD StringSDRevision,
	_Outptr_ PSECURITY_DESCRIPTOR  *SecurityDescriptor,
	_Out_opt_ PULONG  SecurityDescriptorSize
);

BOOL
WINAPI
ConvertStringSecurityDescriptorToSecurityDescriptorW(
	_In_ LPCWSTR StringSecurityDescriptor,
	_In_ DWORD StringSDRevision,
	_Outptr_ PSECURITY_DESCRIPTOR * SecurityDescriptor,
	_Out_opt_ PULONG SecurityDescriptorSize
);

#ifdef UNICODE
#define ConvertStringSecurityDescriptorToSecurityDescriptor  ConvertStringSecurityDescriptorToSecurityDescriptorW
#else
#define ConvertStringSecurityDescriptorToSecurityDescriptor  ConvertStringSecurityDescriptorToSecurityDescriptorA
#endif // !UNICODE

#endif

namespace RobotRaconteur
{

	namespace detail
	{
		namespace LocalTransportUtil
		{
			std::wstring GetSIDStringForProcessId(DWORD process);
			std::string GetUserNameForProcessId(DWORD process);
			boost::optional<std::wstring> GetSIDStringForName(std::wstring name);

			bool IsPipeSameUserOrService(HANDLE pipe, bool allow_service);
			BOOL GetNamedPipeServerProcessId(HANDLE pipe, PULONG pid, PBOOL available);
			bool IsPipeUser(HANDLE pipe, std::string username);

			RR_SHARED_PTR<std::ifstream> HandleToIStream(HANDLE h);
			RR_SHARED_PTR<std::ofstream> HandleToOStream(HANDLE h);
			RR_SHARED_PTR<std::fstream> HandleToStream(HANDLE h);
		}

	}

	namespace detail
	{
		namespace windows
		{
			class named_pipe_acceptor : public RR_ENABLE_SHARED_FROM_THIS<named_pipe_acceptor>
			{
			protected:

				RR_WEAK_PTR<RobotRaconteurNode> node;

				std::string pipe_file_name;

				std::list<RR_SHARED_PTR<boost::asio::windows::stream_handle> > waiting_pipes;
				std::list<RR_SHARED_PTR<boost::asio::windows::stream_handle> > accepted_pipes;

				boost::mutex waiting_pipes_lock;
				bool accepting;

				int32_t waiting_pipe_count;

				std::list<boost::function<void(const boost::system::error_code&, RR_SHARED_PTR<boost::asio::windows::stream_handle>) > > waiting_acceptors;

				bool public_;

				RR_SHARED_PTR<boost::asio::windows::stream_handle> CreatePipe(std::string pipe_name, bool first, boost::system::error_code& ec)
				{
					SECURITY_ATTRIBUTES sa;
					ZeroMemory(&sa, sizeof(sa));
					sa.nLength = sizeof(sa);

					std::wstring sid = RobotRaconteur::detail::LocalTransportUtil::GetSIDStringForProcessId(-1);
					std::wstring dacl_str = L"D:(D;OICI;FA;;;NU)(A;OICI;FRFWFX;;;BA)(A;OICI;FRFWFX;;;SY)(A;OICI;FRFWFX;;;" + sid + L")";
					
					if (public_)
					{
						boost::optional<std::wstring> rr_group1_sid = RobotRaconteur::detail::LocalTransportUtil::GetSIDStringForName(L"robotraconteur");
						if (rr_group1_sid) dacl_str += L"(A; OICI; FRFWFX;;; " + *rr_group1_sid + L")";
						boost::optional<std::wstring> rr_group2_sid = RobotRaconteur::detail::LocalTransportUtil::GetSIDStringForName(L"Robot Raconteur");
						if (rr_group2_sid) dacl_str += L"(A; OICI; FRFWFX;;; " + *rr_group2_sid + L")";
					}
					
					if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(dacl_str.c_str(), SDDL_REVISION_1, &(sa.lpSecurityDescriptor), NULL))

					{
						ec = boost::system::error_code(::GetLastError(), boost::system::system_category());
						return RR_SHARED_PTR<boost::asio::windows::stream_handle>();
					}



					RR_SHARED_PTR<void> sd_sp(sa.lpSecurityDescriptor, &LocalFree);


					DWORD dwOpenMode = PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED;
					if (first)
					{
						dwOpenMode |= FILE_FLAG_FIRST_PIPE_INSTANCE;
					}

					HANDLE p = CreateNamedPipe(pipe_name.c_str(), dwOpenMode, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, PIPE_UNLIMITED_INSTANCES, 1024, 1024, 0, &sa);

					if (p == INVALID_HANDLE_VALUE)
					{
						ec = boost::system::error_code(::GetLastError(), boost::system::system_category());
						return RR_SHARED_PTR<boost::asio::windows::stream_handle>();
					}

					RR_SHARED_PTR<boost::asio::windows::stream_handle> sp = RR_MAKE_SHARED<boost::asio::windows::stream_handle>(boost::ref(GetNode()->GetThreadPool()->get_io_service()), p);

					return sp;
				}
			public:
				template<typename Handler>
				static void async_connect_named_pipe(RR_SHARED_PTR<boost::asio::windows::stream_handle> pipe, Handler handler)
				{
					boost::asio::io_service& io = pipe->get_io_service();

					boost::asio::detail::win_iocp_overlapped_ptr overlapped(io, handler);
					
					int result = ::ConnectNamedPipe(pipe->native_handle(), overlapped.get());
					DWORD last_error = ::WSAGetLastError();
					if (last_error == ERROR_PORT_UNREACHABLE)
						last_error = WSAECONNREFUSED;
					if (result != 0 && last_error != WSA_IO_PENDING)
					{
						boost::system::error_code ec(last_error, boost::asio::error::get_system_category());
						overlapped.complete(ec, 0);
					}
					else
					{
						overlapped.release();
					}
				}
			protected:
				static void end_connect_pipe(RR_WEAK_PTR<named_pipe_acceptor> tthis, RR_SHARED_PTR<boost::asio::windows::stream_handle> pipe, const boost::system::error_code& ec)
				{

					RR_SHARED_PTR<named_pipe_acceptor> tthis1 = tthis.lock();

					boost::mutex::scoped_lock lock(tthis1->waiting_pipes_lock);
					if (!tthis1->accepting) return;

					if (std::count(tthis1->waiting_pipes.begin(), tthis1->waiting_pipes.end(), pipe) != 0)
					{
						tthis1->waiting_pipes.remove(pipe);
					}

					boost::system::error_code ec1;
					tthis1->start_accepting(ec1);

					if (!tthis1->waiting_acceptors.empty())
					{
						boost::function<void(const boost::system::error_code&, RR_SHARED_PTR<boost::asio::windows::stream_handle>) > func = tthis1->waiting_acceptors.front();
						tthis1->waiting_acceptors.pop_front();
						lock.unlock();
						func(ec, pipe);
						return;
					}
					else
					{
						if (ec) return;
						tthis1->accepted_pipes.push_back(pipe);
						return;
					}
				}

				void start_accepting(boost::system::error_code& ec, bool first = false)
				{
					while ((int32_t)waiting_pipes.size() < waiting_pipe_count)

					{
						RR_SHARED_PTR<boost::asio::windows::stream_handle> h = CreatePipe(pipe_file_name, first, ec);
						if (ec) return;
						first = false;
						RR_WEAK_PTR<named_pipe_acceptor> tthis = shared_from_this();
						async_connect_named_pipe(h, boost::bind(&named_pipe_acceptor::end_connect_pipe, tthis, h, _1));
						waiting_pipes.push_back(h);
					}
				}



			public:

				named_pipe_acceptor(std::string pipe_file_name, int32_t waiting_pipe_count, bool public_, RR_SHARED_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp())
				{
					this->pipe_file_name = pipe_file_name;
					this->waiting_pipe_count = waiting_pipe_count;
					this->node = node;
					this->public_ = public_;
					accepting = false;
				}

				void listen(boost::system::error_code& ec)
				{
					boost::mutex::scoped_lock lock(waiting_pipes_lock);
					if (accepting) throw InvalidOperationException("Already listening");

					accepting = true;

					start_accepting(ec, true);

					if (ec)
					{
						accepting = false;
						waiting_pipes.clear();
					}

				}

				void listen()
				{
					boost::system::error_code ec;
					listen(ec);
					boost::asio::detail::throw_error(ec, "listen");
				}

				RR_SHARED_PTR<RobotRaconteurNode> GetNode()
				{
					RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
					if (!n) throw InvalidOperationException("Node has been released");
					return n;
				}

				void async_accept(boost::function<void(const boost::system::error_code&, RR_SHARED_PTR<boost::asio::windows::stream_handle>) > handler)
				{
					{
						boost::system::error_code ec;
						start_accepting(ec);
						if (ec)
						{
							RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, ec, RR_SHARED_PTR<boost::asio::windows::stream_handle>()), true);
							return;
						}

					}


					boost::mutex::scoped_lock lock(waiting_pipes_lock);

					if (!accepted_pipes.empty())
					{
						RR_SHARED_PTR<boost::asio::windows::stream_handle> h = accepted_pipes.front();
						accepted_pipes.pop_front();

						lock.unlock();

						boost::system::error_code ec;

						RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, ec, h));

						return;

					}

					waiting_acceptors.push_back(handler);

				}

				void close()
				{

					std::list<boost::function<void(const boost::system::error_code&, RR_SHARED_PTR<boost::asio::windows::stream_handle>) > > waiting_acceptors;

					{
						boost::mutex::scoped_lock lock(waiting_pipes_lock);
						accepting = false;

						for (std::list<RR_SHARED_PTR<boost::asio::windows::stream_handle> >::iterator e = waiting_pipes.begin(); e != waiting_pipes.end(); )
						{
							(*e)->close();
							e = waiting_pipes.erase(e);
						}

						waiting_acceptors.swap(this->waiting_acceptors);
					}

					while (!waiting_acceptors.empty())
					{
						try
						{
							boost::system::error_code ec = boost::system::windows::make_error_code(boost::system::windows::broken_pipe);
							waiting_acceptors.front()(ec, RR_SHARED_PTR<boost::asio::windows::stream_handle>());
						}
						catch (std::exception& e)
						{
							try
							{
								RobotRaconteurNode::TryHandleException(node, &e);
							}
							catch (std::exception&) {}
						}
						waiting_acceptors.pop_front();
					}
				}
			};
		}
	}

	namespace detail
	{
		class WinLocalTransportDiscovery : public LocalTransportDiscovery, public RR_ENABLE_SHARED_FROM_THIS<WinLocalTransportDiscovery>
		{
		public:
			WinLocalTransportDiscovery(RR_SHARED_PTR<RobotRaconteurNode> node);
			virtual void Init();
			virtual void Shutdown();
			virtual ~WinLocalTransportDiscovery();

			void run();

		protected:			
			RR_SHARED_PTR<void> shutdown_evt;
			RR_SHARED_PTR<void> private_evt;
			RR_SHARED_PTR<void> public_evt;		

		};
	}
}