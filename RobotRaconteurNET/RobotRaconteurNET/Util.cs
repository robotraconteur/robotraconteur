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

class WrappedRobotRaconteurExceptionHelper : RobotRaconteurExceptionHelper
{

    public WrappedRobotRaconteurExceptionHelper() : base()
    {}

    public override void SetRobotRaconteurException(HandlerErrorInfo error)
    {
        using (MessageEntry e = new MessageEntry())
        {
            Exception ex = RobotRaconteurExceptionUtil.ErrorInfoToException(error);
            RobotRaconteurNETPINVOKE.SWIGPendingException.Set(ex);
        }
    }
}

/// <summary>
/// Custom LogRecordHandler that sends log records to a callback function
/// </summary>
public class UserLogRecordHandler : UserLogRecordHandlerBase
{
    class UserLogRecordHandlerDirectorNET : UserLogRecordHandlerDirector
    {
        public UserLogRecordHandlerDirectorNET(Action<RRLogRecord> handler)
        {
            this.handler = handler;
        }

        Action<RRLogRecord> handler;
        public override void HandleLogRecord(RRLogRecord record)
        {
            handler?.Invoke(record);
        }
    }

    /// <summary>
    /// Construct a UserLogRecordHandler with a specified callback. Use RobotRaconteurNode.SetRecordLogRecordHandler()
    /// to set the record handler.
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="handler">The callback to receive log records</param>
    public UserLogRecordHandler(Action<RRLogRecord> handler)
    {
        UserLogRecordHandlerDirectorNET director = new UserLogRecordHandlerDirectorNET(handler);
        int id = RRObjectHeap.AddObject(director);
        _SetHandler(director, id);
    }
}

public class RRObjectHeap
{

    class RRObjectHeap_support : RRNativeObjectHeapSupport
    {
        public override void DeleteObject_i(int id)
        {
            RRObjectHeap.DeleteObject(id);
        }
    }

    static RRObjectHeap_support support;

    static RRObjectHeap()
    {
        support = new RRObjectHeap_support();
        RRNativeObjectHeapSupport.Set_Support(support);
    }

    public static Dictionary<int, object> objects = new Dictionary<int, object>();
    public static int objectcount = 0;

    public static int AddObject(object o)
    {
        lock (objects)
        {
            if (objectcount > int.MaxValue - 10)
                objectcount = 0;
            objectcount++;
            while (objects.ContainsKey(objectcount))
                objectcount++;

            int i = objectcount;
            objects.Add(i, o);
            // Console.WriteLine(objects.Count);
            return i;
        }
    }

    public static void DeleteObject(int id)
    {
        lock (objects)
        {
            try
            {
                objects.Remove(id);
                // Console.WriteLine(objects.Count);
            }
            catch
            {

                // Console.WriteLine(e.ToString());
            }
        }
    }

    public static object GetObject(int id)
    {
        try
        {
            return objects[id];
        }
        catch (KeyNotFoundException)
        {
            throw new ApplicationException("Internal stub error");
        }
    }
}

}
