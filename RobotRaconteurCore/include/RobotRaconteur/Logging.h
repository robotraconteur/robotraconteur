/**
 * @file Logging.h
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

#include "RobotRaconteurConfig.h"
#include "RobotRaconteurConstants.h"
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <fstream>

namespace RobotRaconteur
{

class ROBOTRACONTEUR_CORE_API RobotRaconteurNode;
class ROBOTRACONTEUR_CORE_API MessageStringRef;

// Inspired by Boost.Log V2

/**
 * @brief Robot Raconteur log record
 *
 * Records information about a logging event
 *
 * See \ref logging for more information.
 *
 */
class ROBOTRACONTEUR_CORE_API RRLogRecord
{
  public:
    /** @brief The source node */
    RR_WEAK_PTR<RobotRaconteurNode> Node;
    /** @brief The log level */
    RobotRaconteur_LogLevel Level;
    /** @brief The source component */
    RobotRaconteur_LogComponent Component;
    /** @brief The source component name */
    std::string ComponentName;
    /** @brief The source component object ID */
    std::string ComponentObjectID;
    /** @brief The source endpoint */
    int64_t Endpoint;
    /** @brief The service path of the source object */
    std::string ServicePath;
    /** @brief The source member */
    std::string Member;
    /** @brief Human readable log message */
    std::string Message;
    /** @brief Time of logging event */
    boost::posix_time::ptime Time;
    /** @brief The sourcecode filename */
    std::string SourceFile;
    /** @brief The line within the sourcecode file */
    uint32_t SourceLine;
    /** @brief The source thread */
    std::string ThreadID;
    /** @brief The source coroutine fiber */
    std::string FiberID;

    RRLogRecord();
};

/** @brief Write a RRlogRecord to a stream */
ROBOTRACONTEUR_CORE_API std::ostream& operator<<(std::ostream& out, const RRLogRecord& record);

/** @brief Convert a RobotRaconteur_LogLevel code to a string */
ROBOTRACONTEUR_CORE_API std::string RRLogRecord_Level_ToString(RobotRaconteur_LogLevel level);
/** @brief Convert a RobotRaconteur_LogComponent code to a string */
ROBOTRACONTEUR_CORE_API std::string RRLogRecord_Component_ToString(RobotRaconteur_LogComponent component);
/** @brief Convert a RobotRaconteurNode weak_ptr to a NodeID string. Returns "unknown" if could not resolve */
ROBOTRACONTEUR_CORE_API std::string RRLogRecord_Node_ToString(const RR_WEAK_PTR<RobotRaconteurNode>& node);

class ROBOTRACONTEUR_CORE_API RRLogRecordStream : public boost::intrusive_ref_counter<RRLogRecordStream>
{
  protected:
    RRLogRecord record;
    RR_SHARED_PTR<RobotRaconteurNode> node;
    std::stringstream ss;

  public:
    RRLogRecordStream(const RR_SHARED_PTR<RobotRaconteurNode>& node);
    RRLogRecordStream(const RR_SHARED_PTR<RobotRaconteurNode>& node, RobotRaconteur_LogLevel lvl,
                      RobotRaconteur_LogComponent component, int64_t ep, const boost::posix_time::ptime& time,
                      const std::string& source_file, uint32_t source_line, const std::string& thread_id);
    RRLogRecordStream(const RR_SHARED_PTR<RobotRaconteurNode>& node, RobotRaconteur_LogLevel lvl,
                      RobotRaconteur_LogComponent component, const std::string& component_name,
                      const std::string& component_object_id, int64_t ep, const std::string& service_path,
                      const std::string& member, const boost::posix_time::ptime& time, const std::string& source_file,
                      uint32_t source_line, const std::string& thread_id, const std::string& fiber_id);
    ~RRLogRecordStream();
    std::stringstream& Stream();

    static RR_INTRUSIVE_PTR<RRLogRecordStream> OpenRecordStream(
        RR_WEAK_PTR<RobotRaconteurNode> node, RobotRaconteur_LogLevel lvl, RobotRaconteur_LogComponent component,
        const std::string& component_name, const std::string& component_object_id, int64_t ep,
        MessageStringRef service_path, MessageStringRef member_name, const std::string& source_file,
        uint32_t source_line);
};

