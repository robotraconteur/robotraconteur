/**
 * @file NodeDirectories.h
 *
 * @author John Wason, PhD
 *
 * @copyright Copyright 2011-2022 Wason Technology, LLC
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * @par
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "RobotRaconteur/RobotRaconteurConstants.h"
#include "RobotRaconteur/NodeID.h"

#include <boost/filesystem/path.hpp>

namespace RobotRaconteur
{
/**
 * @brief Directories on local system used by the node
 *
 * The node uses local directories to load configuration information,
 * cache data, communicate with other processes, etc. These directories
 * can be configured using the NodeDirectories structure and
 * RobotRaconteurNode::GetNodeDirectories() and RobotRaconteurNode::SetNodeDirectories()
 * Use GetDefaultNodeDirectories() to retrieve the default directories.
 *
 * Note: for root user, system and user directories are identical
 */
class ROBOTRACONTEUR_CORE_API NodeDirectories
{
  public:
    /**
     * @brief Robot Raconteur System data directory
     *
     * Default value Unix : /usr/local/share/robotraconteur
     * Default value Windows: %PROGRAMDATA%\RobotRaconteur\data
     * Environmental variable override: ROBOTRACONTEUR_SYSTEM_DATA_DIR
     */
    boost::filesystem::path system_data_dir;

    /**
     * @brief Robot Raconteur System config directory
     *
     * Default value Unix: /etc/robotraconteur
     * Default value Windows: %PROGRAMDATA%\RobotRaconteur\
     * Environmental variable override: ROBOTRACONTEUR_SYSTEM_CONFIG_DIR
     */
    boost::filesystem::path system_config_dir;

    /**
     * @brief Robot Raconteur System config directory
     *
     * Default value Unix: /var/lib/robotraconteur
     * Default value Windows: %PROGRAMDATA%\RobotRaconteur\state
     * Environmental variable override: ROBOTRACONTEUR_SYSTEM_STATE_DIR
     */
    boost::filesystem::path system_state_dir;

    /**
     * @brief Robot Raconteur System cache directory
     *
     * Default value Unix: /var/cache/robotraconteur
     * Default value Windows: %PROGRAMDATA%\RobotRaconteur\cache
     * Environmental variable override: ROBOTRACONTEUR_SYSTEM_CACHE_DIR
     */
    boost::filesystem::path system_cache_dir;

    /**
     * @brief Robot Raconteur System run directory
     *
     * Default value: /var/run/robotraconteur
     * Default value Windows: %PROGRAMDATA%\RobotRaconteur\run
     * Environmental variable override: ROBOTRACONTEUR_SYSTEM_RUN_DIR
     */
    boost::filesystem::path system_run_dir;

    /**
     * @brief Robot Raconteur User data directory
     *
     * Default value Unix: $HOME/.local/share/RobotRaconteur or $XDG_DATA_HOME/RobotRaconteur
     * Default value Windows: %LOCALAPPDATA%\RobotRaconteur\data
     * Environmental variable override: ROBOTRACONTEUR_USER_DATA_DIR
     */
    boost::filesystem::path user_data_dir;

    /**
     * @brief Robot Raconteur User config directory
     *
     * Default value Unix: $HOME/.config/RobotRaconteur or $XDG_CONFIG_HOME/RobotRaconteur
     * Default value Windows: %LOCALAPPDATA%\RobotRaconteur\
     * Environmental variable override: ROBOTRACONTEUR_USER_CONFIG_DIR
     */
    boost::filesystem::path user_config_dir;

    /**
     * @brief Robot Raconteur User state directory
     *
     * Default value Unix: $HOME/.local/state/RobotRaconteur or $XDG_STATE_HOME/RobotRaconteur
     * Default value Windows: %LOCALAPPDATA%\RobotRaconteur\state
     * Environmental variable override: ROBOTRACONTEUR_USER_STATE_DIR
     */
    boost::filesystem::path user_state_dir;

    /**
     * @brief Robot Raconteur User cache directory
     *
     * Default value Unix: $HOME/.cache/RobotRaconteur or $XDG_CACHE_HOME/RobotRaconteur
     * Default value Windows: %LOCALAPPDATA%\RobotRaconteur\cache
     * Environmental variable override: ROBOTRACONTEUR_USER_CACHE_DIR
     */
    boost::filesystem::path user_cache_dir;

    /**
     * @brief Robot Raconteur User state directory
     *
     * Default value Unix: $XDG_RUNTIME_DIR/robotraconteur or /tmp/robotraconteur-run-$UID
     * Default value Windows: %LOCALAPPDATA%\RobotRaconteur\run
     * Default value for root: {system_run_dir}/root
     * Environmental variable override: ROBOTRACONTEUR_USER_RUN_DIR
     */
    boost::filesystem::path user_run_dir;
};

class NodeDirectoriesFD;

/**
 * @brief Utility functions for working with NodeDirectories
 *
 */
class ROBOTRACONTEUR_CORE_API NodeDirectoriesUtil
{
  public:
    /**
     * @brief Get the Default Node Directories object
     *
     * @param node The node to send logging information (optional)
     * @param create_user_dirs Create user directories if they don't exist (optional)
     * @return NodeDirectories The default node directories
     */
    static NodeDirectories GetDefaultNodeDirectories(
        const RR_SHARED_PTR<RobotRaconteurNode>& node = RR_SHARED_PTR<RobotRaconteurNode>());

    /**
     * @brief Get the logon name of the current user
     *
     * @return std::string
     */
    static std::string GetLogonUserName();

