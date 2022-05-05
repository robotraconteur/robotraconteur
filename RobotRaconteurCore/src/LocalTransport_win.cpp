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

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "LocalTransport_win_private.h"
#include "LocalTransport_private.h"

#include <boost/locale.hpp>

#include <aclapi.h>

namespace RobotRaconteur
{

namespace detail
{
namespace LocalTransportUtil
{
BOOL GetNamedPipeServerProcessId(HANDLE pipe, PULONG pid, PBOOL available)
{
    bool valid = false;
    *available = 0;
    typedef BOOL(WINAPI * FPGetNamedPipeServerProcessId)(HANDLE, PULONG);
    FPGetNamedPipeServerProcessId get_named_pipe_server_process_id = NULL;

    const HMODULE lib = LoadLibraryW(L"kernel32.dll");
    if (lib != NULL)
    {
        get_named_pipe_server_process_id =
            reinterpret_cast<FPGetNamedPipeServerProcessId>(::GetProcAddress(lib, "GetNamedPipeServerProcessId"));

        if (get_named_pipe_server_process_id != NULL)
        {
            *available = 1;
            if (get_named_pipe_server_process_id(pipe, pid))
            {
                valid = true;
            }
        }
        FreeLibrary(lib);
    }
    return valid ? 1 : 0;
}

bool IsPipeSameUserOrService(HANDLE pipe, bool allow_service)
{
    bool valid = true;
    ULONG serverid;
    BOOL available;
    BOOL serverid_valid = GetNamedPipeServerProcessId(pipe, &serverid, &available);
    if (!available)
        return true;

    if (!serverid_valid)
        return false;

    try
    {
        std::wstring my_sid = GetSIDStringForProcessId(-1);
        std::wstring other_sid = GetSIDStringForProcessId(serverid);
        if (my_sid != other_sid && !((other_sid == L"S-1-5-19") && allow_service))
            valid = false;
    }
    catch (std::exception&)
    {
        valid = false;
    }

    return valid;
}

bool IsPipeUser(HANDLE pipe, std::string username)
{
    bool valid = true;
    ULONG serverid;
    BOOL available;
    BOOL serverid_valid = GetNamedPipeServerProcessId(pipe, &serverid, &available);
    if (!available)
        return true;

    if (!serverid_valid)
        return false;

    try
    {
        std::string pipe_username = GetUserNameForProcessId(serverid);
        if (pipe_username != username)
            valid = false;
    }
    catch (std::exception&)
    {
        valid = false;
    }

    return valid;
}

std::wstring GetSIDStringForProcessId(DWORD processid)
{

    HANDLE hToken = NULL;
    DWORD dwBufferSize = 0;
    boost::shared_array<uint8_t> pTokenUser;
    HANDLE hProc = NULL;

    if (processid == -1)
    {
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
        {
            throw SystemResourceException("Internal error");
        }
    }
    else
    {
        hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processid);
        if (hProc == INVALID_HANDLE_VALUE)
            throw SystemResourceException("Internal error");

        if (!OpenProcessToken(hProc, TOKEN_QUERY, &hToken))
        {
            CloseHandle(hProc);
            throw SystemResourceException("Internal error");
        }
        CloseHandle(hProc);
    }

    (void)GetTokenInformation(hToken, TokenUser, NULL, 0, &dwBufferSize);

    pTokenUser.reset(new uint8_t[dwBufferSize]);
    memset(pTokenUser.get(), 0, dwBufferSize);

    if (!GetTokenInformation(hToken, TokenUser, pTokenUser.get(), dwBufferSize, &dwBufferSize))
    {
        CloseHandle(hToken);
        throw SystemResourceException("Internal error");
    }

    CloseHandle(hToken);

    if (!IsValidSid(((PTOKEN_USER)pTokenUser.get())->User.Sid))
    {
        throw SystemResourceException("Internal error");
    }

    LPWSTR sid1;

