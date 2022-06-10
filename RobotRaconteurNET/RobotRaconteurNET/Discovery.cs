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

public class ServiceInfo2
{
    public string Name;
    public string RootObjectType;
    public string[] RootObjectImplements;
    public string[] ConnectionURL;
    public Dictionary<string, object> Attributes;
    public NodeID NodeID;
    public string NodeName;

    public ServiceInfo2()
    {}

    public ServiceInfo2(ServiceInfo2Wrapped s)
    {
        this.Name = s.Name;
        this.NodeID = new NodeID(s.NodeID.ToString());
        this.NodeName = s.NodeName;
        this.RootObjectType = s.RootObjectType;
        List<string> impl = new List<string>();
        foreach (string impl2 in s.RootObjectImplements)
            impl.Add(impl2);
        this.RootObjectImplements = impl.ToArray();
        this.Attributes = (Dictionary<string, object>)RobotRaconteurNode.s.UnpackVarType(s.Attributes);
        if (s.Attributes != null)
            s.Attributes.Dispose();
        List<string> con = new List<string>();
        foreach (string con2 in s.ConnectionURL)
            con.Add(con2);
        this.ConnectionURL = con.ToArray();
    }
}

public class NodeInfo2
{
    public NodeID NodeID;
    public string NodeName;
    public string[] ConnectionURL;

    public NodeInfo2()
    {}

    public NodeInfo2(WrappedNodeInfo2 n)
    {
        this.NodeID = new NodeID(n.NodeID.ToString());
        this.NodeName = n.NodeName;
        List<string> con = new List<string>();
        foreach (string con2 in n.ConnectionURL)
            con.Add(con2);
        this.ConnectionURL = con.ToArray();
    }
}

}