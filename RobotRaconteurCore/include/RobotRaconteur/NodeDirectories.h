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
     * @brief Get the Default Node Directories object
     * 
     * @param node The node to send logging information (optional)
     * @param create_user_dirs Create user directories if they don't exist (optional)
     * @return NodeDirectories The default node directories
     */
    ROBOTRACONTEUR_CORE_API NodeDirectories GetDefaultNodeDirectories(RR_SHARED_PTR<RobotRaconteurNode> node = RR_SHARED_PTR<RobotRaconteurNode>());

    ROBOTRACONTEUR_CORE_API std::string GetLogonUserName();

    ROBOTRACONTEUR_CORE_API bool IsLogonUserRoot();

    ROBOTRACONTEUR_CORE_API void CreateUserNodeDirectory(const boost::filesystem::path& dir);

    ROBOTRACONTEUR_CORE_API void CreateSystemPrivateNodeDirectory(const boost::filesystem::path& dir);

    ROBOTRACONTEUR_CORE_API void CreateSystemPublicNodeDirectory(const boost::filesystem::path& dir);

    ROBOTRACONTEUR_CORE_API void CreateUserRunDirectory(const NodeDirectories& node_dirs);


}