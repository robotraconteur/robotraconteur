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

#include "RobotRaconteur/DataTypes.h"

#pragma once

namespace RobotRaconteurServiceIndex
{
	class ROBOTRACONTEUR_CORE_API ServiceInfo;

	class ROBOTRACONTEUR_CORE_API NodeInfo;
}


namespace RobotRaconteur
{

	class ROBOTRACONTEUR_CORE_API ServiceInfo2
	{
	public:

		ServiceInfo2();

		std::string Name;
		std::string RootObjectType;
		std::vector<std::string> RootObjectImplements;
		std::vector<std::string> ConnectionURL;
		std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > Attributes;
		RobotRaconteur::NodeID NodeID;
		std::string NodeName;

		ServiceInfo2(const RobotRaconteurServiceIndex::ServiceInfo &info, const RobotRaconteurServiceIndex::NodeInfo &ninfo);

	};

	class ROBOTRACONTEUR_CORE_API NodeInfo2
	{
	public:
		RobotRaconteur::NodeID NodeID;
		std::string NodeName;
		std::vector<std::string> ConnectionURL;
	};

	class ROBOTRACONTEUR_CORE_API NodeDiscoveryInfoURL
	{
	public:
		std::string URL;
		boost::posix_time::ptime LastAnnounceTime;
	};

	class ROBOTRACONTEUR_CORE_API NodeDiscoveryInfo
	{
	public:

		RobotRaconteur::NodeID NodeID;
		std::string NodeName;
		std::vector<NodeDiscoveryInfoURL> URLs;
		std::string ServiceStateNonce;
	};
}
