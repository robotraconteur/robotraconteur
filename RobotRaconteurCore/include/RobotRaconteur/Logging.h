/** 
 * @file Logging.h
 * 
 * @author Dr. John Wason
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

namespace RobotRaconteur
{

class ROBOTRACONTEUR_CORE_API RobotRaconteurNode;

// Inspired by Boost.Log V2

class ROBOTRACONTEUR_CORE_API RRLogRecord
{
public:
    RR_WEAK_PTR<RobotRaconteurNode> Node;
    RobotRaconteur_LogLevel Level;
    RobotRaconteur_LogSource Source;
    int64_t Endpoint;
    std::string ServicePath;
    std::string Member;
    std::string Message;
    boost::posix_time::ptime Time;
    std::string SourceFile;
    uint32_t SourceLine;
    std::string ThreadID;
};

ROBOTRACONTEUR_CORE_API std::ostream & operator << (std::ostream &out, const RRLogRecord &record);

ROBOTRACONTEUR_CORE_API std::string RRLogRecord_Level_ToString(RobotRaconteur_LogLevel level);
ROBOTRACONTEUR_CORE_API std::string RRLogRecord_Source_ToString(RobotRaconteur_LogSource source);
ROBOTRACONTEUR_CORE_API std::string RRLogRecord_Node_ToString(const RR_WEAK_PTR<RobotRaconteurNode>& node);

class ROBOTRACONTEUR_CORE_API RRLogRecordStream : public boost::intrusive_ref_counter<RRLogRecordStream>
{
protected:
    RRLogRecord record;
    RR_SHARED_PTR<RobotRaconteurNode> node;
    std::stringstream ss;

public:

    RRLogRecordStream(RR_SHARED_PTR<RobotRaconteurNode> node);
    RRLogRecordStream(RR_SHARED_PTR<RobotRaconteurNode> node, RobotRaconteur_LogLevel lvl, RobotRaconteur_LogSource source, int64_t ep, const boost::posix_time::ptime& time, const std::string& source_file, uint32_t source_line, const std::string& thread_id);
    RRLogRecordStream(RR_SHARED_PTR<RobotRaconteurNode> node, RobotRaconteur_LogLevel lvl, RobotRaconteur_LogSource source, int64_t ep, const std::string& service_path, const std::string& member, const boost::posix_time::ptime& time, const std::string& source_file, uint32_t source_line, const std::string& thread_id);
    ~RRLogRecordStream();
    std::stringstream& Stream();

    static RR_INTRUSIVE_PTR<RRLogRecordStream> OpenRecordStream(RR_WEAK_PTR<RobotRaconteurNode> node, RobotRaconteur_LogLevel lvl, RobotRaconteur_LogSource source, int64_t ep, const std::string& service_path, const std::string& member_name, const std::string& source_file, uint32_t source_line);

};


#define ROBOTRACONTEUR_LOG_EMPTY_NODE
#define ROBOTRACONTEUR_LOG_DEFAULT_NODE RobotRaconteur::RobotRaconteurNode::weak_sp()

#ifndef ROBOTRACONTEUR_DISABLE_LOGGING
    #define ROBOTRACONTEUR_LOG(node,lvl,source,ep,service_path,member,args) \
    { \
        boost::intrusive_ptr<RobotRaconteur::RRLogRecordStream> ____rr_log_record_stream____ = \
            RobotRaconteur::RRLogRecordStream::OpenRecordStream(node,RobotRaconteur::RobotRaconteur_LogLevel_ ## lvl, \
            RobotRaconteur::RobotRaconteur_LogSource_ ## source, ep, service_path, member, __FILE__, __LINE__); \
        if (____rr_log_record_stream____ ) { ____rr_log_record_stream____->Stream() << args; } \
    } \
    //TODO: Implement throttling
    #define ROBOTRACONTEUR_LOG_THROTTLE(node,lvl,source,ep,service_path,member,limit,args) ROBOTRACONTEUR_LOG(node,lvl,source,ep,service_path,member,args)
#else
    #define ROBOTRACONTEUR_LOG(node,lvl,source,ep,service_path,member,args)
    #define ROBOTRACONTEUR_LOG_THROTTLE(node,lvl,source,ep,service_path,member,limit,args)
#endif

#define ROBOTRACONTEUR_LOG_FATAL(node,args) ROBOTRACONTEUR_LOG(node,Fatal,Default,-1,"","",args)
#define ROBOTRACONTEUR_LOG_FATAL_DEFAULT(args) ROBOTRACONTEUR_LOG(ROBOTRACONTEUR_LOG_DEFAULT_NODE,Fatal,Default,-1,"","",args)
#define ROBOTRACONTEUR_LOG_FATAL_SOURCE(node,source,ep,args) ROBOTRACONTEUR_LOG(node,Fatal,source,ep,"","",args)
#define ROBOTRACONTEUR_LOG_FATAL_SOURCE_PATH(node,source,ep,service_path,member,args) ROBOTRACONTEUR_LOG(node,Fatal,source,ep,service_path,member,args)
#define ROBOTRACONTEUR_LOG_FATAL_THROTTLE(node,source,ep,limit,args) ROBOTRACONTEUR_LOG_THROTTLE(node,Fatal,source,ep,limit,"","",args)
#define ROBOTRACONTEUR_LOG_FATAL_THROTTLE_PATH(node,source,ep,limit,service_path,member,args) ROBOTRACONTEUR_LOG_THROTTLE(node,Fatal,source,ep,limit,service_path,member,args)     

#define ROBOTRACONTEUR_LOG_ERROR(node,args) ROBOTRACONTEUR_LOG(node,Error,Default,-1,"","",args)
#define ROBOTRACONTEUR_LOG_ERROR_DEFAULT(args) ROBOTRACONTEUR_LOG(ROBOTRACONTEUR_LOG_DEFAULT_NODE,Error,Default,-1,"","",args)
#define ROBOTRACONTEUR_LOG_ERROR_SOURCE(node,source,ep,args) ROBOTRACONTEUR_LOG(node,Error,source,ep,"","",args)
#define ROBOTRACONTEUR_LOG_ERROR_SOURCE_PATH(node,source,ep,service_path,member,args) ROBOTRACONTEUR_LOG(node,Error,source,ep,service_path,member,args)
#define ROBOTRACONTEUR_LOG_ERROR_THROTTLE(node,source,ep,limit,args) ROBOTRACONTEUR_LOG_THROTTLE(node,Error,source,ep,limit,"","",args)
#define ROBOTRACONTEUR_LOG_ERROR_THROTTLE_PATH(node,source,ep,limit,service_path,member,args) ROBOTRACONTEUR_LOG_THROTTLE(node,Error,source,ep,limit,service_path,member,args)     

#define ROBOTRACONTEUR_LOG_WARN(node,args) ROBOTRACONTEUR_LOG(node,Warning,Default,-1,"","",args)
#define ROBOTRACONTEUR_LOG_WARN_DEFAULT(args) ROBOTRACONTEUR_LOG(ROBOTRACONTEUR_LOG_DEFAULT_NODE,Warning,Default,-1,"","",args)
#define ROBOTRACONTEUR_LOG_WARN_SOURCE(node,source,ep,args) ROBOTRACONTEUR_LOG(node,Warning,source,ep,"","",args)
#define ROBOTRACONTEUR_LOG_WARN_SOURCE_PATH(node,source,ep,service_path,member,args) ROBOTRACONTEUR_LOG(node,Warning,source,ep,service_path,member,args)
#define ROBOTRACONTEUR_LOG_WARN_THROTTLE(node,source,ep,limit,args) ROBOTRACONTEUR_LOG_THROTTLE(node,Warning,source,ep,limit,"","",args)
#define ROBOTRACONTEUR_LOG_WARN_THROTTLE_PATH(node,source,ep,limit,service_path,member,args) ROBOTRACONTEUR_LOG_THROTTLE(node,Warning,source,ep,limit,service_path,member,args)    

#define ROBOTRACONTEUR_LOG_INFO(node,args) ROBOTRACONTEUR_LOG(node,Info,Default,-1,"","",args)
#define ROBOTRACONTEUR_LOG_INFO_DEFAULT(args) ROBOTRACONTEUR_LOG(ROBOTRACONTEUR_LOG_DEFAULT_NODE,Info,Default,-1,"","",args)
#define ROBOTRACONTEUR_LOG_INFO_SOURCE(node,source,ep,args) ROBOTRACONTEUR_LOG(node,Info,source,ep,"","",args)
#define ROBOTRACONTEUR_LOG_INFO_SOURCE_PATH(node,source,ep,service_path,member,args) ROBOTRACONTEUR_LOG(node,Info,source,ep,service_path,member,args)
#define ROBOTRACONTEUR_LOG_INFO_THROTTLE(node,source,ep,limit,args) ROBOTRACONTEUR_LOG_THROTTLE(node,Info,source,ep,limit,"","",args)
#define ROBOTRACONTEUR_LOG_INFO_THROTTLE_PATH(node,source,ep,limit,service_path,member,args) ROBOTRACONTEUR_LOG_THROTTLE(node,Info,source,ep,limit,service_path,member,args)       

#define ROBOTRACONTEUR_LOG_DEBUG(node,args) ROBOTRACONTEUR_LOG(node,Debug,Default,-1,"","",args)
#define ROBOTRACONTEUR_LOG_DEBUG_DEFAULT(args) ROBOTRACONTEUR_LOG(ROBOTRACONTEUR_LOG_DEFAULT_NODE,Debug,Default,-1,"","",args)
#define ROBOTRACONTEUR_LOG_DEBUG_SOURCE(node,source,ep,args) ROBOTRACONTEUR_LOG(node,Debug,source,ep,"","",args)
#define ROBOTRACONTEUR_LOG_DEBUG_SOURCE_PATH(node,source,ep,service_path,member,args) ROBOTRACONTEUR_LOG(node,Debug,source,ep,service_path,member,args)
#define ROBOTRACONTEUR_LOG_DEBUG_THROTTLE(node,source,ep,limit,args) ROBOTRACONTEUR_LOG_THROTTLE(node,Debug,source,ep,limit,"","",args)
#define ROBOTRACONTEUR_LOG_DEBUG_THROTTLE_PATH(node,source,ep,limit,service_path,member,args) ROBOTRACONTEUR_LOG_THROTTLE(node,Debug,source,ep,limit,service_path,member,args)     

#define ROBOTRACONTEUR_LOG_TRACE(node,args) ROBOTRACONTEUR_LOG(node,Trace,Default,-1,"","",args)
#define ROBOTRACONTEUR_LOG_TRACE_DEFAULT(args) ROBOTRACONTEUR_LOG(ROBOTRACONTEUR_LOG_DEFAULT_NODE,Trace,Default,-1,"","",args)
#define ROBOTRACONTEUR_LOG_TRACE_SOURCE(node,source,ep,args) ROBOTRACONTEUR_LOG(node,Trace,source,ep,"","",args)
#define ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node,source,ep,service_path,member,args) ROBOTRACONTEUR_LOG(node,Trace,source,ep,service_path,member,args)
#define ROBOTRACONTEUR_LOG_TRACE_THROTTLE(node,source,ep,limit,args) ROBOTRACONTEUR_LOG_THROTTLE(node,Trace,source,ep,limit,"","",args)
#define ROBOTRACONTEUR_LOG_TRACE_THROTTLE_PATH(node,source,ep,limit,service_path,member,args) ROBOTRACONTEUR_LOG_THROTTLE(node,Trace,source,ep,limit,service_path,member,args)  
}