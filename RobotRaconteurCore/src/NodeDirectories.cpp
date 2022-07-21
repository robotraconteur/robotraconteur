#include "RobotRaconteur/NodeDirectories.h"
#include "RobotRaconteur/Logging.h"
#include "NodeDirectories_private.h"

#ifdef ROBOTRACONTEUR_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shlobj.h>
#else
#include <pwd.h>
#endif

#include <boost/filesystem.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>


namespace RobotRaconteur
{

    namespace detail
    {
        static boost::filesystem::path replace_default_val_with_env(const boost::filesystem::path& default_val, const std::string& rr_env_var)
        {
            const char* env_val = std::getenv(rr_env_var.c_str());
            if (env_val)
            {
                return boost::filesystem::path(env_val);
                
            }
            return default_val;
        }

#ifndef ROBOTRACONTEUR_WINDOWS
        static boost::filesystem::path user_unix_home_dir(const std::string& default_rel_dir, const std::string& xdg_env, const std::string& rr_env_var)
        {
            const char* rr_env_var_val = std::getenv(rr_env_var.c_str());
            if (rr_env_var_val)
            {
                return boost::filesystem::path(rr_env_var_val);
            }

            const char* xdg_env_val = std::getenv(xdg_env.c_str());
            if (xdg_env_val)
            {
                return boost::filesystem::path(xdg_env_val) / "RobotRaconteur";
            }

            const char* home = std::getenv("HOME");
            if (!home)
            {
                throw SystemResourceException("Home directory not set");
            }

            return boost::filesystem::path(home) / default_rel_dir;
        }

        static boost::filesystem::path system_unix_dir(const std::string& default_dir, const std::string& rr_env_var)
        {
            const char* rr_env_var_val = std::getenv(rr_env_var.c_str());
            if (rr_env_var_val)
            {
                return boost::filesystem::path(rr_env_var_val);
            }

            return boost::filesystem::path(default_dir);
        }

        static boost::filesystem::path user_unix_run_dir(const std::string& xdg_env, const std::string& rr_env_var)
        {
            const char* rr_env_var_val = std::getenv(rr_env_var.c_str());
            if (rr_env_var_val)
            {
                return boost::filesystem::path(rr_env_var_val);
            }

            const char* xdg_env_val = std::getenv(xdg_env.c_str());
            if (xdg_env_val)
            {
                return boost::filesystem::path(xdg_env_val) / "robotraconteur";
            }

            uid_t u = getuid();
            std::string run_path = "/tmp/robotraconteur-run-" + boost::lexical_cast<std::string>(u);

            return boost::filesystem::path(run_path);
        }

        static boost::filesystem::path user_apple_run_dir(const std::string& rr_env_var)
        {
            const char* rr_env_var_val = std::getenv(rr_env_var.c_str());
            if (rr_env_var_val)
            {
                return boost::filesystem::path(rr_env_var_val);
            }

            char* path1 = std::getenv("TMPDIR");
            if (!path1)
                throw SystemResourceException("Could not activate system for local transport");

            boost::filesystem::path path = path1;
            path = path.remove_trailing_separator().parent_path();
            path /= "C";
            if (!boost::filesystem::is_directory(path))
                throw SystemResourceException("Could not activate system for local transport");
            path /= "robotraconteur";
            return path;
        }
#endif

#ifdef ROBOTRACONTEUR_WINDOWS
        static boost::filesystem::path get_user_localappdata()
        {
            boost::scoped_array<char> sysdata_path1(new char[MAX_PATH]);
            if (FAILED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, sysdata_path1.get())))
            {
                throw SystemResourceException("Could not get system information");
            }

            boost::filesystem::path sysdata_path(sysdata_path1.get());
            return sysdata_path;
        }

        static boost::filesystem::path get_common_appdata()
        {
            boost::scoped_array<wchar_t> sysdata_path1(new wchar_t[MAX_PATH]);
            if (FAILED(SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, sysdata_path1.get())))
            {
                throw SystemResourceException("Could not get system information");
            }

            boost::filesystem::path sysdata_path(sysdata_path1.get());
            return sysdata_path;
        }