    if (!ConvertSidToStringSidW(((PTOKEN_USER)pTokenUser.get())->User.Sid, &sid1))
    {
        throw SystemResourceException("Internal error");
    }

    std::wstring sid(sid1);
    LocalFree(sid1);
    return sid;
}

std::string GetUserNameForProcessId(DWORD processid)
{
    HANDLE hToken = NULL;
    DWORD dwBufferSize = 0;
    boost::shared_array<uint8_t> pTokenUser;
    HANDLE hProc = NULL;

    if (processid == -1)
    {
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
        {
            throw SystemResourceException("Internal error");
        }
    }
    else
    {
        hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processid);
        if (hProc == INVALID_HANDLE_VALUE)
            throw SystemResourceException("Internal error");

        if (!OpenProcessToken(hProc, TOKEN_QUERY, &hToken))
        {
            CloseHandle(hProc);
            throw SystemResourceException("Internal error");
        }
        CloseHandle(hProc);
    }

    (void)GetTokenInformation(hToken, TokenUser, NULL, 0, &dwBufferSize);

    pTokenUser.reset(new uint8_t[dwBufferSize]);
    memset(pTokenUser.get(), 0, dwBufferSize);

    if (!GetTokenInformation(hToken, TokenUser, pTokenUser.get(), dwBufferSize, &dwBufferSize))
    {
        CloseHandle(hToken);
        throw SystemResourceException("Internal error");
    }

    CloseHandle(hToken);

    if (!IsValidSid(((PTOKEN_USER)pTokenUser.get())->User.Sid))
    {
        throw SystemResourceException("Internal error");
    }

    wchar_t username1[512];
    DWORD cchName = sizeof(username1);
    SID_NAME_USE peUse;
    wchar_t lpReferencedDomainName[512];
    DWORD cchReferencedDomainName = sizeof(lpReferencedDomainName);
    if (!::LookupAccountSidW(NULL, ((PTOKEN_USER)pTokenUser.get())->User.Sid, username1, &cchName,
                             lpReferencedDomainName, &cchReferencedDomainName, &peUse))
    {
        throw InvalidOperationException("Internal error: username too long");
    }

    std::wstring username2(username1, cchName);

    std::string username = boost::locale::conv::utf_to_utf<char>(username2);

