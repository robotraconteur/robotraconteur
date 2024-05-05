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

%shared_ptr(RobotRaconteur::IntraTransport)

namespace RobotRaconteur
{

class IntraTransport : public Transport
{
public:
	IntraTransport();
	IntraTransport(const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node);

	virtual std::string GetUrlSchemeString() const;
	virtual std::vector<std::string> GetServerListenUrls();
	void Close();

	void StartClient();
	void StartServer();
};

}