#endif

    static bool is_sub_dir(boost::filesystem::path p, boost::filesystem::path root) 
    {
        while(p != boost::filesystem::path()) {
            if(p == root) {
                return true;
            }
            p = p.parent_path();
        }
        return false;
    }

    }

    #define CATCH_DIR_RESOLVE(cmd, dir_type) \
        try { \
            cmd; \
        } catch (std::exception& e) { \
            if (node)   { \
            ROBOTRACONTEUR_LOG_WARNING_COMPONENT(node, Node, -1, "Error resolving " << dir_type << ":" << e.what()); \
        }}

    NodeDirectories GetDefaultNodeDirectories(RR_SHARED_PTR<RobotRaconteurNode> node)
    {
        NodeDirectories ret;
#ifdef ROBOTRACONTEUR_WINDOWS
        // Windows implementation
        boost::filesystem::path user_localappdata = detail::get_user_localappdata();
        boost::filesystem::path common_appdata = detail::get_common_appdata();

        CATCH_DIR_RESOLVE(ret.system_data_dir = detail::replace_default_val_with_env(common_appdata / "RobotRaconteur" / "data", "ROBOTRACONTEUR_SYSTEM_DATA_DIR"),"system_data_dir");
        CATCH_DIR_RESOLVE(ret.system_config_dir = detail::replace_default_val_with_env(common_appdata / "RobotRaconteur", "ROBOTRACONTEUR_SYSTEM_CONFIG_DIR"),"system_config_dir");
        CATCH_DIR_RESOLVE(ret.system_state_dir = detail::replace_default_val_with_env(common_appdata / "RobotRaconteur" / "state", "ROBOTRACONTEUR_SYSTEM_STATE_DIR"), "system_state_dir");
        CATCH_DIR_RESOLVE(ret.system_cache_dir = detail::replace_default_val_with_env(common_appdata / "RobotRaconteur" / "cache", "ROBOTRACONTEUR_SYSTEM_CACHE_DIR"),"system_cache_dir");
        CATCH_DIR_RESOLVE(ret.system_run_dir = detail::replace_default_val_with_env(common_appdata / "RobotRaconteur" / "run", "ROBOTRACONTEUR_SYSTEM_RUN_DIR"), "system_run_dir");
#else
        CATCH_DIR_RESOLVE(ret.system_data_dir = detail::system_unix_dir("/usr/local/share/robotraconteur", "ROBOTRACONTEUR_SYSTEM_DATA_DIR"),"system_data_dir");
        CATCH_DIR_RESOLVE(ret.system_config_dir = detail::system_unix_dir("/etc/robotraconteur", "ROBOTRACONTEUR_SYSTEM_CONFIG_DIR"),"system_config_dir");
        CATCH_DIR_RESOLVE(ret.system_state_dir = detail::system_unix_dir("/var/lib/robotraconteur", "ROBOTRACONTEUR_SYSTEM_STATE_DIR"),"system_state_dir");
        CATCH_DIR_RESOLVE(ret.system_cache_dir = detail::system_unix_dir("/var/cache/robotraconteur", "ROBOTRACONTEUR_SYSTEM_CACHE_DIR"),"system_cache_dir");
        CATCH_DIR_RESOLVE(ret.system_run_dir = detail::system_unix_dir("/var/run/robotraconteur", "ROBOTRACONTEUR_SYSTEM_RUN_DIR"),"system_run_dir"); 
#endif

        if (IsLogonUserRoot())
        {
            ret.user_data_dir = ret.system_data_dir;
            ret.user_config_dir = ret.system_config_dir;
            ret.user_state_dir = ret.system_state_dir;
            ret.user_cache_dir = ret.system_cache_dir;
            ret.user_run_dir = ret.system_run_dir / "root";
        }
        else
        {
#ifdef ROBOTRACONTEUR_WINDOWS

            CATCH_DIR_RESOLVE(ret.user_data_dir = detail::replace_default_val_with_env(user_localappdata / "RobotRaconteur" / "data", "ROBOTRACONTEUR_USER_DATA_DIR"),"user_data_dir");
            CATCH_DIR_RESOLVE(ret.user_config_dir = detail::replace_default_val_with_env(user_localappdata / "RobotRaconteur", "ROBOTRACONTEUR_USER_CONFIG_DIR"),"user_config_dir");
            CATCH_DIR_RESOLVE(ret.user_state_dir = detail::replace_default_val_with_env(user_localappdata / "RobotRaconteur" / "state", "ROBOTRACONTEUR_USER_STATE_DIR"),"user_state_dir");
            CATCH_DIR_RESOLVE(ret.user_cache_dir = detail::replace_default_val_with_env(user_localappdata / "RobotRaconteur" / "cache", "ROBOTRACONTEUR_USER_CACHE_DIR"),"user_cache_dir");
            CATCH_DIR_RESOLVE(ret.user_run_dir = detail::replace_default_val_with_env(user_localappdata / "RobotRaconteur" / "run", "ROBOTRACONTEUR_USER_RUN_DIR"),"user_run_dir");

#else
            CATCH_DIR_RESOLVE(ret.user_data_dir = detail::user_unix_home_dir(".local/share/RobotRaconteur", "XDG_DATA_HOME", "ROBOTRACONTEUR_USER_DATA_DIR"),"user_data_dir");
            CATCH_DIR_RESOLVE(ret.user_config_dir = detail::user_unix_home_dir(".config/RobotRaconteur", "XDG_CONFIG_HOME", "ROBOTRACONTEUR_USER_CONFIG_DIR"),"user_config_dir");
            CATCH_DIR_RESOLVE(ret.user_state_dir = detail::user_unix_home_dir(".local/state/RobotRaconteur", "XDG_STATE_HOME", "ROBOTRACONTEUR_USER_CONFIG_DIR"),"user_state_dir");
            CATCH_DIR_RESOLVE(ret.user_cache_dir = detail::user_unix_home_dir(".cache/RobotRaconteur", "XDG_CACHE_HOME", "ROBOTRACONTEUR_USER_CACHE_DIR"),"user_cache_dir");

#ifdef ROBOTRACONTEUR_APPLE
            CATCH_DIR_RESOLVE(ret.user_run_dir = detail::user_apple_run_dir("ROBOTRACONTEUR_USER_RUN_DIR"),"user_run_dir");
#else
            CATCH_DIR_RESOLVE(ret.user_run_dir = detail::user_unix_run_dir("XDG_RUNTIME_DIR", "ROBOTRACONTEUR_USER_RUN_DIR"),"user_run_dir");
#endif

#endif
        }

        return ret;
    }

    std::string GetLogonUserName()
    {
    #ifdef ROBOTRACONTEUR_WINDOWS
        DWORD size = 0;
        GetUserName(NULL, &size);

        boost::scoped_array<char> n(new char[size]);

        BOOL res = GetUserName(n.get(), &size);
        if (!res)
            throw SystemResourceException("Could not determine current user");

        return std::string(n.get());
    #else
        struct passwd* passwd = NULL;
        passwd = getpwuid(getuid());
        if (passwd == NULL)
            throw ConnectionException("Could not determine current user");

        std::string o(passwd->pw_name);
        if (o.empty())
            throw ConnectionException("Could not determine current user");

        return o;

    #endif
    }


    #ifdef ROBOTRACONTEUR_WINDOWS

    bool IsLogonUserRoot_win_compare_rid(PTOKEN_USER pTokenUser, DWORD rid)
    {
        SID_IDENTIFIER_AUTHORITY siaNT = SECURITY_NT_AUTHORITY;
        PSID pSystemSid;
        if (!AllocateAndInitializeSid(&siaNT, 1, rid,
            0, 0, 0, 0, 0, 0, 0, &pSystemSid))
            return FALSE;

        // compare the user SID from the token with the LocalSystem SID
        BOOL res = EqualSid(pTokenUser->User.Sid, pSystemSid);
        FreeSid(pSystemSid);

        return res != 0;
    }

    bool IsLogonUserRoot_win()
    {
        HANDLE hToken;
        UCHAR bTokenUser[sizeof(TOKEN_USER) + 8 + 4 * SID_MAX_SUB_AUTHORITIES];
        PTOKEN_USER pTokenUser = (PTOKEN_USER)bTokenUser;
        ULONG cbTokenUser;

        // open process token
        if (!OpenProcessToken(GetCurrentProcess(),
            TOKEN_QUERY,
            &hToken))
            return false;

        // retrieve user SID
        if (!GetTokenInformation(hToken, TokenUser, pTokenUser,
            sizeof(bTokenUser), &cbTokenUser))
        {
            CloseHandle(hToken);
            return false;
        }

        CloseHandle(hToken);

        if (IsLogonUserRoot_win_compare_rid(pTokenUser, SECURITY_LOCAL_SYSTEM_RID))
        {
            return true;
        }

        if (IsLogonUserRoot_win_compare_rid(pTokenUser, SECURITY_LOCAL_SERVICE_RID))
        {
            return true;
        }

        if (IsLogonUserRoot_win_compare_rid(pTokenUser, SECURITY_NETWORK_SERVICE_RID))
        {
            return true;
        }
        return false;
    }
    #endif

    bool IsLogonUserRoot()
    {
    #ifndef ROBOTRACONTEUR_WINDOWS
        uid_t user_uid = getuid();
        return user_uid == 0;
    #else
        return IsLogonUserRoot_win();
    #endif

    }

    void CreateUserNodeDirectory(const boost::filesystem::path& dir)
    {
        boost::filesystem::create_directories(dir);
    }

    void CreateSystemPrivateNodeDirectory(const boost::filesystem::path& dir)
    {
        // TODO: file permissions
        boost::filesystem::create_directories(dir);
    }

    void CreateSystemPublicNodeDirectory(const boost::filesystem::path& dir)
    {
        // TODO: file permissions
        boost::filesystem::create_directories(dir);
    }

    void CreateUserRunDirectory(const NodeDirectories& node_dirs)
    {
        if (node_dirs.user_run_dir == node_dirs.system_run_dir)
        {
            // TODO: Create directory accessible by RobotRaconteur group
            boost::filesystem::create_directories(node_dirs.user_run_dir);
        }
        else if (detail::is_sub_dir(node_dirs.user_run_dir, node_dirs.system_run_dir))
        {
            // TODO: Create system_run_dir accessible by RobotRaconteur group, and private remaining
            boost::filesystem::create_directories(node_dirs.user_run_dir);
        }
        else
        {
            boost::filesystem::create_directories(node_dirs.user_run_dir);
        }
    }

    NodeDirectoriesFD::NodeDirectoriesFD()
{
#ifdef ROBOTRACONTEUR_WINDOWS
    fd = NULL;
#else
    fd = -1;
#endif
}