    /**
     * @brief Returns true if user is root
     *
     * @return true
     * @return false
     */
    static bool IsLogonUserRoot();

    /**
     * @brief Create directories with user permissions
     *
     * Parent directories are created, existing directories ignored
     *
     * @param dir The directories tree to create
     */
    // TODO
    // static void CreateUserNodeDirectory(const boost::filesystem::path& dir);

    /**
     * @brief Create directories with private system permissions
     *
     * Parent directories are created, existing directories ignored
     *
     * @param dir The directories tree to create
     */
    // TODO
    // static void CreateSystemPrivateNodeDirectory(const boost::filesystem::path& dir);

    /**
     * @brief Create directories with public system permissions
     *
     * Parent directories are created, existing directories ignored
     * Typically these directories will allow the "robotraconteur" group
     * ("RobotRaconteur" on Windows) access.
     *
     * @param dir The directories tree to create
     */
    // TODO
    // static void CreateSystemPublicNodeDirectory(const boost::filesystem::path& dir);

    /**
     * @brief Create a directory with user run permissions
     *
     * @param node_dirs The directories to create
     */
    // TODO
    // static void CreateUserRunDirectory(const NodeDirectories& node_dirs);

    /**
     * @brief Read an info file with key-value pairs
     *
     * @param fname The file to read
     * @param data (out) The map to fill with read values
     * @return true Success
     * @return false Failure
     */
    static bool ReadInfoFile(const boost::filesystem::path& fname, std::map<std::string, std::string>& data);

    /**
     * @brief Create a pid file
     *
     * Creates a file with the current program's pid. The file will also be locked for writing. The file
     * will be deleted and unlocked when handle is destroyed.
     *
     * @param path The pid file filename
     * @return RR_SHARED_PTR<NodeDirectoriesFD> Handle to pid file. Destroy to delete and release the file lock.
     */
    static RR_SHARED_PTR<NodeDirectoriesFD> CreatePidFile(const boost::filesystem::path& path);

    /**
     * @brief Create an info file with key-value pairs
     *
     * The file will be locked for writing. The file
     * will be deleted and unlocked when handle is destroyed.
     *
     * @param path The info filename
     * @param info The key-value pairs to write to the file.
     * @return RR_SHARED_PTR<NodeDirectoriesFD> Handle to pid file. Destroy to delete and release the file lock.
     */
    static RR_SHARED_PTR<NodeDirectoriesFD> CreateInfoFile(const boost::filesystem::path& path,
                                                           std::map<std::string, std::string> info);

    /**
     * @brief Update key-value pairs in a file created with CreateInfoFile()
     *
     * Replaces key-value pairs in a file opened with CreateInfoFile()
     *
     * @param h_info Handle to the opened info file
     * @param updated_info Key-value pairs to replace in the info file.
     */
    static void RefreshInfoFile(const RR_SHARED_PTR<NodeDirectoriesFD>& h_info,
                                const std::map<std::string, std::string>& updated_info);
};

/**
 * @brief Structure to hold result of GetUuidForNameAndLock
 *
 */
class ROBOTRACONTEUR_CORE_API GetUuidForNameAndLockResult
{
  public:
    /**
     * @brief The UUID assigned to the name in specified domain
     *
     * Note: NodeID data type used for convenience. This may or may not be a node id.
     */
    NodeID uuid;
    /**
     * @brief The identifier name
     *
     */
    std::string name;
    /**
     * @brief The scope of the identifier
     *
     */
    std::vector<std::string> scope;
    /**
     * @brief Lock for identifier. Release fd to release the lock.
     *
     */
    RR_SHARED_PTR<NodeDirectoriesFD> fd;
};

/**
 * @brief Get a saved UUID for a name with a specified scope
 *
 * Robot Raconteur uses "identifiers", which are a combination of a name and a UUID. For nodes, this is a
 * NodeName and NodeID pair. For convenience, the UUIDs can be automatically generated on individual machines,
 * and saved to the system. They can also be pre-specified by creating files in specific locations. The "scope"
 * is a path that separates different types of identifiers. For nodes, the scope is "nodeids". For devices,
 * it is something like ["identifiers","devices"].
 *
 * The base directory is typically "user_config_dir", except for root users on Unix systems. In those cases,
 * the search will initially look in "user_config_dir" (/etc/robotraconteur), but then fall back to "user_state_dir"
 * (/var/lib/robotraconteur) to save the generated UUID.
 *
 * The files contain the UUID in text form, and no other characters.
 *
 * Throws NodeDirectoriesResourceAlreadyInUse if the name is already in use by another node.
 *
 * @param node_dirs Node directory structure returned by RobotRaconteurNode::GetNodeDirectories()
 * @param name The name to get a UUID
 * @param scope The scope of the type of identifier
 * @return GetUuidForNameAndLockResult The resulting UUID
 */
ROBOTRACONTEUR_CORE_API GetUuidForNameAndLockResult GetUuidForNameAndLock(const NodeDirectories& node_dirs,
                                                                          boost::string_ref name,
                                                                          const std::vector<std::string>& scope);

/**
 * @brief Exception thrown by GetUuidForNameAndLockResult if the name with
 * the specified scope is currently in use.
 *
 */
class ROBOTRACONTEUR_CORE_API NodeDirectoriesResourceAlreadyInUse : public std::runtime_error
{
  public:
    NodeDirectoriesResourceAlreadyInUse() : runtime_error("Identifier UUID or Name already in use"){};
};

} // namespace RobotRaconteur