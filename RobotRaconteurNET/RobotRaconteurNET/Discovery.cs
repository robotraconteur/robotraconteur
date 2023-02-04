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

/// <summary>
/// Contains information about a service found using discovery
/// </summary>
/// <remarks>
/// <para>
/// ServiceInfo2 contains information about a service required to
/// connect to the service, metadata, and the service attributes
/// </para>
/// <para>
/// ServiceInfo2 structures are returned by RobotRaconteurNode::FindServiceByType()
/// and ServiceInfo2Subscription
/// </para>
/// </remarks>
public class ServiceInfo2
{
    /// <summary>
    /// The name of the service
    /// </summary>
    /// <remarks>None</remarks>
    public string Name;
    /// <summary>
    /// The fully qualified type of the root object in the service
    /// </summary>
    /// <remarks>None</remarks>
    public string RootObjectType;
    /// <summary>
    /// The fully qualified types the root object implements
    /// </summary>
    /// <remarks>None</remarks>
    public string[] RootObjectImplements;
    /// <summary>
    /// Candidate URLs to connect to the service
    /// </summary>
    /// <remarks>None</remarks>
    public string[] ConnectionURL;
    /// <summary>
    /// Service attributes
    /// </summary>
    /// <remarks>None</remarks>
    public Dictionary<string, object> Attributes;
    /// <summary>
    /// The NodeID of the node that owns the service
    /// </summary>
    /// <remarks>None</remarks>
    public NodeID NodeID;
    /// <summary>
    /// The NodeName of the node that owns the service
    /// </summary>
    /// <remarks>None</remarks>
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

/// <summary>
/// Contains information about a node detected using discovery
/// </summary>
/// <remarks>
/// <para>
/// NodeInfo2 contains information about a node detected using discovery.
/// Node information is typically not verified, and is used as a first
/// step to detect available services.
/// </para>
/// <para>
/// NodeInfo2 structures are returned by RobotRaconteurNode.FindNodeByName()
/// and RobotRaconteurNode.FindNodeByID()
/// </para>
/// </remarks>
public class NodeInfo2
{
    /// <summary>
    /// The NodeID of the detected node
    /// </summary>
    /// <remarks>None</remarks>
    public NodeID NodeID;
    /// <summary>
    /// The NodeName of the detected node
    /// </summary>
    /// <remarks>None</remarks>
    public string NodeName;
    /// <summary>
    /// Candidate URLs to connect to the node
    /// </summary>
    /// <remarks>
    /// The URLs for the node typically contain the node transport endpoint
    /// and the nodeid. A URL service parameter must be appended
    /// to connect to a service.
    /// </remarks>
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