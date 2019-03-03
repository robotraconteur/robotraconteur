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

%feature("director") RobotRaconteur::AsyncServiceInfo2VectorReturnDirector;
%feature("director") RobotRaconteur::AsyncNodeInfo2VectorReturnDirector;

//Find services
namespace RobotRaconteur
{

	class AsyncServiceInfo2VectorReturnDirector
	{
	public:
		virtual ~AsyncServiceInfo2VectorReturnDirector();
		virtual void handler(const std::vector<RobotRaconteur::ServiceInfo2Wrapped>& ret) {}
	};

	class AsyncNodeInfo2VectorReturnDirector
	{
	public:
		virtual ~AsyncNodeInfo2VectorReturnDirector();
		virtual void handler(const std::vector<RobotRaconteur::NodeInfo2>& ret) {}
	};


	class ServiceInfo2Wrapped
	{
	public:

		std::string Name;
		std::string RootObjectType;
		std::vector<std::string> RootObjectImplements;
		std::vector<std::string> ConnectionURL;
		boost::intrusive_ptr<RobotRaconteur::MessageElement> Attributes;
		RobotRaconteur::NodeID NodeID;
		std::string NodeName;

		//ServiceInfo2Wrapped(const ServiceInfo2& value);

	};

RR_RELEASE_GIL()
	std::vector<RobotRaconteur::ServiceInfo2Wrapped> WrappedFindServiceByType(boost::shared_ptr<RobotRaconteurNode> node, const std::string& servicetype, const std::vector<std::string>& transportschemes);
RR_KEEP_GIL()
		
	void AsyncWrappedFindServiceByType(boost::shared_ptr<RobotRaconteurNode> node, const std::string &servicetype, const std::vector<std::string>& transportschemes, int32_t timeout, AsyncServiceInfo2VectorReturnDirector* handler, int32_t id);
	
}

%template(vectorserviceinfo2wrapped) std::vector<RobotRaconteur::ServiceInfo2Wrapped>;
	
//Find Nodes

namespace RobotRaconteur
{

	class NodeInfo2
	{
	public:		
		RobotRaconteur::NodeID NodeID;
		std::string NodeName;
		std::vector<std::string> ConnectionURL;
	};

RR_RELEASE_GIL()
	std::vector<RobotRaconteur::NodeInfo2> WrappedFindNodeByID(boost::shared_ptr<RobotRaconteurNode> node, NodeID id, const std::vector<std::string>& transportschemes);
	std::vector<RobotRaconteur::NodeInfo2> WrappedFindNodeByName(boost::shared_ptr<RobotRaconteurNode> node, const std::string& name, const std::vector<std::string>& transportschemes);
	
	void AsyncWrappedFindNodeByID(boost::shared_ptr<RobotRaconteurNode> node, const NodeID& id, const std::vector<std::string>& transportschemes, int32_t timeout, AsyncNodeInfo2VectorReturnDirector* handler, int32_t id1);
	void AsyncWrappedFindNodeByName(boost::shared_ptr<RobotRaconteurNode> node, const std::string& name, const std::vector<std::string>& transportschemes, int32_t timeout, AsyncNodeInfo2VectorReturnDirector* handler, int32_t id);

	void WrappedUpdateDetectedNodes(boost::shared_ptr<RobotRaconteurNode> node, const std::vector<std::string>& schemes);

	void AsyncWrappedUpdateDetectedNodes(boost::shared_ptr<RobotRaconteurNode> node, const std::vector<std::string>& schemes, int32_t timeout, AsyncVoidNoErrReturnDirector* handler, int32_t id1);

	std::vector<std::string> WrappedGetDetectedNodes(boost::shared_ptr<RobotRaconteurNode> node);

RR_KEEP_GIL()

}

%template(vectornodeinfo2) std::vector<RobotRaconteur::NodeInfo2>;