NodeDirectoriesFD::~NodeDirectoriesFD()
{
#ifdef ROBOTRACONTEUR_WINDOWS
    CloseHandle(fd);
#else
    close(fd);
#endif
}

void NodeDirectoriesFD::open_read(const boost::filesystem::path& path, boost::system::error_code& err)
{
#ifdef BOOST_WINDOWS
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE)
    {
        err = boost::system::error_code(GetLastError(), boost::system::system_category());
        return;
    }

    fd = h;
#else
    int fd1 = open(path.c_str(), O_CLOEXEC | O_RDONLY);
    if (fd1 < 0)
    {
        err = boost::system::error_code(errno, boost::system::system_category());
        return;
    }
    fd = fd1;
#endif
}
void NodeDirectoriesFD::open_lock_write(const boost::filesystem::path& path, bool delete_on_close,
                                       boost::system::error_code& err)
{
    RR_UNUSED(delete_on_close);
#ifdef BOOST_WINDOWS
    DWORD flags = FILE_ATTRIBUTE_NORMAL;
    if (delete_on_close)
    {
        flags |= FILE_FLAG_DELETE_ON_CLOSE;
    }
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, flags, NULL);
    if (h == INVALID_HANDLE_VALUE)
    {
        err = boost::system::error_code(GetLastError(), boost::system::system_category());
        return;
    }

    fd = h;
