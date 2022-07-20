#include "RobotRaconteur/NodeDirectories.h"
#include "RobotRaconteur/Logging.h"

#ifdef ROBOTRACONTEUR_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shlobj.h>
#else
#include <pwd.h>
#endif

#include <boost/filesystem.hpp>


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

    }

    #define CATCH_DIR_RESOLVE(cmd, dir_type) \
        try { \
            cmd; \
        } catch (std::exception& e) { \
            if (node)   { \
            ROBOTRACONTEUR_LOG_WARNING_COMPONENT(node, Node, -1, "Error resolving " << dir_type << ":" << e.what()); \
        }}

    NodeDirectories GetDefaultNodeDirectories(RR_SHARED_PTR<RobotRaconteurNode> node, bool create_user_dirs)
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
            ret.user_run_dir = ret.system_run_dir;
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

}