/**
 * @brief Base class of log record handler
 *
 * By default, RobotRaconteurNode will print log records to std::cerr. Use
 * RobotRaconteurNode::SetLogRecordHandler() to specify a LogRecordHandler
 * to accept log records instead of printing them to the screen.
 *
 * See \ref logging for more information.
 */
class ROBOTRACONTEUR_CORE_API LogRecordHandler
{
  public:
    /**
     * @brief Function to accept log records from node
     *
     * Override in subclasses
     *
     * @param record The log record
     */
    virtual void HandleLogRecord(const RRLogRecord& record) = 0;
    virtual ~LogRecordHandler() {}
};

/**
 * @brief Log record handler that saves to a file
 *
 * See \ref logging for more information.
 */
class ROBOTRACONTEUR_CORE_API FileLogRecordHandler : public LogRecordHandler
{
    std::ofstream file;

  public:
    /**
     * @brief Open a file to store log records
     *
     * @param filename The filename
     * @param append If true, log messages are appended. If false, the file is truncated when opened
     */
    void OpenFile(const std::string& filename, bool append = true);
    RR_OVIRTUAL void HandleLogRecord(const RRLogRecord& record) RR_OVERRIDE;
};

#define ROBOTRACONTEUR_LOG_EMPTY_NODE
#define ROBOTRACONTEUR_LOG_DEFAULT_NODE RobotRaconteur::RobotRaconteurNode::weak_sp()

#ifndef ROBOTRACONTEUR_DISABLE_LOGGING

// NOLINTBEGIN(bugprone-macro-parentheses)
#define ROBOTRACONTEUR_LOG(node, lvl, component, component_name, component_object_id, ep, service_path, member, args)  \
    {                                                                                                                  \
        boost::intrusive_ptr<RobotRaconteur::RRLogRecordStream> ____rr_log_record_stream____ =                         \
            RobotRaconteur::RRLogRecordStream::OpenRecordStream(                                                       \
                node, RobotRaconteur::RobotRaconteur_LogLevel_##lvl,                                                   \
                RobotRaconteur::RobotRaconteur_LogComponent_##component, component_name, component_object_id, ep,      \
                service_path, member, __FILE__, __LINE__);                                                             \
        if (____rr_log_record_stream____)                                                                              \
        {                                                                                                              \
            ____rr_log_record_stream____->Stream() << args;                                                            \
        }                                                                                                              \
    }

// NOLINTEND(bugprone-macro-parentheses)

// TODO: Implement throttling
#define ROBOTRACONTEUR_LOG_THROTTLE(node, lvl, component, component_name, component_object_id, ep, service_path,       \
                                    member, limit, args)                                                               \
    ROBOTRACONTEUR_LOG(node, lvl, component, component_name, component_object_id, ep, service_path, member, args)
#else
#define ROBOTRACONTEUR_LOG(node, lvl, component, component_name, component_object_id, ep, service_path, member, args)
#define ROBOTRACONTEUR_LOG_THROTTLE(node, lvl, component, component_name, component_object_id, ep, service_path,       \
                                    member, limit, args)
#endif

#define ROBOTRACONTEUR_LOG_FATAL(node, args) ROBOTRACONTEUR_LOG(node, Fatal, Default, "", "", -1, "", "", args)
#define ROBOTRACONTEUR_LOG_FATAL_DEFAULT(args)                                                                         \
    ROBOTRACONTEUR_LOG(ROBOTRACONTEUR_LOG_DEFAULT_NODE, Fatal, Default, "", "", -1, "", "", args)
#define ROBOTRACONTEUR_LOG_FATAL_COMPONENT(node, component, ep, args)                                                  \
    ROBOTRACONTEUR_LOG(node, Fatal, component, "", "", ep, "", "", args)
#define ROBOTRACONTEUR_LOG_FATAL_COMPONENT_PATH(node, component, ep, service_path, member, args)                       \
    ROBOTRACONTEUR_LOG(node, Fatal, component, "", "", ep, service_path, member, args)
#define ROBOTRACONTEUR_LOG_FATAL_COMPONENTNAME(node, component, component_name, component_object_id, ep, args)         \
    ROBOTRACONTEUR_LOG(node, Fatal, component, component_name, component_object_id, ep, "", "", args)
#define ROBOTRACONTEUR_LOG_FATAL_COMPONENTNAME_PATH(node, component, component_name, component_object_id, ep,          \
                                                    service_path, member, args)                                        \
    ROBOTRACONTEUR_LOG(node, Fatal, component, component_name, component_object_id, ep, service_path, member, args)
