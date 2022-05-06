/**
 * @file BroadcastDownsampler.h
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

#include "RobotRaconteur/RobotRaconteurConfig.h"
#include <boost/unordered_map.hpp>

namespace RobotRaconteur
{
class ROBOTRACONTEUR_CORE_API ServerContext;
class ROBOTRACONTEUR_CORE_API PipeBroadcasterBase;
class ROBOTRACONTEUR_CORE_API WireBroadcasterBase;

/**
 * @brief Downsampler to manage rate of packets sent to client
 *
 * PipeBroadcaster and WireBroadcaster by default sends packets to all clients when
 * a pipe packet is sent or the wire value is changed. The updates typically happen
 * within a sensor or control loop, with the rate set by the specific device producing
 * the updates. Some clients may require less frequent data, and may run in to bandwidth
 * or processing issues if the data is sent at the full update rate. The BroadcastDownsampler
 * is used to implement broadcaster predicates that will drop packets.
 * Clients specify how many packets they want dropped between each packet sent. For instance,
 * a downsample of 0 means that no packets are dropped. A downsample of 1 will drop every other
 * packet. A downsample of two will drop 2 packets between sending 1 packet, etc. The
 * downsample level for each client is set using SetClientDownsample(). This should be
 * made available to the client using a property member.
 *
 * PipeBroadcaster and WireBroadcaster must be added to the downsampler
 * using AddPipeBroadcaster() and AddWireBroadcaster(), respectively.
 * It is recommended that these functions be called within
 * an IRRServiceObject::RRServiceObjectInit() override.
 *
 * BeginStep() and EndStep() must be called for each iteration of the
 * broadcasting loop. Use BroadcastDownsamplerStep for automatic
 * management in the loop.
 *
 * See com.robotraconteur.isoch.IsochDevice for the standard use
 * of downsampling.
 *
 */
class ROBOTRACONTEUR_CORE_API BroadcastDownsampler : public RR_ENABLE_SHARED_FROM_THIS<BroadcastDownsampler>
{
  public:
    /**
     * @brief Construct a new BroadcastDownsampler
     *
     * Must use boost::make_shared<BroadcastDownsampler>()
     *
     * Must call Init() after construction.
     *
     */
    BroadcastDownsampler();

    /**
     * @brief Initialize the BroadcastDownsampler
     *
     * @param context The ServerContext of the service
     * @param default_downsample The default downsample for clients
     */
    void Init(RR_SHARED_PTR<ServerContext> context, uint32_t default_downsample = 0);

    /**
     * @brief Get the downsample for the specified client
     *
     * @param ep The uint32_t endpoint ID of the client
     * @return uint32_t The downsample
     */
    uint32_t GetClientDownsample(uint32_t ep);

    /**
     * @brief Set the downsample for the specified client
     *
     * @param ep The uint32_t endpoint ID of the client
     * @param downsample The desired downsample
     */
    void SetClientDownsample(uint32_t ep, uint32_t downsample);

    /**
     * @brief Begin the update loop step
     *
     * Use BroadcastDownsamplerStep for automatic stepping
     *
     */
    void BeginStep();

    /**
     * @brief End the update loop step
     *
     * Use BroadcastDownsamplerStep for automatic stepping
     *
     */
    void EndStep();

    /**
     * @brief Add a PipeBroadcaster to the downsampler
     *
     * Sets the predicate of the broadcaster to this downsampler
     *
     * @param broadcaster The broadcaster to add
     */
    void AddPipeBroadcaster(RR_SHARED_PTR<PipeBroadcasterBase> broadcaster);

    /**
     * @brief Add a WireBroadcaster to the downsampler
     *
     * Sets the predicate of the broadcaster to this downsampler
     *
     * @param broadcaster The broadcaster to add
     */
    void AddWireBroadcaster(RR_SHARED_PTR<WireBroadcasterBase> broadcaster);

  protected:
    RR_WEAK_PTR<ServerContext> context;

    uint32_t default_downsample;
    uint64_t step_count;
    RR_UNORDERED_MAP<uint32_t, uint32_t> client_downsamples;

    boost::mutex this_lock;

    static bool wire_predicate(RR_WEAK_PTR<BroadcastDownsampler> this_, RR_SHARED_PTR<WireBroadcasterBase>& wire,
                               uint32_t ep);
    static bool pipe_predicate(RR_WEAK_PTR<BroadcastDownsampler> this_, RR_SHARED_PTR<PipeBroadcasterBase>& wire,
                               uint32_t ep, uint32_t index);

    static void server_event(RR_WEAK_PTR<BroadcastDownsampler> this_, RR_SHARED_PTR<ServerContext> ctx,
                             ServerServiceListenerEventType evt, RR_SHARED_PTR<void> p);
};

/**
 * @brief Class for automatic broadcast downsampler stepping
 *
 * Helper class to automate BroadcastDownsampler stepping.
 * Calls BroadcastDownsampler::BeginStep() on construction,
 * and BroadcastDownsampler::EndStep() on destruction.
 *
 */
class ROBOTRACONTEUR_CORE_API BroadcastDownsamplerStep
{
    RR_SHARED_PTR<BroadcastDownsampler>& _downsampler;

  public:
    /**
     * @brief Construct a BroadcastDownsampler
     *
     * Calls BroadcastDownsampler::BeginStep() on downsampler.
     * Calls BroadcastDownsampler::EndStep() on downsampler
     * when destroyed.
     *
     * @param downsampler The downsampler to step
     */
    BroadcastDownsamplerStep(RR_SHARED_PTR<BroadcastDownsampler>& downsampler);
    ~BroadcastDownsamplerStep();
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
/** @brief Convenience alias for BroadcastDownsampler shared_ptr */
using BroadcastDownsamplerPtr = RR_SHARED_PTR<BroadcastDownsampler>;
#endif
} // namespace RobotRaconteur