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

#include "RobotRaconteur/AutoResetEvent.h"

#ifdef ROBOTRACONTEUR_WINDOWS
#include <Windows.h>
#endif

namespace RobotRaconteur
{

#ifdef ROBOTRACONTEUR_WINDOWS
AutoResetEvent::AutoResetEvent()
{

    HANDLE* evp = new HANDLE[1];

    evp[0] = ::CreateEventW(NULL, false, false, NULL);
    if (!evp[0])
        throw InternalErrorException("Internal error creating event");

    ev = (void*)evp;
}

AutoResetEvent::~AutoResetEvent()
{
    ::CloseHandle(*((HANDLE*)ev));
    HANDLE* evp = (HANDLE*)ev;
    delete[](evp);
}

void AutoResetEvent::Set()
{
    if (!::SetEvent(*((HANDLE*)ev)))
    {
        throw InternalErrorException("Internal error setting event");
    }
}

void AutoResetEvent::Reset()
{
    if (!::ResetEvent(*((HANDLE*)ev)))
    {
        throw InternalErrorException("Internal error setting event");
    }
}

void AutoResetEvent::WaitOne()
{
    DWORD waitres = ::WaitForSingleObject(*((HANDLE*)ev), INFINITE);

    if (waitres == WAIT_OBJECT_0 || waitres == WAIT_ABANDONED)
        return;

    throw InternalErrorException("Internal error waiting for event");
}

bool AutoResetEvent::WaitOne(int32_t timeout)
{
    DWORD waitres = ::WaitForSingleObject(*((HANDLE*)ev), timeout);

    if (waitres == WAIT_OBJECT_0)
        return true;

    if (waitres == WAIT_TIMEOUT || waitres == WAIT_ABANDONED)
        return false;

    throw InternalErrorException("Internal error waiting for event");
}

#else

AutoResetEvent::AutoResetEvent() { m_bSet = false; }

AutoResetEvent::~AutoResetEvent() {}

void AutoResetEvent::Set()
{
    {
        boost::unique_lock<boost::mutex> lock(m_mutex);
        m_bSet = true;

        m_setCondition.notify_one();
    }
}

void AutoResetEvent::Reset()
{
    boost::unique_lock<boost::mutex> lock(m_mutex);
    m_bSet = false;
}

void AutoResetEvent::WaitOne()
{
    boost::unique_lock<boost::mutex> lock(m_mutex);
    while (!m_bSet)
    {
        m_setCondition.wait(lock);
    }

    m_bSet = false;
}

bool AutoResetEvent::WaitOne(int32_t timeout)
{

    boost::system_time const timeout1 =
        boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(timeout);
    while (!m_bSet)
    {
        boost::unique_lock<boost::mutex> lock(m_mutex);
        m_setCondition.timed_wait(lock, timeout1);
        if (!m_bSet && boost::posix_time::microsec_clock::universal_time() > timeout1)
            return false;
    }

    m_bSet = false;
    return true;
}
#endif
} // namespace RobotRaconteur