#define ROBOTRACONTEUR_LOG_FATAL_THROTTLE(node, component, ep, limit, args)                                            \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Fatal, component, "", "", ep, "", "", limit, args)
#define ROBOTRACONTEUR_LOG_FATAL_THROTTLE_PATH(node, component, ep, service_path, member, limit, args)                 \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Fatal, component, "", "", ep, service_path, member, limit, args)
#define ROBOTRACONTEUR_LOG_FATAL_THROTTLE_COMPONENTNAME_PATH(node, component, component_name, component_object_id, ep, \
                                                             service_path, member, limit, args)                        \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Fatal, component, component_name, component_object_id, ep, service_path, member, \
                                limit, args)

#define ROBOTRACONTEUR_LOG_ERROR(node, args) ROBOTRACONTEUR_LOG(node, Error, Default, "", "", -1, "", "", args)
#define ROBOTRACONTEUR_LOG_ERROR_DEFAULT(args)                                                                         \
    ROBOTRACONTEUR_LOG(ROBOTRACONTEUR_LOG_DEFAULT_NODE, Error, Default, "", "", -1, "", "", args)
#define ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, component, ep, args)                                                  \
    ROBOTRACONTEUR_LOG(node, Error, component, "", "", ep, "", "", args)
#define ROBOTRACONTEUR_LOG_ERROR_COMPONENT_PATH(node, component, ep, service_path, member, args)                       \
    ROBOTRACONTEUR_LOG(node, Error, component, "", "", ep, service_path, member, args)
#define ROBOTRACONTEUR_LOG_ERROR_COMPONENTNAME(node, component, component_name, component_object_id, ep, args)         \
    ROBOTRACONTEUR_LOG(node, Error, component, component_name, component_object_id, ep, "", "", args)
#define ROBOTRACONTEUR_LOG_ERROR_COMPONENTNAME_PATH(node, component, component_name, component_object_id, ep,          \
                                                    service_path, member, args)                                        \
    ROBOTRACONTEUR_LOG(node, Error, component, component_name, component_object_id, ep, service_path, member, args)
#define ROBOTRACONTEUR_LOG_ERROR_THROTTLE(node, component, ep, limit, args)                                            \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Error, component, "", "", ep, "", "", limit, args)
#define ROBOTRACONTEUR_LOG_ERROR_THROTTLE_PATH(node, component, ep, service_path, member, limit, args)                 \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Error, component, "", "", ep, service_path, member, limit, args)
#define ROBOTRACONTEUR_LOG_ERROR_THROTTLE_COMPONENTNAME_PATH(node, component, component_name, component_object_id, ep, \
                                                             service_path, member, limit, args)                        \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Error, component, component_name, component_object_id, ep, service_path, member, \
                                limit, args)

#define ROBOTRACONTEUR_LOG_WARNING(node, args) ROBOTRACONTEUR_LOG(node, Warning, Default, "", "", -1, "", "", args)
#define ROBOTRACONTEUR_LOG_WARNING_DEFAULT(args)                                                                       \
    ROBOTRACONTEUR_LOG(ROBOTRACONTEUR_LOG_DEFAULT_NODE, Warning, Default, "", "", -1, "", "", args)
#define ROBOTRACONTEUR_LOG_WARNING_COMPONENT(node, component, ep, args)                                                \
    ROBOTRACONTEUR_LOG(node, Warning, component, "", "", ep, "", "", args)
#define ROBOTRACONTEUR_LOG_WARNING_COMPONENT_PATH(node, component, ep, service_path, member, args)                     \
    ROBOTRACONTEUR_LOG(node, Warning, component, "", "", ep, service_path, member, args)
#define ROBOTRACONTEUR_LOG_WARNING_COMPONENTNAME(node, component, component_name, component_object_id, ep, args)       \
    ROBOTRACONTEUR_LOG(node, Warning, component, component_name, component_object_id, ep, "", "", args)
#define ROBOTRACONTEUR_LOG_WARNING_COMPONENTNAME_PATH(node, component, component_name, component_object_id, ep,        \
                                                      service_path, member, args)                                      \
    ROBOTRACONTEUR_LOG(node, Warning, component, component_name, component_object_id, ep, service_path, member, args)
