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

#include "RobotRaconteur/NodeID.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <iomanip>
#include <boost/regex.hpp>
#include "RobotRaconteur/DataTypes.h"
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>

using namespace std;
using namespace boost::algorithm;

namespace RobotRaconteur
{

NodeID::NodeID() { id = boost::uuids::nil_uuid(); }

NodeID::NodeID(boost::array<uint8_t, 16> id)
{
    if (id.size() != 16)
        throw InvalidArgumentException("Invalid NodeID");
    std::copy(id.begin(), id.end(), this->id.begin());
}

NodeID::NodeID(const NodeID& id)
{
    if (id.id.size() != 16)
        throw InvalidArgumentException("Invalid NodeID");
    this->id = id.id;
}

NodeID::NodeID(boost::string_ref id) { init_from_string(id); }

void NodeID::init_from_string(const boost::string_ref& id)
{
    if (id.find(":") != id.npos)
    {
        throw InvalidArgumentException("Old style node-ids are no longer supported as of Robot Raconteur version 0.5.  "
                                       "Please use UUID node-id format");
    }
    else
    {
        if (boost::trim_copy(to_range(id)) == boost::string_ref("{0}"))
        {
            this->id = boost::uuids::nil_uuid();
            return;
        }

        boost::match_results<boost::string_ref::const_iterator> what;
        boost::regex r(
            "\\{?([a-fA-F0-9]{8})-?([a-fA-F0-9]{4})-?([a-fA-F0-9]{4})-?([a-fA-F0-9]{4})-?([a-fA-F0-9]{12})\\}?");
        if (!boost::regex_match(id.begin(), id.end(), what, r))
            throw InvalidArgumentException("Invalid NodeID");
        size_t len = what.size();
        if (what.size() != 6)
            throw InvalidArgumentException("Invalid NodeID");
        std::string id2 = what[1] + what[2] + what[3] + what[4] + what[5];
        if (id2.size() != 32)
            throw InvalidArgumentException("Invalid NodeID");
        // boost::uuids::string_generator gen;
        // this->id=gen(id1);
        boost::uuids::uuid u1;
        for (size_t i = 0; i < 16; i++)
        {
            std::string id3 = id2.substr(i * 2, 2);
            std::stringstream ss;
            ss << std::hex << id3;
            uint32_t id4;
            ss >> id4;
            if (ss.fail() || !ss.eof())
            {
                throw InvalidArgumentException("Invalid NodeID");
            }
            u1.data[i] = static_cast<uint8_t>((id4 & 0xFF));
        }
        this->id = u1;
    }
}

NodeID::NodeID(boost::uuids::uuid id) { this->id = id; }

const boost::array<uint8_t, 16> NodeID::ToByteArray() const
{
    boost::array<uint8_t, 16> o;
    std::copy(id.begin(), id.end(), o.begin());
    return o;
}

const boost::uuids::uuid NodeID::ToUuid() const { return id; }

std::string NodeID::ToString() const { return "{" + boost::lexical_cast<std::string>(id) + "}"; }

std::string NodeID::ToString(boost::string_ref format) const
{
    if (format == "B")
    {
        return "{" + boost::lexical_cast<std::string>(id) + "}";
    }
    else if (format == "D")
    {
        return boost::lexical_cast<std::string>(id);
    }
    else if (format == "N")
    {
        return boost::replace_all_copy(boost::lexical_cast<std::string>(id), "-", "");
    }

    throw InvalidArgumentException("Invalid NodeID format");
}

NodeID NodeID::NewUniqueID()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    return NodeID(uuid);
}

bool NodeID::operator==(const NodeID& id2) const { return id == id2.id; }

bool NodeID::operator!=(const NodeID& id2) const { return id != id2.id; }

bool NodeID::IsAnyNode() const { return id.is_nil(); }

NodeID NodeID::GetAny() { return NodeID(); }

int32_t NodeID::GetHashCode() const
{
    int sum = 0;

    for (boost::uuids::uuid::const_iterator e = id.begin(); e != id.end(); e++)
    {
        sum += *e;
    }
    return sum;
}

bool NodeID::operator<(const NodeID& id2) const { return id < id2.id; }

NodeID& NodeID::operator=(const NodeID& id)
{
    if (id.id.size() != 16)
        throw InvalidArgumentException("Invalid NodeID");
    this->id = id.id;
    return *this;
}

size_t hash(const NodeID& id)
{
    boost::array<uint8_t, 16> b = id.ToByteArray();
    return *reinterpret_cast<size_t*>(b.data());
}

} // namespace RobotRaconteur
