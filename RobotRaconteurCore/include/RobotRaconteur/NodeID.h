/** 
 * @file NodeID.h
 * 
 * @author Dr. John Wason
 * 
 * @copyright Copyright 2011-2020 Wason Technology, LLC
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * @par
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "RobotRaconteur/RobotRaconteurConstants.h"
#include "RobotRaconteur/DataTypes.h"
#include <boost/array.hpp>
#include <boost/uuid/uuid.hpp>

namespace RobotRaconteur
{
	
	class ROBOTRACONTEUR_CORE_API NodeID : virtual public RRObject
	{
	private:
		boost::uuids::uuid id;

	public:

		NodeID();

		NodeID(boost::array<uint8_t,16> id);

		NodeID(boost::uuids::uuid id);

		NodeID(const NodeID& id);

		NodeID(boost::string_ref id);

		const boost::array<uint8_t,16> ToByteArray() const;

		const boost::uuids::uuid ToUuid() const;

		virtual std::string ToString() const;

		virtual std::string ToString(boost::string_ref format) const;

		
		static NodeID NewUniqueID();

		bool operator == (const NodeID &id2) const;

		bool operator != (const NodeID &id2) const;

		bool operator <(const NodeID& id2) const;

		NodeID& operator =(const NodeID& id);

		bool IsAnyNode() const;
		
		static NodeID GetAny(); 

		virtual int32_t GetHashCode() const;

		virtual std::string RRType() { return "RobotRaconteur::NodeID";}

	private:
		void init_from_string(const boost::string_ref& str);
	
	};



}