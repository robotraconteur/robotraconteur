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

%shared_ptr(RobotRaconteur::TcpTransport)

namespace RobotRaconteur
{

enum IPNodeDiscoveryFlags
{
	IPNodeDiscoveryFlags_NODE_LOCAL = 0x1,
	IPNodeDiscoveryFlags_LINK_LOCAL = 0x2,
	IPNodeDiscoveryFlags_SITE_LOCAL = 0x4,
	IPNodeDiscoveryFlags_IPV4_BROADCAST = 0x8
};

class TcpTransport : public Transport
{
public:

	TcpTransport();
	
	TcpTransport(const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node);
	
	RR_PROPERTY(DefaultReceiveTimeout)
	int32_t GetDefaultReceiveTimeout();	
	void SetDefaultReceiveTimeout(int32_t milliseconds);
	
	RR_PROPERTY(DefaultConnectTimeout)
	int32_t GetDefaultConnectTimeout();
	void SetDefaultConnectTimeout(int32_t milliseconds);
	
	RR_PROPERTY(DefaultHeartbeatPeriod)
	int32_t GetDefaultHeartbeatPeriod();	
	void SetDefaultHeartbeatPeriod(int32_t milliseconds);
	
	virtual std::string GetUrlSchemeString() const;
	virtual std::vector<std::string> GetServerListenUrls();
	virtual int32_t GetListenPort();
	virtual void StartServer(int32_t porte, bool localhost_only = false);
	void Close();
	void EnableNodeDiscoveryListening(uint32_t flags=(IPNodeDiscoveryFlags_LINK_LOCAL));
	void DisableNodeDiscoveryListening();
	void EnableNodeAnnounce(uint32_t flags=IPNodeDiscoveryFlags_LINK_LOCAL);
	void DisableNodeAnnounce();

	RR_PROPERTY(NodeAnnouncePeriod)
	int32_t GetNodeAnnouncePeriod();
	void SetNodeAnnouncePeriod(int32_t millis);
	
	RR_PROPERTY(MaxMessageSize)
	int32_t GetMaxMessageSize();	
	void SetMaxMessageSize(int32_t size);
	
	RR_PROPERTY(MaxConnectionCount)
	int32_t GetMaxConnectionCount();	
	void SetMaxConnectionCount(int32_t count);
	
	RR_PROPERTY(RequireTls)
	virtual bool GetRequireTls();	
	virtual void SetRequireTls(bool require_tls);
	
	virtual void LoadTlsNodeCertificate();
	RR_MAKE_METHOD_PRIVATE(IsTlsNodeCertificateLoaded)	
	virtual bool IsTlsNodeCertificateLoaded();
	
	
	virtual bool IsTransportConnectionSecure(uint32_t endpoint);
	RR_MAKE_METHOD_PRIVATE(IsTransportConnectionSecure)	
	virtual bool IsTransportConnectionSecure(const boost::shared_ptr<RobotRaconteur::RRObject>& obj);
		
	virtual bool IsSecurePeerIdentityVerified(uint32_t endpoint);
	RR_MAKE_METHOD_PRIVATE(IsSecurePeerIdentityVerified)	
	virtual bool IsSecurePeerIdentityVerified(const boost::shared_ptr<RobotRaconteur::RRObject>& obj);
	
	virtual std::string GetSecurePeerIdentity(uint32_t endpoint);
	RR_MAKE_METHOD_PRIVATE(GetSecurePeerIdentity)	
	virtual std::string GetSecurePeerIdentity(const boost::shared_ptr<RobotRaconteur::RRObject>& obj);
	
	virtual void StartServerUsingPortSharer();
	RR_MAKE_METHOD_PRIVATE(IsPortSharerRunning)	
	virtual bool IsPortSharerRunning();
	
	RR_PROPERTY(AcceptWebSockets)
	virtual bool GetAcceptWebSockets();	
	virtual void SetAcceptWebSockets(bool value);

	virtual std::vector<std::string> GetWebSocketAllowedOrigins();
	virtual void AddWebSocketAllowedOrigin(const std::string& origin);
	virtual void RemoveWebSocketAllowedOrigin(const std::string& origin);
	
	RR_PROPERTY(DisableMessage4)
	virtual bool GetDisableMessage4();
	virtual void SetDisableMessage4(bool d);

	RR_PROPERTY(DisableStringTable)
	virtual bool GetDisableStringTable();
	virtual void SetDisableStringTable(bool d);

	RR_PROPERTY(DisableAsyncMessageIO)
	virtual bool GetDisableAsyncMessageIO();
	virtual void SetDisableAsyncMessageIO(bool d);

%extend {
	static std::vector<std::string> GetLocalAdapterIPAddresses()
	{
		std::vector<std::string> o;
		std::vector<boost::asio::ip::address> addr;
		RobotRaconteur::TcpTransport::GetLocalAdapterIPAddresses(addr);
		for (size_t i=0; i<addr.size(); i++)
		{
			o.push_back(boost::lexical_cast<std::string>(addr[i]));
		}
		return o;
	}
}
	
};
}