/**
 * @file Tap.h
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

#include "RobotRaconteur/Logging.h"
#include "RobotRaconteur/Message.h"
#include <boost/bind/placeholders.hpp>
#include <boost/asio.hpp>

namespace RobotRaconteur
{
/**
 * @brief Base class for message taps
 *
 * Taps are used to save messages and log records that pass through a node.
 * The saved messages and logs can be analyzed and replayed for visualization,
 * diagnostics, and analysis.
 *
 * Message taps are configured using RobotRaconteurNodeSetup command line
 * options (recommended), or by registering the tap using
 * RobotRaconteurNode::SetMessageTap().
 *
 * See \ref taps for more information on taps.
 *
 */
class ROBOTRACONTEUR_CORE_API MessageTap
{
  public:
    /**
     * @brief Open the tap for writing
     *
     */
    virtual void Open() = 0;
    /**
     * @brief Close the tap
     *
     */
    virtual void Close() = 0;

    /**
     * @brief Record a log record
     *
     * @param log_record
     */
    virtual void RecordLogRecord(const RRLogRecord& log_record) = 0;

    /**
     * @brief Record a message
     *
     * @param message
     */
    virtual void RecordMessage(RR_INTRUSIVE_PTR<Message> message) = 0;

    virtual ~MessageTap() {}
};

namespace detail
{
class LocalMessageTapImpl;
}

/**
 * @brief Local message tap
 *
 * The LocalMessageTap works by creating a UNIX domain socket in an operating
 * system dependant directory, and writes messages to the socket. Log records
 * are encoded as messages and sent interleaved with messages from the transport.
 * UTC timestamps are added to the MetaData header field of the message.
 *
 * It is recommended that RobotRaconteurNodeSetup command line options
 * be used to configure the local tap. See \ref command_line_options.
 *
 * See \ref taps for more information on taps.
 *
 */
class ROBOTRACONTEUR_CORE_API LocalMessageTap : public MessageTap
{
    RR_WEAK_PTR<detail::LocalMessageTapImpl> tap_impl;
    std::string tap_name;

  public:
    /**
     * @brief Construct a new local tap
     *
     * Must use boost::make_shared<LocalMessageTap>()
     *
     * @param tap_name The name of the local tap
     */
    LocalMessageTap(const std::string& tap_name);
    ~LocalMessageTap();

    virtual void Open();
    virtual void Close();

    virtual void RecordLogRecord(const RRLogRecord& log_record);
    virtual void RecordMessage(RR_INTRUSIVE_PTR<Message> message);
};

}; // namespace RobotRaconteur