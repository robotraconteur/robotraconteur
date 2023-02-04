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

//NodeDirectories

%shared_ptr(RobotRaconteur::NodeDirectoriesFD)

namespace RobotRaconteur
{
    class NodeDirectories
    {
    public:
        boost::filesystem::path system_data_dir;
        boost::filesystem::path system_config_dir;
        boost::filesystem::path system_state_dir;
        boost::filesystem::path system_cache_dir;
        boost::filesystem::path system_run_dir;
        boost::filesystem::path user_data_dir;
        boost::filesystem::path user_config_dir;
        boost::filesystem::path user_state_dir;
        boost::filesystem::path user_cache_dir;
        boost::filesystem::path user_run_dir;
    };

    %nodefaultctor NodeDirectoriesFD;
    class NodeDirectoriesFD {};

    class NodeDirectoriesUtil
    {
    public:
        static NodeDirectories GetDefaultNodeDirectories(boost::shared_ptr<RobotRaconteurNode> node = boost::shared_ptr<RobotRaconteurNode>());
        static std::string GetLogonUserName();
        static bool IsLogonUserRoot();
        //static void CreateUserNodeDirectory(const boost::filesystem::path& dir);
        // static void CreateSystemPrivateNodeDirectory(const boost::filesystem::path& dir);
        // static void CreateSystemPublicNodeDirectory(const boost::filesystem::path& dir);
        // static void CreateUserRunDirectory(const NodeDirectories& node_dirs);
        static bool ReadInfoFile(const boost::filesystem::path& fname, std::map<std::string, std::string>& data);
        static boost::shared_ptr<NodeDirectoriesFD> CreatePidFile(const boost::filesystem::path& path);
        static boost::shared_ptr<NodeDirectoriesFD> CreateInfoFile(const boost::filesystem::path& path, std::map<std::string, std::string> info);
        static void RefreshInfoFile(const boost::shared_ptr<NodeDirectoriesFD>& h_info, const std::map<std::string, std::string>& updated_info);
    };

    class GetUuidForNameAndLockResult
    {
    public:
        NodeID uuid;
        std::string name;
        std::vector<std::string> scope;
        boost::shared_ptr<NodeDirectoriesFD> fd;
    };

    GetUuidForNameAndLockResult GetUuidForNameAndLock(const NodeDirectories& node_dirs, const std::string& name, const std::vector<std::string>& scope);

    class NodeDirectoriesResourceAlreadyInUse : public std::runtime_error
    {
    public:
        NodeDirectoriesResourceAlreadyInUse();
    };

}