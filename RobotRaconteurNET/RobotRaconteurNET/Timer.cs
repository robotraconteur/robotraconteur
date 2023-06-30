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

using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.IO;
using System.Reflection;
using System.Linq;
using System.Threading.Tasks;

namespace RobotRaconteur
{

public partial class WallTimer
{

    public WallTimer(int period, Action<TimerEvent> handler, bool oneshot, RobotRaconteurNode node = null)
        : this(period, oneshot, node, AttachHandler(handler))
    {
        if (node == null)
            node = RobotRaconteurNode.s;

        if (RobotRaconteurNETPINVOKE.SWIGPendingException.Pending)
            throw RobotRaconteurNETPINVOKE.SWIGPendingException.Retrieve();
    }

    private static WallTimer_initstruct AttachHandler(Action<TimerEvent> handler)
    {
        WallTimer_initstruct s = new WallTimer_initstruct();
        AsyncTimerEventReturnDirectorImpl h = new AsyncTimerEventReturnDirectorImpl(handler);
        s.handler = h;
        s.id = RRObjectHeap.AddObject(h);
        return s;
    }
}

}