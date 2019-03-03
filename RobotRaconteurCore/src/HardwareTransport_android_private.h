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

#include "RobotRaconteur/HardwareTransport.h"
#include "ASIOStreamBaseTransport.h"

//#include "HardwareTransport_libusb_private.h"
#include "HardwareTransport_bluetoothcommon_private.h"
#include "HardwareTransport_discoverycommon_private.h"

#pragma once

namespace RobotRaconteur
{
namespace detail
{

class AndroidBluetoothConnector_params
{
public:
	RR_SHARED_PTR<HardwareTransport> parent;
	NodeID target_nodeid;
	std::string target_nodename;
	std::string noden;

	uint32_t endpoint;

	boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler;
};

class AndroidHardwareDirector
{
public:

	static void SetDirector(AndroidHardwareDirector* d);
	static void ClearDirector(AndroidHardwareDirector* d);


	AndroidHardwareDirector();
	virtual ~AndroidHardwareDirector();

	static void ConnectBluetooth(RR_SHARED_PTR<HardwareTransport> parent, const ParseConnectionURLResult& url, const std::string& noden, uint32_t endpoint, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler);

protected:

	static boost::mutex director_lock;
	static AndroidHardwareDirector* director;

	virtual void ConnectBluetooth2(void* p) = 0;

};

class AndroidHardwareHelper
{
public:

	static int32_t ConnectBluetooth_success(RR_SHARED_PTR<AndroidBluetoothConnector_params> p);
	static void ConnectBluetooth_error(RR_SHARED_PTR<AndroidBluetoothConnector_params> p, const std::string& message);
	static RR_SHARED_PTR<AndroidBluetoothConnector_params> VoidToAndroidBluetoothConnector_params(void* p);
	static std::vector<int8_t> MessageToVector(RR_INTRUSIVE_PTR<Message> m);
	static RR_INTRUSIVE_PTR<Message> VectorToMessage(std::vector<int8_t> v);
};

}
}
