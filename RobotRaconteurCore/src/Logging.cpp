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

#include "RobotRaconteur/Logging.h"
#include "RobotRaconteur/RobotRaconteurNode.h"
#include <boost/filesystem.hpp>

namespace RobotRaconteur
{

std::ostream& operator<<(std::ostream& out, const RRLogRecord& record)
{
    out << "[" << to_iso_extended_string(record.Time) << "] [" << RRLogRecord_Level_ToString(record.Level) << "] ["
        << record.ThreadID;
    if (!record.FiberID.empty())
    {
        out << "," << record.FiberID;
    }
    out << "] [" << RRLogRecord_Node_ToString(record.Node) << "] [" << RRLogRecord_Component_ToString(record.Component);
    if (!record.ComponentName.empty() || !record.ComponentObjectID.empty())
    {
        out << "," << record.ComponentName << "," << record.ComponentObjectID;
    }
    out << "] [" << record.Endpoint;
    if (!record.ServicePath.empty())
    {
        out << "," << record.ServicePath;
        if (!record.Member.empty())
        {
            out << "," << record.Member;
        }
    }
    out << "] ";

    if (!record.SourceFile.empty())
    {
        out << "[" << boost::filesystem::path(record.SourceFile).filename().string() << ":" << record.SourceLine
            << "] ";
    }

    out << record.Message;
    return out;
}

RRLogRecordStream::RRLogRecordStream(RR_SHARED_PTR<RobotRaconteurNode> node)
{
    BOOST_ASSERT_MSG(node, "RRLogRecordStream node must not be nullptr");
    this->node = node;
    record.Node = node;
    record.Level = RobotRaconteur_LogLevel_Trace;
    record.Component = RobotRaconteur_LogComponent_Default;
    record.Time = boost::posix_time::microsec_clock::local_time();
}

RRLogRecordStream::RRLogRecordStream(RR_SHARED_PTR<RobotRaconteurNode> node, RobotRaconteur_LogLevel lvl,
                                     RobotRaconteur_LogComponent component, int64_t ep,
                                     const boost::posix_time::ptime& time, const std::string& source_file,
                                     uint32_t source_line, const std::string& thread_id)
{
    BOOST_ASSERT_MSG(node, "RRLogRecordStream node must not be nullptr");

    this->node = node;
    record.Node = node;
    record.Level = lvl;
    record.Component = component;
    record.Endpoint = ep;
    record.Time = time;
    record.SourceFile = source_file;
    record.SourceLine = source_line;
    record.ThreadID = thread_id;
}

RRLogRecordStream::RRLogRecordStream(RR_SHARED_PTR<RobotRaconteurNode> node, RobotRaconteur_LogLevel lvl,
                                     RobotRaconteur_LogComponent component, const std::string& component_name,
                                     const std::string& component_object_id, int64_t ep,
                                     const std::string& service_path, const std::string& member,
                                     const boost::posix_time::ptime& time, const std::string& source_file,
                                     uint32_t source_line, const std::string& thread_id, const std::string& fiber_id)
{
    BOOST_ASSERT_MSG(node, "RRLogRecordStream node must not be nullptr");

    this->node = node;
    record.Node = node;
    record.Level = lvl;
    record.Component = component;
    record.ComponentName = component_name;
    record.ComponentObjectID = component_object_id;
    record.Endpoint = ep;
    record.ServicePath = service_path;
    record.Member = member;
    record.Time = time;
    record.SourceFile = source_file;
    record.SourceLine = source_line;
    record.ThreadID = thread_id;
    record.FiberID = fiber_id;
}

RRLogRecordStream::~RRLogRecordStream()
{
    record.Message = ss.str();
    node->LogRecord(record);
}

std::stringstream& RRLogRecordStream::Stream() { return ss; }

RR_INTRUSIVE_PTR<RRLogRecordStream> RRLogRecordStream::OpenRecordStream(
    RR_WEAK_PTR<RobotRaconteurNode> node, RobotRaconteur_LogLevel lvl, RobotRaconteur_LogComponent component,
    const std::string& component_name, const std::string& component_object_id, int64_t ep,
    MessageStringRef service_path, MessageStringRef member_name, const std::string& source_file, uint32_t source_line)
{
    RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
    if (!node1)
        return RR_INTRUSIVE_PTR<RRLogRecordStream>();

    if (!node1->CompareLogLevel(lvl))
        return RR_INTRUSIVE_PTR<RRLogRecordStream>();

    return new RRLogRecordStream(node1, lvl, component, component_name, component_object_id, ep,
                                 service_path.str().to_string(), member_name.str().to_string(),
                                 boost::posix_time::microsec_clock::local_time(), source_file, source_line,
                                 "0x" + boost::lexical_cast<std::string>(boost::this_thread::get_id()), "");
}

std::string RRLogRecord_Level_ToString(RobotRaconteur_LogLevel level)
{
    switch (level)
    {
    case RobotRaconteur_LogLevel_Trace:
        return "trace";
    case RobotRaconteur_LogLevel_Debug:
        return "debug";
    case RobotRaconteur_LogLevel_Info:
        return "info";
    case RobotRaconteur_LogLevel_Warning:
        return "warning";
    case RobotRaconteur_LogLevel_Error:
        return "error";
    case RobotRaconteur_LogLevel_Fatal:
        return "fatal";
    default:
        return "unknown";
    }
}
std::string RRLogRecord_Component_ToString(RobotRaconteur_LogComponent component)
{
    switch (component)
    {
    case RobotRaconteur_LogComponent_Default:
        return "default";
    case RobotRaconteur_LogComponent_Node:
        return "node";
    case RobotRaconteur_LogComponent_Transport:
        return "transport";
    case RobotRaconteur_LogComponent_Message:
        return "message";
    case RobotRaconteur_LogComponent_Client:
        return "client";
    case RobotRaconteur_LogComponent_Service:
        return "service";
    case RobotRaconteur_LogComponent_Member:
        return "member";
    case RobotRaconteur_LogComponent_Pack:
        return "pack";
    case RobotRaconteur_LogComponent_Unpack:
        return "unpack";
    case RobotRaconteur_LogComponent_ServiceDefinition:
        return "service_definition";
    case RobotRaconteur_LogComponent_Discovery:
        return "discovery";
    case RobotRaconteur_LogComponent_Subscription:
        return "subscription";
    case RobotRaconteur_LogComponent_NodeSetup:
        return "node_setup";
    case RobotRaconteur_LogComponent_Utility:
        return "utility";
    case RobotRaconteur_LogComponent_RobDefLib:
        return "robdeflib";
    case RobotRaconteur_LogComponent_User:
        return "user";
    case RobotRaconteur_LogComponent_UserClient:
        return "user_client";
    case RobotRaconteur_LogComponent_UserService:
        return "user_service";
    case RobotRaconteur_LogComponent_ThirdParty:
        return "third_party";
    default:
        return "unknown";
    }
}

std::string RRLogRecord_Node_ToString(const RR_WEAK_PTR<RobotRaconteurNode>& node)
{
    RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();

    if (!node1)
    {
        return "unknown";
    }

    NodeID id;
    std::string name;
    if (!node1->TryGetNodeID(id))
    {
        return "unknown";
    }
    if (!node1->TryGetNodeName(name) || name.empty())
    {
        return id.ToString("B");
    }
    return id.ToString("B") + "," + name;
}

void FileLogRecordHandler::OpenFile(const std::string& filename, bool append)
{
    if (append)
    {
        file.open(filename.c_str(), std::ofstream::app);
    }
    else
    {
        file.open(filename.c_str(), std::ofstream::trunc);
    }

    if (!file.is_open())
    {
        throw SystemResourceException("Open file for writing log failed");
    }
}

void FileLogRecordHandler::HandleLogRecord(const RRLogRecord& record)
{
    try
    {
        file << record << std::endl;
    }
    catch (std::exception& exp)
    {
        std::cerr << "Failed to log record to file: " << record << std::endl;
    }
}

} // namespace RobotRaconteur