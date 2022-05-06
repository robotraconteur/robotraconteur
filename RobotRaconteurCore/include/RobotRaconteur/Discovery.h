/**
 * @file Discovery.h
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

#include "RobotRaconteur/DataTypes.h"

#pragma once

namespace RobotRaconteurServiceIndex
{
class ROBOTRACONTEUR_CORE_API ServiceInfo;

class ROBOTRACONTEUR_CORE_API NodeInfo;
} // namespace RobotRaconteurServiceIndex

namespace RobotRaconteur
{

/**
 * @brief Contains information about a service found using discovery
 *
 * ServiceInfo2 contains information about a service required to
 * connect to the service, metadata, and the service attributes
 *
 * ServiceInfo2 structures are returned by RobotRaconteurNode::FindServiceByType()
 * and ServiceInfo2Subscription
 *
 */
class ROBOTRACONTEUR_CORE_API ServiceInfo2
{
  public:
    /**
     * @brief Construct an empty ServiceInfo2
     *
     */
    ServiceInfo2();

    /** @brief The name of the service */
    std::string Name;
    /** @brief The fully qualified type of the root object in the service */
    std::string RootObjectType;
    /** @brief The fully qualified types the root object implements */
    std::vector<std::string> RootObjectImplements;
    /** @brief Candidate URLs to connect to the service */
    std::vector<std::string> ConnectionURL;
    /** @brief Service attributes */
    std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > Attributes;
    /** @brief The NodeID of the node that owns the service */
    RobotRaconteur::NodeID NodeID;
    /** @brief The NodeName of the node that owns the service */
    std::string NodeName;

    /**
     * @brief Construct a ServiceInfo2 from ServiceInfo and NodeInfo structures
     *
     */
    ServiceInfo2(const RobotRaconteurServiceIndex::ServiceInfo& info,
                 const RobotRaconteurServiceIndex::NodeInfo& ninfo);
};

/**
 * @brief Contains information about a node detected using discovery
 *
 * NodeInfo2 contains information about a node detected using discovery.
 * Node information is typically not verified, and is used as a first
 * step to detect available services.
 *
 * NodeInfo2 structures are returned by RobotRaconteurNode::FindNodeByName()
 * and RobotRaconteurNode::FindNodeByID()
 *
 */
class ROBOTRACONTEUR_CORE_API NodeInfo2
{
  public:
    /** @brief The NodeID of the detected node */
    RobotRaconteur::NodeID NodeID;
    /** @brief The NodeName of the detected node */
    std::string NodeName;
    /**
     * @brief Candidate URLs to connect to the node
     *
     * The URLs for the node typically contain the node transport endpoint
     * and the nodeid. A URL service parameter must be appended
     * to connect to a service.
     */
    std::vector<std::string> ConnectionURL;
};

/** @brief A candidate node connection URL and its timestamp */
class ROBOTRACONTEUR_CORE_API NodeDiscoveryInfoURL
{
  public:
    /** @brief Candidate node connection URL */
    std::string URL;
    /**
     * @brief Last time that this URL announce was received
     *
     * Candidate URLs typically expire after one minute. If all
     * candidate URLs expire, the node is considered lost.
     */
    boost::posix_time::ptime LastAnnounceTime;
};

/**
 * @brief Raw information used to announce and detect nodes
 *
 * For TCP/IP and QUIC/IP, this information is transmitted using
 * UDP multicast packets. For local transports, the filesystem is used.
 *
 * The data contained in NodeDiscoveryInfo is unverified and unfiltered.
 *
 * NodeDiscoveryInfo is used with RobotRaconteurNode::GetDetectedNodes(),
 * RobotRaconteurNode::AddNodeServicesDetectedListener(),
 * and RobotRaconteurNode::AddNodeDetectionLostListener()
 *
 */
class ROBOTRACONTEUR_CORE_API NodeDiscoveryInfo
{
  public:
    /** @brief The detected NodeID */
    RobotRaconteur::NodeID NodeID;
    /** @brief The detected NodeName */
    std::string NodeName;
    /** @brief Candidate URLs to connect to the node */
    std::vector<NodeDiscoveryInfoURL> URLs;
    /** @brief The current nonce for the node's services
     *
     * The ServiceStateNonce is a random string that represents the current
     * state of the nodes services. If the services change, the nonce will
     * change to a new random string, indicating that the client should
     * reinterrogate the node.
     *
     */
    std::string ServiceStateNonce;
};
} // namespace RobotRaconteur