    return username;
}

boost::optional<std::wstring> GetSIDStringForName(std::wstring name)
{
    SID_NAME_USE use;
    DWORD sid_size = 0;
    DWORD zero = 0;
    if (!LookupAccountNameW(NULL, name.c_str(), NULL, &sid_size, NULL, &zero, &use))
    {
        return boost::optional<std::wstring>();
    }

    boost::shared_array<uint8_t> sid_buf(new uint8_t[sid_size]);
    zero = 0;

    if (!LookupAccountNameW(NULL, name.c_str(), (PSID)sid_buf.get(), &sid_size, NULL, &zero, &use))
    {
        return boost::optional<std::wstring>();
    }

    if (!IsValidSid((PSID)sid_buf.get()))
    {
        throw SystemResourceException("Internal error");
    }

    LPWSTR sid1;

    if (!ConvertSidToStringSidW((PSID)sid_buf.get(), &sid1))
    {
        throw SystemResourceException("Internal error");
    }

    std::wstring sid(sid1);
    LocalFree(sid1);
    return sid;
}

template <typename T>
void CloseAndDeleteStream(T* t)
{
    t->close();
    delete t;
}

RR_SHARED_PTR<std::ifstream> HandleToIStream(HANDLE h)
{
    if (h == INVALID_HANDLE_VALUE)
        return RR_SHARED_PTR<std::ifstream>();

    int file_descriptor = _open_osfhandle((intptr_t)h, _O_RDONLY);
    if (file_descriptor < 0)
    {
        CloseHandle(h);
        return RR_SHARED_PTR<std::ifstream>();
    }

    FILE* file = _fdopen(file_descriptor, "r");
    if (file == NULL)
    {
        _close(file_descriptor);
        return RR_SHARED_PTR<std::ifstream>();
    }
    std::ifstream* f = new std::ifstream(file);
    return RR_SHARED_PTR<std::ifstream>(f, &CloseAndDeleteStream<std::ifstream>);
}

RR_SHARED_PTR<std::ofstream> HandleToOStream(HANDLE h)
{
    if (h == INVALID_HANDLE_VALUE)
        return RR_SHARED_PTR<std::ofstream>();

    int file_descriptor = _open_osfhandle((intptr_t)h, 0);
    if (file_descriptor < 0)
    {
        CloseHandle(h);
        return RR_SHARED_PTR<std::ofstream>();
    }

    FILE* file = _fdopen(file_descriptor, "w");
    if (file == NULL)
    {
        _close(file_descriptor);
        return RR_SHARED_PTR<std::ofstream>();
    }

    std::ofstream* f = new std::ofstream(file);
    return RR_SHARED_PTR<std::ofstream>(f, &CloseAndDeleteStream<std::ofstream>);
}

RR_SHARED_PTR<std::fstream> HandleToStream(HANDLE h)
{
    if (h == INVALID_HANDLE_VALUE)
        return RR_SHARED_PTR<std::fstream>();

    int file_descriptor = _open_osfhandle((intptr_t)h, 0);
    if (file_descriptor < 0)
    {
        CloseHandle(h);
        return RR_SHARED_PTR<std::fstream>();
    }

    FILE* file = _fdopen(file_descriptor, "r+");
    if (file == NULL)
    {
        _close(file_descriptor);
        return RR_SHARED_PTR<std::fstream>();
    }

    std::fstream* f = new std::fstream(file);
    return RR_SHARED_PTR<std::fstream>(f, &CloseAndDeleteStream<std::fstream>);
}

bool SetWindowsSocketPermissions(const std::string& socket_fname, bool public_)
{
    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);

    std::wstring sid = RobotRaconteur::detail::LocalTransportUtil::GetSIDStringForProcessId(-1);
    std::wstring dacl_str = L"D:(D;OICI;FA;;;NU)(A;OICI;FRFWFX;;;BA)(A;OICI;FRFWFX;;;SY)(A;OICI;FRFWFX;;;" + sid + L")";

    if (public_)
    {
        boost::optional<std::wstring> rr_group1_sid =
            RobotRaconteur::detail::LocalTransportUtil::GetSIDStringForName(L"robotraconteur");
        if (rr_group1_sid)
            dacl_str += L"(A; OICI; FRFWFX;;; " + *rr_group1_sid + L")";
        boost::optional<std::wstring> rr_group2_sid =
            RobotRaconteur::detail::LocalTransportUtil::GetSIDStringForName(L"Robot Raconteur");
        if (rr_group2_sid)
            dacl_str += L"(A; OICI; FRFWFX;;; " + *rr_group2_sid + L")";
    }

    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(dacl_str.c_str(), SDDL_REVISION_1,
                                                              &(sa.lpSecurityDescriptor), NULL))

    {
        return false;
    }

    RR_SHARED_PTR<void> sd_sp(sa.lpSecurityDescriptor, &LocalFree);

    std::string socket_fname_c = socket_fname + "\0";

    DWORD ret = ::SetNamedSecurityInfoA(&socket_fname_c[0], SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL,
                                        (PACL)sa.lpSecurityDescriptor, NULL);
    if (ret != ERROR_SUCCESS)
    {
        return false;
    }
    return true;
}
} // namespace LocalTransportUtil

} // namespace detail

