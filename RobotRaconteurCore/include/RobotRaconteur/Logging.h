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

#ifndef ROBOTRACONTEUR_DISABLE_LOGGING
#include <boost/log/trivial.hpp>
#endif

namespace RobotRaconteur
{

class ROBOTRACONTEUR_CORE_API RobotRaconteurNode;

#define ROBOTRACONTEUR_LOG_EMPTY_NODE
#define ROBOTRACONTEUR_LOG_DEFAULT_NODE RobotRaconteur::RobotRaconteurNode::weak_sp()

#ifndef ROBOTRACONTEUR_DISABLE_LOGGING
    #define ROBOTRACONTEUR_LOG_LEVEL(lvl,node,args) BOOST_LOG_TRIVIAL(lvl) << RobotRaconteur::detail::rr_get_node_log_ident(node) << args
    //TODO: Implement throttling
    #define ROBOTRACONTEUR_LOG_LEVEL_THROTTLE(lvl,node,limit,args) ROBOTRACONTEUR_LOG_LEVEL(lvl,node) << args
#else
    #define ROBOTRACONTEUR_LOG_LEVEL(lvl,node,args)
    #define ROBOTRACONTEUR_LOG_LEVEL_THROTTLE(lvl,node,limit,args)
#endif

#define ROBOTRACONTEUR_LOG_FATAL_GLOBAL(args) ROBOTRACONTEUR_LOG_LEVEL(fatal,,args)
#define ROBOTRACONTEUR_LOG_FATAL_DEFAULT(args) ROBOTRACONTEUR_LOG_LEVEL(fatal,ROBOTRACONTEUR_LOG_DEFAULT_NODE,args)
#define ROBOTRACONTEUR_LOG_FATAL(node,args) ROBOTRACONTEUR_LOG_LEVEL(fatal,node,args)
#define ROBOTRACONTEUR_LOG_FATAL_THROTTLE(node,limit,args) ROBOTRACONTEUR_LOG_LEVEL(fatal,node,limit,args)

#define ROBOTRACONTEUR_LOG_ERROR_GLOBAL(args) ROBOTRACONTEUR_LOG_LEVEL(error,,args)
#define ROBOTRACONTEUR_LOG_ERROR_DEFAULT(args) ROBOTRACONTEUR_LOG_LEVEL(error,ROBOTRACONTEUR_LOG_DEFAULT_NODE,args)
#define ROBOTRACONTEUR_LOG_ERROR(node,args) ROBOTRACONTEUR_LOG_LEVEL(error,node,args)
#define ROBOTRACONTEUR_LOG_ERROR_THROTTLE(node,limit,args) ROBOTRACONTEUR_LOG_LEVEL(error,node,limit,args)

#define ROBOTRACONTEUR_LOG_WARN_GLOBAL(args) ROBOTRACONTEUR_LOG_LEVEL(warning,,args)
#define ROBOTRACONTEUR_LOG_WARN_DEFAULT(args) ROBOTRACONTEUR_LOG_LEVEL(warning,ROBOTRACONTEUR_LOG_DEFAULT_NODE,args)
#define ROBOTRACONTEUR_LOG_WARN(node,args) ROBOTRACONTEUR_LOG_LEVEL(warning,node,args)
#define ROBOTRACONTEUR_LOG_WARN_THROTTLE(node,limit,args) ROBOTRACONTEUR_LOG_LEVEL_THROTTLE(warning,node,limit,args)

#define ROBOTRACONTEUR_LOG_INFO_GLOBAL(args) ROBOTRACONTEUR_LOG_LEVEL(info,,args)
#define ROBOTRACONTEUR_LOG_INFO_DEFAULT(args) ROBOTRACONTEUR_LOG_LEVEL(info,ROBOTRACONTEUR_LOG_DEFAULT_NODE,args)
#define ROBOTRACONTEUR_LOG_INFO(node,args) ROBOTRACONTEUR_LOG_LEVEL(info,node,args)
#define ROBOTRACONTEUR_LOG_INFO_THROTTLE(node,limit,args) ROBOTRACONTEUR_LOG_LEVEL(info,node,limit,args)

#define ROBOTRACONTEUR_LOG_DEBUG_GLOBAL(args) ROBOTRACONTEUR_LOG_LEVEL(debug,,args)
#define ROBOTRACONTEUR_LOG_DEBUG_DEFAULT(args) ROBOTRACONTEUR_LOG_LEVEL(debug,ROBOTRACONTEUR_LOG_DEFAULT_NODE,args)
#define ROBOTRACONTEUR_LOG_DEBUG(node,args) ROBOTRACONTEUR_LOG_LEVEL(debug,node,args)
#define ROBOTRACONTEUR_LOG_DEBUG_THROTTLE(node,limit,args) ROBOTRACONTEUR_LOG_LEVEL(debug,node,limit,args)

#define ROBOTRACONTEUR_LOG_TRACE_GLOBAL(args) ROBOTRACONTEUR_LOG_LEVEL(trace,,args)
#define ROBOTRACONTEUR_LOG_TRACE_DEFAULT(args) ROBOTRACONTEUR_LOG_LEVEL(trace,ROBOTRACONTEUR_LOG_DEFAULT_NODE,args)
#define ROBOTRACONTEUR_LOG_TRACE(node,args) ROBOTRACONTEUR_LOG_LEVEL(trace,node,args)
#define ROBOTRACONTEUR_LOG_TRACE_THROTTLE(node,limit,args) ROBOTRACONTEUR_LOG_LEVEL(trace,node,limit,args)


namespace detail
{
    std::string rr_get_node_log_ident();

    std::string rr_get_node_log_ident(RR_WEAK_PTR<RobotRaconteur::RobotRaconteurNode> node);
}

}