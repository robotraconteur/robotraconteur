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

#include "LocalTransport_private.h"

#ifdef ROBOTRACONTEUR_OSX

#include "LocalTransport_darwin_private.h"

#include <CoreServices/CoreServices.h>
#include <CoreFoundation/CoreFoundation.h>

namespace RobotRaconteur
{
namespace detail
{

static void DarwinLocalTransportDiscovery_TimerCallback(CFRunLoopTimerRef timer, void* callbackInfo)
{
    DarwinLocalTransportDiscovery* this_ = (DarwinLocalTransportDiscovery*)callbackInfo;
    this_->Refresh();
}

static void DarwinLocalTransportDiscovery_FSCallback(ConstFSEventStreamRef stream, void* callbackInfo, size_t numEvents,
                                                     void* evPaths, const FSEventStreamEventFlags evFlags[],
                                                     const FSEventStreamEventId evIds[])
{
    DarwinLocalTransportDiscovery* this_ = (DarwinLocalTransportDiscovery*)callbackInfo;
    this_->Refresh();
}

DarwinLocalTransportDiscovery::DarwinLocalTransportDiscovery(const RR_SHARED_PTR<RobotRaconteurNode>& node)
    : LocalTransportDiscovery(node)
{
    runloop = NULL;
    running = false;
}
void DarwinLocalTransportDiscovery::Init()
{
    boost::mutex::scoped_lock lock(runloop_lock);
    running = true;
    RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
    if (node1)
    {
        node_dirs = node1->GetNodeDirectories();
    }
    boost::thread(boost::bind(&DarwinLocalTransportDiscovery::run, shared_from_this()));
}
void DarwinLocalTransportDiscovery::Shutdown()
{
    boost::mutex::scoped_lock lock(runloop_lock);
    running = false;
    if (runloop)
    {
        CFRunLoopStop((CFRunLoopRef)runloop);
    }
}
DarwinLocalTransportDiscovery::~DarwinLocalTransportDiscovery() {}

void DarwinLocalTransportDiscovery::run()
{
    CFRunLoopTimerRef timer = NULL;
    FSEventStreamRef stream = NULL;

    try
    {

        {
            boost::mutex::scoped_lock lock(runloop_lock);
            if (!running)
                return;
            runloop = CFRunLoopGetCurrent();
        }

        std::string private_path = (node_dirs.user_run_dir / "transport" / "local").string();
        std::string public_path = (node_dirs.system_run_dir / "transport" / "local").string();

        CFStringRef cf_paths[2];
        cf_paths[0] = CFStringCreateWithCString(kCFAllocatorDefault, private_path.c_str(), kCFStringEncodingUTF8);
        cf_paths[1] = CFStringCreateWithCString(kCFAllocatorDefault, public_path.c_str(), kCFStringEncodingUTF8);
        if (!cf_paths[0] || !cf_paths[1])
            throw SystemResourceException("");
        CFArrayRef paths = CFArrayCreate(NULL, (const void**)cf_paths, 2, NULL);
        if (!paths)
            throw SystemResourceException("");

        CFAbsoluteTime latency = 3.0;
        FSEventStreamContext streamContext = {0, this, NULL, NULL, NULL};

        stream = FSEventStreamCreate(NULL, &DarwinLocalTransportDiscovery_FSCallback, &streamContext, paths,
                                     kFSEventStreamEventIdSinceNow, latency, kFSEventStreamCreateFlagNone);
        if (!stream)
            throw SystemResourceException("");

        FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        FSEventStreamStart(stream);

        CFTimeInterval timer_interval = 55;
        CFRunLoopTimerContext TimerContext = {0, this, NULL, NULL, NULL};
        CFAbsoluteTime FireTime = CFAbsoluteTimeGetCurrent() + timer_interval;

        timer = CFRunLoopTimerCreate(kCFAllocatorDefault, FireTime, timer_interval, 0, 0,
                                     DarwinLocalTransportDiscovery_TimerCallback, &TimerContext);
        if (!timer)
            throw SystemResourceException("");

        CFRunLoopAddTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopDefaultMode);

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                           "LocalTransport discovery watching private path \"" << private_path << "\"")
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                           "LocalTransport discovery watching public path \"" << public_path << "\"")

        CFRunLoopRun();
    }
    catch (std::exception&)
    {
        // Errors are very unlikely unless the system is unstable. Ignore and clean up.
    }

    {
        boost::mutex::scoped_lock lock(runloop_lock);
        runloop = NULL;
    }

    if (stream)
    {
        FSEventStreamInvalidate(stream);
        FSEventStreamRelease(stream);
    }

    if (timer)
    {
        CFRunLoopTimerInvalidate(timer);
        CFRelease(timer);
    }
}

} // namespace detail

} // namespace RobotRaconteur

#endif
