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

#pragma once

#include <string>

#include <vector>
#include <RobotRaconteur.h>
#include <ostream>

using namespace std;
using namespace boost;
using namespace RobotRaconteur;

namespace RobotRaconteurGen
{
std::set<std::string> GenerateStringTable(std::vector<RR_SHARED_PTR<ServiceDefinition> > gen_defs,
                                          std::vector<RR_SHARED_PTR<ServiceDefinition> > all_defs);

std::set<std::string> GetServiceDefStrings(RR_SHARED_PTR<ServiceDefinition> def);

std::set<std::string> GetServiceEntryDefStrings(RR_SHARED_PTR<ServiceEntryDefinition> def, const std::string& def_name);

std::set<std::string> GetServiceEntriesDefStrings(std::vector<RR_SHARED_PTR<ServiceEntryDefinition> > def,
                                                  const std::string& def_name);

std::set<std::string> GetMemberStrings(RR_SHARED_PTR<MemberDefinition> m);

std::set<std::string> GetMembersStrings(std::vector<RR_SHARED_PTR<MemberDefinition> > m);
} // namespace RobotRaconteurGen