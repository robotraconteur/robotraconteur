/**
 * @file NodeID.h
 *
 * @author John Wason, PhD
 *
 * @copyright Copyright 2011-2020 Wason Technology, LLC
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
#include "RobotRaconteur/DataTypes.h"
#include <boost/array.hpp>
#include <boost/uuid/uuid.hpp>

namespace RobotRaconteur
{

/**
 * @brief NodeID UUID storage and generation
 *
 * Robot Raconteur uses NodeID and NodeName to uniquely identify a node.
 * NodeID is a UUID (Universally Unique ID), while NodeName is a string. The
 * NodeID is expected to be unique, while the NodeName is set by the user
 * and may not be unique. The NodeID class represents the UUID NodeID.
 *
 * A UUID is a 128-bit randomly generated number that is statistically guaranteed
 * to be unique to a very high probability. NodeID uses the Boost.UUID library
 * to generate, manage, and store the UUID.
 *
 * The UUID can be loaded from a string, bytes, or generated randomly at runtime.
 * It can be converted to a string.
 *
 * The LocalTransport and ServerNodeSetup classes will automatically assign
 * a NodeID to a node when the local transport is started with a specified node name.
 * The generated NodeID is stored on the local system, and is associated with the node name.
 * It will be loaded when a node is started with the same NodeName.
 *
 * NodeID with all zeros is considered "any" node.
 *
 */
class ROBOTRACONTEUR_CORE_API NodeID : virtual public RRObject
{
  private:
    boost::uuids::uuid id;

  public:
    /**
     * @brief Construct a new NodeID with "any" node UUID
     *
     */
    NodeID();

    /**
     * @brief Construct a new NodeID with the specified UUID bytes
     *
     * @param id The UUID bytes
     */
    NodeID(boost::array<uint8_t, 16> id);

    /**
     * @brief Construct a new NodeID with the specified boost::uuids::uuid
     *
     * @param id The UUID
     */
    NodeID(boost::uuids::uuid id);

    /**
     * @brief Copy construct a new NodeID
     *
     * @param id
     */
    NodeID(const NodeID& id);

    /**
     * @brief Construct a new NodeID parsing a string UUID
     *
     * @param id The UUID as a string
     */
    NodeID(boost::string_ref id);

    /**
     * @brief Convert the NodeID UUID to bytes
     *
     * @return const boost::array<uint8_t,16> The UUID as bytes
     */
    const boost::array<uint8_t, 16> ToByteArray() const;

    const boost::uuids::uuid ToUuid() const;

    /**
     * @brief Convert the NodeID UUID to string with "B" format
     *
     * Convert the UUID string to 8-4-4-4-12 "B" format (with brackets)
     *
     *     {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
     *
     * @return std::string
     */
    virtual std::string ToString() const;

    /**
     * @brief Convert the NodeID UUID to specified format
     *
     * Format may either be "B" for the 8-4-4-4-12 UUID with brackets,
     * "D" for the 8-4-4-4-12 UUID without brackets, or "N" for 32 UUID
     * format with no dashes or brackets
     *
     * "B" format:
     *
     *     {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
     *
     * "D" format:
     *
     *     xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
     *
     * "N" format:
     *
     *     xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
     *
     * @param format "B", "D", or "N" for the format
     * @return std::string
     */
    virtual std::string ToString(boost::string_ref format) const;

    /**
     * @brief Generate a new random NodeID UUID
     *
     * Returned UUID is statistically guaranteed to be unique
     *
     * @return NodeID The newly generated UUID
     */
    static NodeID NewUniqueID();

    /** @brief Test if NodeID is equal */
    bool operator==(const NodeID& id2) const;

    /** @brief Test if NodeID is not equal */
    bool operator!=(const NodeID& id2) const;

    /** @internal for std::map compatibility */
    bool operator<(const NodeID& id2) const;

    /** @brief Assignment operator */
    NodeID& operator=(const NodeID& id);

    /**
     * @brief Is the NodeID UUID all zeros
     *
     * The all zero UUID respresents "any" node, or an unset NodeID
     *
     * @return true The NodeID UUID is all zeros, representing any node
     * @return false The NodeID UUID is not all zeros
     */
    bool IsAnyNode() const;

    /**
     * @brief Get the "any" NodeId
     *
     * @return NodeID The "any" NodeID
     */
    static NodeID GetAny();

    /** @internal for std::unordered_map compatibility */
    virtual int32_t GetHashCode() const;

    virtual std::string RRType() { return "RobotRaconteur::NodeID"; }

  private:
    void init_from_string(const boost::string_ref& str);
};

size_t hash(const NodeID& nodeid);

} // namespace RobotRaconteur