#else
    int fd1 = open(path.c_str(), O_CLOEXEC | O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (fd1 < 0)
    {
        err = boost::system::error_code(errno, boost::system::system_category());
        return;
    }

    struct ::flock lock = {};
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (::fcntl(fd1, F_SETLK, &lock) < 0)
    {
        close(fd1);
        err = boost::system::error_code(boost::system::errc::no_lock_available, boost::system::system_category());
        return;
    }

    fd = fd1;
#endif
}

/*void NodeDirectoriesFD::reopen_lock_write(bool delete_on_close, boost::system::error_code& err)
{
    DWORD flags = FILE_ATTRIBUTE_NORMAL;
    if (delete_on_close)
    {
        flags |= FILE_FLAG_DELETE_ON_CLOSE;
    }
    HANDLE h = ReOpenFile(fd, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, flags);
    if (h == INVALID_HANDLE_VALUE)
    {
        err = boost::system::error_code(GetLastError(), boost::system::system_category());
        return;
    }

    fd = h;
}*/

bool NodeDirectoriesFD::read(std::string& data) // NOLINT(readability-make-member-function-const)
{
#ifdef ROBOTRACONTEUR_WINDOWS
    if (::SetFilePointer(fd, 0, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return false;
    DWORD len = GetFileSize(fd, NULL);
    if (len == INVALID_FILE_SIZE)
        return false;
    if (len > 16 * 1024)
        return false;
    std::string ret;
    ret.resize(len);
    DWORD bytes_read;
    if (!::ReadFile(fd, &ret[0], len, &bytes_read, NULL))
    {
        return false;
    }

    if (bytes_read != len)
        return false;
    data = ret;
    return true;
#else
    if (lseek(fd, 0, SEEK_END) < 0)
        return false;
    off_t len = lseek(fd, 0, SEEK_CUR);
    if (len < 0)
        return false;
    if (lseek(fd, 0, SEEK_SET) < 0)
        return false;

    std::string ret;
    ret.resize(len);

    ssize_t retval = ::read(fd, &ret[0], len);
    if (retval < 0)
    {
        return false;
    }

    if (retval != len)
        return false;
    data = ret;
    return true;
#endif
}

bool NodeDirectoriesFD::read_info()
{
    std::string in;
    if (!read(in))
        return false;

    std::vector<std::string> lines;
    boost::split(lines, in, boost::is_any_of("\n"), boost::algorithm::token_compress_on);
    info.clear();
    BOOST_FOREACH (std::string& l, lines)
    {
        boost::regex r("^\\s*([\\w+\\.\\-]+)\\s*\\:\\s*(.*)\\s*$");
        boost::smatch r_match;
        if (!boost::regex_match(l, r_match, r))
        {
            continue;
        }
        info.insert(std::make_pair(boost::trim_copy(r_match[1].str()), boost::trim_copy(r_match[2].str())));
    }
    return true;
}

bool NodeDirectoriesFD::write(boost::string_ref data) // NOLINT(readability-make-member-function-const)
{
#ifdef ROBOTRACONTEUR_WINDOWS
    DWORD bytes_written = 0;
    if (!::WriteFile(fd, &data[0], data.size(), &bytes_written, NULL))
        return false;
    if (bytes_written != data.size())
        return false;
    if (!FlushFileBuffers(fd))
        return false;
#else
    ssize_t ret = ::write(fd, &data[0], data.size());
    if (ret != data.size())
        return false;
    if (fsync(fd) < 0)
        return false;
#endif
    return true;
}

bool NodeDirectoriesFD::write_info()
{
    std::string data;
    for (std::map<std::string, std::string>::iterator e = info.begin(); e != info.end(); e++)
    {
        data += e->first + ": " + e->second + "\n";
    }

    return write(data);
}

bool NodeDirectoriesFD::reset() // NOLINT(readability-make-member-function-const)
{
#ifdef ROBOTRACONTEUR_WINDOWS
    if (::SetFilePointer(fd, 0, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return false;
    if (!::SetEndOfFile(fd))
        return false;
#else
    if (lseek(fd, 0, SEEK_SET) < 0)
        return false;
    if (ftruncate(fd, 0) < 0)
        return false;
#endif
    return true;
}

size_t NodeDirectoriesFD::file_len() // NOLINT(readability-make-member-function-const)
{
#ifdef ROBOTRACONTEUR_WINDOWS
    return ::GetFileSize(fd, NULL);
#else
    off_t init_pos = lseek(fd, 0, SEEK_CUR);
    if (init_pos < 0)
        return -1;
    if (lseek(fd, 0, SEEK_END) < 0)
        return -1;
    off_t len = lseek(fd, 0, SEEK_CUR);
    if (len < 0)
        return -1;
    if (lseek(fd, init_pos, SEEK_SET) < 0)
        return -1;
    return len;
#endif
}

}