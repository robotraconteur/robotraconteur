// Copyright 2011-2018 Wason Technology, LLC
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
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RobotRaconteur
{
    public class RobotRaconteurTaskCompletion<T>
    {

        private TaskCompletionSource<T> tc;

        public Task<T> Task
        {
            get
            {
                return tc.Task;
            }
        }

        public RobotRaconteurTaskCompletion()
        {
            tc = new TaskCompletionSource<T>();
        }


        public void handler(T ret, Exception e)
        {
            if (e != null)
            {
                tc.SetException(e);
                return;
            }
            else
            {
                tc.SetResult(ret);
            }
        }

        public void handlerNoErr(T ret)
        {           
            tc.SetResult(ret);            
        }
    }

    public class RobotRaconteurVoidTaskCompletion
    {
        private TaskCompletionSource<object> tc;

        public Task Task
        {
            get
            {
                return tc.Task;
            }
        }

        public RobotRaconteurVoidTaskCompletion()
        {
            tc = new TaskCompletionSource<object>();
        }


        public void handler(Exception e)
        {
            if (e != null)
            {
                tc.SetException(e);
                return;
            }
            else
            {
                tc.SetResult(null);
            }
        }

        public void handlerNoErr()
        {           
            tc.SetResult(null);            
        }

    }
    

    public static class Extensions
    {
        public static Task<object> AsyncConnectService(this RobotRaconteurNode node, string url, string username=null, Dictionary<string, object> credentials=null, RobotRaconteurNode.ClientServiceListenerDelegate listener=null, string objecttype=null, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {

            RobotRaconteurTaskCompletion<object> t = new RobotRaconteurTaskCompletion<object>();
            node.AsyncConnectService(url, username, credentials, listener, objecttype, t.handler, timeout);
            return t.Task;
        }
    
        public static Task<object> AsyncConnectService(this RobotRaconteurNode node, string[] url, string username=null, Dictionary<string, object> credentials=null, RobotRaconteurNode.ClientServiceListenerDelegate listener=null, string objecttype=null, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            RobotRaconteurTaskCompletion<object> t = new RobotRaconteurTaskCompletion<object>();
            node.AsyncConnectService(url, username, credentials, listener, objecttype, t.handler, timeout);
            return t.Task;
        }

        public static Task AsyncDisconnectService(this RobotRaconteurNode node, object obj)
        {
            RobotRaconteurVoidTaskCompletion t=new RobotRaconteurVoidTaskCompletion();
            node.AsyncDisconnectService(obj, t.handlerNoErr);
            return t.Task;
        }

        public static Task<object> AsyncFindObjRefTyped(this RobotRaconteurNode node, object obj, string objref, string objecttype, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            RobotRaconteurTaskCompletion<object> t = new RobotRaconteurTaskCompletion<object>();
            node.AsyncFindObjRefTyped(obj, objref, objecttype, t.handler, timeout);
            return t.Task;
        }

        public static Task<object> AsyncFindObjRefTyped(this RobotRaconteurNode node, object obj, string objref, string index, string objecttype, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            RobotRaconteurTaskCompletion<object> t = new RobotRaconteurTaskCompletion<object>();
            node.AsyncFindObjRefTyped(obj, objref, index, objecttype, t.handler, timeout);
            return t.Task;
        }

        public static Task<ServiceInfo2[]> AsyncFindServiceByType(this RobotRaconteurNode node, string servicetype, string[] transportschemes, int timeout = 5000)
        {
            RobotRaconteurTaskCompletion<ServiceInfo2[]> t = new RobotRaconteurTaskCompletion<ServiceInfo2[]>();
            node.AsyncFindServiceByType(servicetype, transportschemes, t.handlerNoErr, timeout);
            return t.Task;
        }

        public static Task<NodeInfo2[]> AsyncFindNodeByID(this RobotRaconteurNode node, NodeID id, string[] transportschemes, int timeout = 5000)
        {
            RobotRaconteurTaskCompletion<NodeInfo2[]> t = new RobotRaconteurTaskCompletion<NodeInfo2[]>();
            node.AsyncFindNodeByID(id, transportschemes, t.handlerNoErr, timeout);
            return t.Task;
        }

        public static Task<NodeInfo2[]> AsyncFindNodeByName(this RobotRaconteurNode node, string name, string[] transportschemes, int timeout = 5000)
        {
            RobotRaconteurTaskCompletion<NodeInfo2[]> t = new RobotRaconteurTaskCompletion<NodeInfo2[]>();
            node.AsyncFindNodeByName(name, transportschemes, t.handlerNoErr, timeout);
            return t.Task;
        }

        public static Task<string> AsyncFindObjectType(this RobotRaconteurNode node, object obj, string objref, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            RobotRaconteurTaskCompletion<string> t = new RobotRaconteurTaskCompletion<string>();
            node.AsyncFindObjectType(obj, objref, t.handler, timeout);
            return t.Task;
        }

        public static Task<string> AsyncFindObjectType(this RobotRaconteurNode node, object obj, string objref, string index, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            RobotRaconteurTaskCompletion<string> t = new RobotRaconteurTaskCompletion<string>();
            node.AsyncFindObjectType(obj, objref, index, t.handler, timeout);
            return t.Task;
        }

        public static Task<string> AsyncRequestObjectLock(this RobotRaconteurNode node, object obj, RobotRaconteurObjectLockFlags flags, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            RobotRaconteurTaskCompletion<string> t = new RobotRaconteurTaskCompletion<string>();
            node.AsyncRequestObjectLock(obj, flags, t.handler, timeout);
            return t.Task;
        }

        public static Task<string> AsyncReleaseObjectLock(this RobotRaconteurNode node, object obj, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            RobotRaconteurTaskCompletion<string> t = new RobotRaconteurTaskCompletion<string>();
            node.AsyncReleaseObjectLock(obj, t.handler, timeout);
            return t.Task;
        }

        public static Task<Pipe<T>.PipeEndpoint> AsyncConnect<T>(this Pipe<T> pipe, int index, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            RobotRaconteurTaskCompletion<Pipe<T>.PipeEndpoint> t = new RobotRaconteurTaskCompletion<Pipe<T>.PipeEndpoint>();
            pipe.AsyncConnect(index, t.handler, timeout);
            return t.Task;
        }

        public static Task<uint> AsyncSendPacket<T>(this Pipe<T>.PipeEndpoint ep, T packet)
        {
            RobotRaconteurTaskCompletion<uint> t = new RobotRaconteurTaskCompletion<uint>();
            ep.AsyncSendPacket(packet, t.handler);
            return t.Task;
        }

        public static Task AsyncClose<T>(this Pipe<T>.PipeEndpoint ep, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            RobotRaconteurVoidTaskCompletion t = new RobotRaconteurVoidTaskCompletion();
            ep.AsyncClose(t.handler, timeout);
            return t.Task;
        }

        public static Task AsyncSendPacket<T>(this PipeBroadcaster<T> pipe, T packet)
        {
            RobotRaconteurVoidTaskCompletion t = new RobotRaconteurVoidTaskCompletion();
            pipe.AsyncSendPacket(packet, t.handlerNoErr);
            return t.Task;
        }

        public static Task<Wire<T>.WireConnection> AsyncConnect<T>(this Wire<T> wire, int index, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            RobotRaconteurTaskCompletion<Wire<T>.WireConnection> t = new RobotRaconteurTaskCompletion<Wire<T>.WireConnection>();
            wire.AsyncConnect(t.handler, timeout);
            return t.Task;
        }

        public static Task AsyncClose<T>(this Wire<T>.WireConnection ep, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            RobotRaconteurVoidTaskCompletion t = new RobotRaconteurVoidTaskCompletion();
            ep.AsyncClose(t.handler, timeout);
            return t.Task;
        }
    }
}
