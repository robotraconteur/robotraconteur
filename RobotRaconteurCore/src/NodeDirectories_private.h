/**
 * @file NodeDirectories_private.h
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

#include "RobotRaconteur/NodeDirectories.h"

#ifdef ROBOTRACONTEUR_WINDOWS
#include <windef.h>
#endif

#pragma once

namespace RobotRaconteur
{
class NodeDirectoriesFD
{
  public:
    boost::mutex this_lock;

    std::map<std::string, std::string> info;

#ifdef ROBOTRACONTEUR_WINDOWS
    HANDLE fd;
#else
    int fd;
#endif

    NodeDirectoriesFD();

    ~NodeDirectoriesFD();

    void open_read(const boost::filesystem::path& path, boost::system::error_code& err);
    void open_lock_write(const boost::filesystem::path& path, bool delete_on_close, boost::system::error_code& err);
    // void reopen_lock_write(bool delete_on_close, boost::system::error_code& err);

    bool read(std::string& data);

    bool read_info();

    bool write(boost::string_ref data);

    bool write_info();

    bool reset();

    size_t file_len();
};

} // namespace RobotRaconteur
