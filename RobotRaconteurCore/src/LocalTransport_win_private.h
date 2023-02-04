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

#include "RobotRaconteur/LocalTransport.h"
#include "RobotRaconteur/ASIOStreamBaseTransport.h"
#include "LocalTransport_discovery_private.h"

#include <boost/filesystem.hpp>

#include <boost/system/windows_error.hpp>
#include <Sddl.h>
#include <io.h>
#include <FCNTL.h>

#pragma once

// Workaround for missing definition in MinGW.
#ifdef __MINGW32__

BOOL WINAPI ConvertStringSecurityDescriptorToSecurityDescriptorA(_In_ LPCSTR StringSecurityDescriptor,
                                                                 _In_ DWORD StringSDRevision,
                                                                 _Outptr_ PSECURITY_DESCRIPTOR* SecurityDescriptor,
                                                                 _Out_opt_ PULONG SecurityDescriptorSize);

BOOL WINAPI ConvertStringSecurityDescriptorToSecurityDescriptorW(_In_ LPCWSTR StringSecurityDescriptor,
                                                                 _In_ DWORD StringSDRevision,
                                                                 _Outptr_ PSECURITY_DESCRIPTOR* SecurityDescriptor,
                                                                 _Out_opt_ PULONG SecurityDescriptorSize);

#ifdef UNICODE
#define ConvertStringSecurityDescriptorToSecurityDescriptor ConvertStringSecurityDescriptorToSecurityDescriptorW
#else
#define ConvertStringSecurityDescriptorToSecurityDescriptor ConvertStringSecurityDescriptorToSecurityDescriptorA
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
bool IsPipeUser(HANDLE pipe, const std::string& username);

RR_SHARED_PTR<std::ifstream> HandleToIStream(HANDLE h);
RR_SHARED_PTR<std::ofstream> HandleToOStream(HANDLE h);
RR_SHARED_PTR<std::fstream> HandleToStream(HANDLE h);

bool SetWindowsSocketPermissions(const std::string& socket_fname, bool public_);
} // namespace LocalTransportUtil

} // namespace detail

namespace detail
{
class WinLocalTransportDiscovery : public LocalTransportDiscovery,
                                   public RR_ENABLE_SHARED_FROM_THIS<WinLocalTransportDiscovery>
{
  public:
    WinLocalTransportDiscovery(const RR_SHARED_PTR<RobotRaconteurNode>& node);
    virtual void Init();
    virtual void Shutdown();
    virtual ~WinLocalTransportDiscovery();

    void run();

  protected:
    RR_SHARED_PTR<void> shutdown_evt;
    RR_SHARED_PTR<void> private_evt;
    RR_SHARED_PTR<void> public_evt;
    NodeDirectories node_dirs;
};
} // namespace detail
} // namespace RobotRaconteur