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

#pragma once

#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/ServiceDefinition.h"

namespace RobotRaconteur
{

	class ROBOTRACONTEUR_CORE_API StructureStub;
	class ROBOTRACONTEUR_CORE_API ServiceStub;
	class ROBOTRACONTEUR_CORE_API ServiceSkel;

	class ROBOTRACONTEUR_CORE_API ClientContext;
	class ROBOTRACONTEUR_CORE_API ServerContext;

	class ROBOTRACONTEUR_CORE_API RobotRaconteurNode;
	
	class ROBOTRACONTEUR_CORE_API ServiceFactory
	{

	public:

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		void SetNode(RR_SHARED_PTR<RobotRaconteurNode> node);

		virtual ~ServiceFactory() {}

		virtual std::string GetServiceName()=0;

		virtual std::string DefString()=0;

		virtual RR_SHARED_PTR<StructureStub> FindStructureStub(const std::string& s)=0;
		
		virtual RR_INTRUSIVE_PTR<MessageElementStructure> PackStructure(RR_INTRUSIVE_PTR<RRStructure> structin)=0;
				
		virtual RR_INTRUSIVE_PTR<RRValue> UnpackStructure(RR_INTRUSIVE_PTR<MessageElementStructure> mstructin)=0; 

		virtual RR_INTRUSIVE_PTR<MessageElementPodArray> PackPodArray(RR_INTRUSIVE_PTR<RRPodBaseArray> structure)=0;

		virtual RR_INTRUSIVE_PTR<RRPodBaseArray> UnpackPodArray(RR_INTRUSIVE_PTR<MessageElementPodArray> structure)=0;

		virtual RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> PackPodMultiDimArray(RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> structure)=0;

		virtual RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> UnpackPodMultiDimArray(RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> structure)=0;

		virtual RR_INTRUSIVE_PTR<MessageElementNamedArray> PackNamedArray(RR_INTRUSIVE_PTR<RRNamedBaseArray> structure) = 0;

		virtual RR_INTRUSIVE_PTR<RRNamedBaseArray> UnpackNamedArray(RR_INTRUSIVE_PTR<MessageElementNamedArray> structure) = 0;

		virtual RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> PackNamedMultiDimArray(RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> structure) = 0;

		virtual RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> UnpackNamedMultiDimArray(RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> structure) = 0;

		virtual RR_SHARED_PTR<ServiceStub> CreateStub(const std::string& objecttype, const std::string& path, RR_SHARED_PTR<ClientContext> context)=0;

		virtual RR_SHARED_PTR<ServiceSkel> CreateSkel(const std::string& objecttype, const std::string& path, RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<ServerContext> context)=0;

		virtual RR_SHARED_PTR<ServiceDefinition> ServiceDef();
		
		virtual std::string RemovePath(const std::string &path);

		virtual void DownCastAndThrowException(RobotRaconteurException& exp)=0;

		virtual RR_SHARED_PTR<RobotRaconteurException> DownCastException(RR_SHARED_PTR<RobotRaconteurException> exp)=0;

	private:
		RR_SHARED_PTR<ServiceDefinition> sdef;
		RR_WEAK_PTR<RobotRaconteurNode> node;
	};

	class ROBOTRACONTEUR_CORE_API DynamicServiceFactory
	{
	public:

		virtual ~DynamicServiceFactory() {}

		virtual RR_SHARED_PTR<ServiceFactory> CreateServiceFactory(const std::string& def) = 0;

		virtual std::vector<RR_SHARED_PTR<ServiceFactory> > CreateServiceFactories(const std::vector<std::string>& def) = 0;
	};

}