#define ROBOTRACONTEUR_LOG_WARNING_THROTTLE(node, component, ep, limit, args)                                          \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Warning, component, "", "", ep, "", "", limit, args)
#define ROBOTRACONTEUR_LOG_WARNING_THROTTLE_PATH(node, component, ep, service_path, member, limit, args)               \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Warning, component, "", "", ep, service_path, member, limit, args)
#define ROBOTRACONTEUR_LOG_WARNING_THROTTLE_COMPONENTNAME_PATH(node, component, component_name, component_object_id,   \
                                                               ep, service_path, member, limit, args)                  \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Warning, component, component_name, component_object_id, ep, service_path,       \
                                member, limit, args)

#define ROBOTRACONTEUR_LOG_INFO(node, args) ROBOTRACONTEUR_LOG(node, Info, Default, "", "", -1, "", "", args)
#define ROBOTRACONTEUR_LOG_INFO_DEFAULT(args)                                                                          \
    ROBOTRACONTEUR_LOG(ROBOTRACONTEUR_LOG_DEFAULT_NODE, Info, Default, "", "", -1, "", "", args)
#define ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, component, ep, args)                                                   \
    ROBOTRACONTEUR_LOG(node, Info, component, "", "", ep, "", "", args)
#define ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, component, ep, service_path, member, args)                        \
    ROBOTRACONTEUR_LOG(node, Info, component, "", "", ep, service_path, member, args)
#define ROBOTRACONTEUR_LOG_INFO_COMPONENTNAME(node, component, component_name, component_object_id, ep, args)          \
    ROBOTRACONTEUR_LOG(node, Info, component, component_name, component_object_id, ep, "", "", args)
#define ROBOTRACONTEUR_LOG_INFO_COMPONENTNAME_PATH(node, component, component_name, component_object_id, ep,           \
                                                   service_path, member, args)                                         \
    ROBOTRACONTEUR_LOG(node, Info, component, component_name, component_object_id, ep, service_path, member, args)
#define ROBOTRACONTEUR_LOG_INFO_THROTTLE(node, component, ep, limit, args)                                             \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Info, component, "", "", ep, "", "", limit, args)
#define ROBOTRACONTEUR_LOG_INFO_THROTTLE_PATH(node, component, ep, service_path, member, limit, args)                  \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Info, component, "", "", ep, service_path, member, limit, args)
#define ROBOTRACONTEUR_LOG_INFO_THROTTLE_COMPONENTNAME_PATH(node, component, component_name, component_object_id, ep,  \
                                                            service_path, member, limit, args)                         \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Info, component, component_name, component_object_id, ep, service_path, member,  \
                                limit, args)

#define ROBOTRACONTEUR_LOG_DEBUG(node, args) ROBOTRACONTEUR_LOG(node, Debug, Default, "", "", -1, "", "", args)
#define ROBOTRACONTEUR_LOG_DEBUG_DEFAULT(args)                                                                         \
    ROBOTRACONTEUR_LOG(ROBOTRACONTEUR_LOG_DEFAULT_NODE, Debug, Default, "", "", -1, "", "", args)
#define ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, component, ep, args)                                                  \
    ROBOTRACONTEUR_LOG(node, Debug, component, "", "", ep, "", "", args)
#define ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, component, ep, service_path, member, args)                       \
    ROBOTRACONTEUR_LOG(node, Debug, component, "", "", ep, service_path, member, args)
#define ROBOTRACONTEUR_LOG_DEBUG_COMPONENTNAME(node, component, component_name, component_object_id, ep, args)         \
    ROBOTRACONTEUR_LOG(node, Debug, component, component_name, component_object_id, ep, "", "", args)
#define ROBOTRACONTEUR_LOG_DEBUG_COMPONENTNAME_PATH(node, component, component_name, component_object_id, ep,          \
                                                    service_path, member, args)                                        \
    ROBOTRACONTEUR_LOG(node, Debug, component, component_name, component_object_id, ep, service_path, member, args)
#define ROBOTRACONTEUR_LOG_DEBUG_THROTTLE(node, component, ep, limit, args)                                            \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Debug, component, "", "", ep, "", "", limit, args)
#define ROBOTRACONTEUR_LOG_DEBUG_THROTTLE_PATH(node, component, ep, service_path, member, limit, args)                 \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Debug, component, "", "", ep, service_path, member, limit, args)
#define ROBOTRACONTEUR_LOG_DEBUG_THROTTLE_COMPONENTNAME_PATH(node, component, component_name, component_object_id, ep, \
                                                             service_path, member, limit, args)                        \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Debug, component, component_name, component_object_id, ep, service_path, member, \
                                limit, args)