namespace detail
{
static RR_SHARED_PTR<void> WinLocalTransportDiscovery_find_first(const boost::filesystem::path& p)
{
    RR_SHARED_PTR<void> o(FindFirstChangeNotificationW(p.wstring().c_str(), TRUE,
                                                       FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
                                                           FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY),
                          &FindCloseChangeNotification);
    if (o.get() == INVALID_HANDLE_VALUE)
        o.reset();
    return o;
}

WinLocalTransportDiscovery::WinLocalTransportDiscovery(RR_SHARED_PTR<RobotRaconteurNode> node)
    : LocalTransportDiscovery(node)
{}

void WinLocalTransportDiscovery::Init()
{
    try
    {
        private_path = LocalTransportUtil::GetTransportPrivateSocketPath();
    }
    catch (std::exception&)
    {}
    public_path = LocalTransportUtil::GetTransportPublicSearchPath();

    shutdown_evt.reset(CreateEvent(NULL, TRUE, FALSE, NULL), &CloseHandle);
    if (shutdown_evt.get() == INVALID_HANDLE_VALUE)
        throw SystemResourceException("Internal error");

    boost::thread(boost::bind(&WinLocalTransportDiscovery::run, shared_from_this()));
}

void WinLocalTransportDiscovery::Shutdown() { SetEvent(shutdown_evt.get()); }

WinLocalTransportDiscovery::~WinLocalTransportDiscovery() {}

void WinLocalTransportDiscovery::run()
{
    while (true)
    {
        if (!private_evt)
        {
            try
            {
                private_path = LocalTransportUtil::GetTransportPrivateSocketPath();
                private_evt = WinLocalTransportDiscovery_find_first(*private_path);
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(
                    node, Transport, -1, "LocalTransport discovery watching private path \"" << *private_path << "\"")
            }
            catch (std::exception&)
            {}
        }

        if (!public_evt)
        {
            try
            {
                public_path = LocalTransportUtil::GetTransportPublicSearchPath();
                if (public_path)
                {
                    RR_SHARED_PTR<void> public_evt1 = WinLocalTransportDiscovery_find_first(*public_path);
                    if (public_evt1)
                    {
                        public_evt = public_evt1;
                    }
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                                       "LocalTransport discovery watching public path \""
                                                           << *private_path << "\"")
                }
            }
            catch (std::exception&)
            {}
        }

        std::vector<HANDLE> waiters;
        waiters.push_back(shutdown_evt.get());
        if (private_evt)
        {
            waiters.push_back(private_evt.get());
        }

        if (public_evt)
        {
            waiters.push_back(public_evt.get());
        }

        WaitForMultipleObjectsEx(waiters.size(), &waiters[0], FALSE, 55000, TRUE);

        // Wait for a bit to prevent excessive refreshing

        {
            DWORD shutdown_status = WaitForSingleObject(shutdown_evt.get(), 250);
            if (shutdown_status == WAIT_OBJECT_0 || shutdown_status == WAIT_FAILED)
            {
                return;
            }
        }

        // Do the node refresh!

        if (private_evt)
        {
            DWORD private_status;
            do
            {
                private_status = WaitForSingleObject(private_evt.get(), 50);
                if (private_status == WAIT_OBJECT_0)
                {
                    if (!FindNextChangeNotification(private_evt.get()))
                    {
                        private_evt.reset();
                        private_path.reset();
                        break;
                    }
                }
            } while (private_status == WAIT_OBJECT_0);

            if (private_status == WAIT_FAILED)
            {
                private_evt.reset();
                private_path.reset();
            }
        }

        if (public_evt)
        {
            DWORD public_status;
            do
            {
                public_status = WaitForSingleObject(public_evt.get(), 50);
                if (public_status == WAIT_OBJECT_0)
                {
                    if (!FindNextChangeNotification(public_evt.get()))
                    {
                        public_evt.reset();
                        public_path.reset();
                        break;
                    }
                }
            } while (public_status == WAIT_OBJECT_0);

            if (public_status == WAIT_FAILED)
            {
                public_evt.reset();
                public_path.reset();
            }
        }

        Refresh();
    }
}
} // namespace detail

} // namespace RobotRaconteur