#ifndef NDEBUG
#define ROBOTRACONTEUR_LOG_TRACE(node, args) ROBOTRACONTEUR_LOG(node, Trace, Default, "", "", -1, "", "", args)
#define ROBOTRACONTEUR_LOG_TRACE_DEFAULT(args)                                                                         \
    ROBOTRACONTEUR_LOG(ROBOTRACONTEUR_LOG_DEFAULT_NODE, Trace, Default, "", "", -1, "", "", args)
#define ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, component, ep, args)                                                  \
    ROBOTRACONTEUR_LOG(node, Trace, component, "", "", ep, "", "", args)
#define ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, component, ep, service_path, member, args)                       \
    ROBOTRACONTEUR_LOG(node, Trace, component, "", "", ep, service_path, member, args)
#define ROBOTRACONTEUR_LOG_TRACE_COMPONENTNAME(node, component, component_name, component_object_id, ep, args)         \
    ROBOTRACONTEUR_LOG(node, Trace, component, component_name, component_object_id, ep, "", "", args)
#define ROBOTRACONTEUR_LOG_TRACE_COMPONENTNAME_PATH(node, component, component_name, component_object_id, ep,          \
                                                    service_path, member, args)                                        \
    ROBOTRACONTEUR_LOG(node, Trace, component, component_name, component_object_id, ep, service_path, member, args)
#define ROBOTRACONTEUR_LOG_TRACE_THROTTLE(node, component, ep, limit, args)                                            \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Trace, component, "", "", ep, "", "", limit, args)
#define ROBOTRACONTEUR_LOG_TRACE_THROTTLE_PATH(node, component, ep, service_path, member, limit, args)                 \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Trace, component, "", "", ep, service_path, member, limit, args)
#define ROBOTRACONTEUR_LOG_TRACE_THROTTLE_COMPONENTNAME_PATH(node, component, component_name, component_object_id, ep, \
                                                             service_path, member, limit, args)                        \
    ROBOTRACONTEUR_LOG_THROTTLE(node, Trace, component, component_name, component_object_id, ep, service_path, member, \
                                limit, args)
#else
// Disable Trace log level in release builds for performance
#define ROBOTRACONTEUR_LOG_TRACE(node, args)
#define ROBOTRACONTEUR_LOG_TRACE_DEFAULT(args)
#define ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, component, ep, args)
#define ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, component, ep, service_path, member, args)
#define ROBOTRACONTEUR_LOG_TRACE_COMPONENTNAME(node, component, component_name, component_object_id, ep, args)
#define ROBOTRACONTEUR_LOG_TRACE_COMPONENTNAME_PATH(node, component, component_name, component_object_id, ep,          \
                                                    service_path, member, args)
#define ROBOTRACONTEUR_LOG_TRACE_THROTTLE(node, component, ep, limit, args)
#define ROBOTRACONTEUR_LOG_TRACE_THROTTLE_PATH(node, component, ep, service_path, member, limit, args)
#define ROBOTRACONTEUR_LOG_TRACE_THROTTLE_COMPONENTNAME_PATH(node, component, component_name, component_object_id, ep, \
                                                             service_path, member, limit, args)

#endif

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using RRLogRecordStreamPtr = RR_INTRUSIVE_PTR<RRLogRecordStream>;
using RRLogRecordStreamConstPtr = RR_INTRUSIVE_PTR<const RRLogRecordStream>;
/** @brief Convenience alias for LogRecordHandler shared_ptr */
using LogRecordHandlerPtr = RR_SHARED_PTR<LogRecordHandler>;
/** @brief Convenience alias for LogRecordHandler const shared_ptr */
using LogRecordHandlerConstPtr = RR_SHARED_PTR<const LogRecordHandler>;
/** @brief Convenience alias for FileLogRecordHandler shared_ptr */
using FileLogRecordHandlerPtr = RR_SHARED_PTR<FileLogRecordHandler>;
/** @brief Convenience alias for FileLogRecordHandler const shared_ptr */
using FileLogRecordHandlerConstPtr = RR_SHARED_PTR<const FileLogRecordHandler>;
#endif

} // namespace RobotRaconteur