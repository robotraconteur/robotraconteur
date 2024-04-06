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

//Subscriptions

%shared_ptr(RobotRaconteur::WrappedServiceSubscriptionFilterNode);
%shared_ptr(RobotRaconteur::WrappedServiceSubscriptionFilter);
%shared_ptr(RobotRaconteur::WrappedServiceInfo2Subscription);
%shared_ptr(RobotRaconteur::WrappedServiceSubscription);
%shared_ptr(RobotRaconteur::WrappedPipeSubscription);
%shared_ptr(RobotRaconteur::WrappedWireSubscription);
%shared_ptr(RobotRaconteur::WrappedSubObjectSubscription);
%shared_ptr(RobotRaconteur::WrappedServiceSubscriptionManager)

%rename (WrappedServiceSubscriptionClientID) ServiceSubscriptionClientID;

%template(map_subscriptionserviceinfo2) std::map<RobotRaconteur::ServiceSubscriptionClientID, RobotRaconteur::ServiceInfo2Wrapped>;
%template(map_subscriptionclients) std::map<RobotRaconteur::ServiceSubscriptionClientID, boost::shared_ptr<RobotRaconteur::WrappedServiceStub> >;
%template(vectorptr_wrappedservicesubscriptionnode) std::vector<boost::shared_ptr<RobotRaconteur::WrappedServiceSubscriptionFilterNode> >;
%template(vectorptr_subscriptionclientid) std::vector<RobotRaconteur::ServiceSubscriptionClientID>;
%template(vector_wrappedservicesubscriptionmanagerdetails) std::vector<RobotRaconteur::WrappedServiceSubscriptionManagerDetails>;

%feature("director") RobotRaconteur::WrappedServiceInfo2SubscriptionDirector;
%feature("director") RobotRaconteur::WrappedServiceSubscriptionDirector;
%feature("director") RobotRaconteur::WrappedServiceSubscriptionFilterPredicateDirector;
%feature("director") RobotRaconteur::WrappedWireSubscriptionDirector;
%feature("director") RobotRaconteur::WrappedPipeSubscriptionDirector;



namespace RobotRaconteur
{

	class TimeSpec;
	class ServiceInfo2;

	class ServiceSubscriptionFilterAttribute
	{
		public:

			std::string Name;
			std::string Value;
			// boost::regex ValueRegex;
			bool UseRegex;

			ServiceSubscriptionFilterAttribute(const std::string& value);
			ServiceSubscriptionFilterAttribute(const std::string& name, const std::string& value);

			bool IsMatch(const std::string& value) const;
			bool IsMatch(const std::string& name, const std::string& value) const;
			bool IsMatch(const std::vector<std::string>& values) const;
			bool IsMatch(const std::map<std::string, std::string>& values) const;
	};

	ServiceSubscriptionFilterAttribute CreateServiceSubscriptionFilterAttributeRegex(const std::string& regex_value);
	ServiceSubscriptionFilterAttribute CreateServiceSubscriptionFilterAttributeRegex(const std::string& name, const std::string& regex_value);

	ServiceSubscriptionFilterAttribute
	CreateServiceSubscriptionFilterAttributeIdentifier(const std::string& identifier_name, const std::string& uuid_string);
	ServiceSubscriptionFilterAttribute
	CreateServiceSubscriptionFilterAttributeIdentifier(const std::string& name, const std::string& identifier_name, const std::string& uuid_string);
	ServiceSubscriptionFilterAttribute
	CreateServiceSubscriptionFilterAttributeCombinedIdentifier(const std::string& combined_identifier);
}

%template(vector_subscriptionattribute)  std::vector<RobotRaconteur::ServiceSubscriptionFilterAttribute>;
%template(map_subscriptionattribute)  std::map<std::string,RobotRaconteur::ServiceSubscriptionFilterAttribute>;

namespace RobotRaconteur
{

	enum ServiceSubscriptionFilterAttributeGroupOperation
	{
		ServiceSubscriptionFilterAttributeGroupOperation_OR,
		ServiceSubscriptionFilterAttributeGroupOperation_AND,
		ServiceSubscriptionFilterAttributeGroupOperation_NOR,
		ServiceSubscriptionFilterAttributeGroupOperation_NAND
	};


	class ServiceSubscriptionFilterAttributeGroup
	{
		public:
			std::vector<ServiceSubscriptionFilterAttribute> Attributes;
			std::vector<ServiceSubscriptionFilterAttributeGroup> Groups;
			ServiceSubscriptionFilterAttributeGroupOperation Operation;
			bool SplitStringAttribute;
        	char SplitStringDelimiter;

			ServiceSubscriptionFilterAttributeGroup();
			ServiceSubscriptionFilterAttributeGroup(ServiceSubscriptionFilterAttributeGroupOperation operation);
			ServiceSubscriptionFilterAttributeGroup(ServiceSubscriptionFilterAttributeGroupOperation operation, std::vector<ServiceSubscriptionFilterAttribute> attributes);
			ServiceSubscriptionFilterAttributeGroup(ServiceSubscriptionFilterAttributeGroupOperation operation, std::vector<ServiceSubscriptionFilterAttributeGroup> groups);

			bool IsMatch(const std::string& value) const;
			bool IsMatch(const std::vector<std::string>& values) const;
			bool IsMatch(const std::map<std::string, std::string>& values) const;
	};
}

%template(map_subscriptionattributegroup)  std::map<std::string,RobotRaconteur::ServiceSubscriptionFilterAttributeGroup>;

namespace RobotRaconteur
{

	class WrappedServiceSubscriptionFilterPredicateDirector
	{
	public:
		virtual bool Predicate(const ServiceInfo2Wrapped& info) = 0;

		virtual ~WrappedServiceSubscriptionFilterPredicateDirector() {}
	};

	class WrappedServiceSubscriptionFilterNode
	{
	public:
		::RobotRaconteur::NodeID NodeID;
		std::string NodeName;
		std::string Username;
		boost::intrusive_ptr<MessageElementData> Credentials;
	};

	class WrappedServiceSubscriptionFilter
	{
	public:
		std::vector<boost::shared_ptr<WrappedServiceSubscriptionFilterNode> > Nodes;
		std::vector<std::string> ServiceNames;
		std::vector<std::string> TransportSchemes;
		//boost::shared_ptr<WrappedServiceSubscriptionFilterPredicateDirector> Predicate;
		std::map<std::string,ServiceSubscriptionFilterAttributeGroup> Attributes;
		ServiceSubscriptionFilterAttributeGroupOperation AttributesMatchOperation;
		void SetRRPredicateDirector(WrappedServiceSubscriptionFilterPredicateDirector* director, int32_t id);
		uint32_t MaxConnections;
	};

	%nodefaultctor ServiceSubscriptionClientID;
	class ServiceSubscriptionClientID
	{
	public:

		%rename(eq) operator== ;
		%rename(ne) operator!=;

		::RobotRaconteur::NodeID NodeID;
		std::string ServiceName;

		bool operator == (const ServiceSubscriptionClientID &id2) const;

		bool operator != (const ServiceSubscriptionClientID &id2) const;
	};

	class WrappedServiceInfo2SubscriptionDirector
	{
	public:
		virtual void ServiceDetected(const boost::shared_ptr<RobotRaconteur::WrappedServiceInfo2Subscription>& subscription, const ServiceSubscriptionClientID& id, const ServiceInfo2Wrapped& service) = 0;
		virtual void ServiceLost(const boost::shared_ptr<RobotRaconteur::WrappedServiceInfo2Subscription>& subscription, const ServiceSubscriptionClientID& id, const ServiceInfo2Wrapped& service)  = 0;

		virtual ~WrappedServiceInfo2SubscriptionDirector() {}
	};

	%nodefaultctor WrappedServiceInfo2Subscription;
	class WrappedServiceInfo2Subscription
	{
	public:

		std::map<ServiceSubscriptionClientID, ServiceInfo2Wrapped> GetDetectedServiceInfo2();

		void Close();

		void SetRRDirector(WrappedServiceInfo2SubscriptionDirector* director, int32_t id);

		boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> GetNode();

	};

	class WrappedServiceSubscriptionDirector
	{
	public:
		virtual void ClientConnected(const boost::shared_ptr<RobotRaconteur::WrappedServiceSubscription>& subscription, const ServiceSubscriptionClientID& id, const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& slient) = 0;
		virtual void ClientDisconnected(const boost::shared_ptr<RobotRaconteur::WrappedServiceSubscription>& subscription, const ServiceSubscriptionClientID& id, const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& slient) = 0;
		virtual void ClientConnectFailed(const boost::shared_ptr<RobotRaconteur::WrappedServiceSubscription>& subscription, const ServiceSubscriptionClientID& id, const std::vector<std::string>& url, HandlerErrorInfo& error) = 0;

		virtual ~WrappedServiceSubscriptionDirector() {}
	};

	class WrappedServiceSubscription_TryDefaultClientRes
	{
	public:
		bool res;
		boost::shared_ptr<RobotRaconteur::WrappedServiceStub> client;
	};

	class WrappedWireSubscription;
	class WrappedPipeSubscription;

	%nodefaultctor WrappedServiceSubscription;
	class WrappedServiceSubscription
	{
	public:

		std::map<ServiceSubscriptionClientID, boost::shared_ptr<WrappedServiceStub> > GetConnectedClients();

		void Close();

		void ClaimClient(const boost::shared_ptr<WrappedServiceStub>& client);
		void ReleaseClient(const boost::shared_ptr<WrappedServiceStub>& client);

		uint32_t GetConnectRetryDelay();
		void SetConnectRetryDelay(uint32_t delay_milliseconds);

		boost::shared_ptr<WrappedWireSubscription> SubscribeWire(const std::string& membername, const std::string& servicepath);

		boost::shared_ptr<WrappedPipeSubscription> SubscribePipe(const std::string& membername, const std::string& servicepath, int32_t max_recv_packets = -1);

		boost::shared_ptr<WrappedServiceStub> GetDefaultClient();

		WrappedServiceSubscription_TryDefaultClientRes TryGetDefaultClient();

		boost::shared_ptr<WrappedServiceStub> GetDefaultClientWait(int32_t timeout = -1);

		WrappedServiceSubscription_TryDefaultClientRes TryGetDefaultClientWait(int32_t timeout = -1);

		void AsyncGetDefaultClient(int32_t timeout, AsyncStubReturnDirector* handler, int32_t id);

		void SetRRDirector(WrappedServiceSubscriptionDirector* director, int32_t id);

		boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> GetNode();

		std::vector<std::string> GetServiceURL();

		void UpdateServiceURL(const std::vector<std::string>& url, const std::string& username = "", boost::intrusive_ptr<MessageElementData> credentials=boost::intrusive_ptr<MessageElementData>(),  const std::string& objecttype = "", bool close_connected = false);
		void UpdateServiceURL(const std::string& url, const std::string& username = "", boost::intrusive_ptr<MessageElementData> credentials=boost::intrusive_ptr<MessageElementData>(),  const std::string& objecttype = "", bool close_connected = false);
		void UpdateServiceByType(const std::vector<std::string>& service_types, const boost::shared_ptr<WrappedServiceSubscriptionFilter>& filter = boost::shared_ptr<WrappedServiceSubscriptionFilter>());

		boost::shared_ptr<RobotRaconteur::WrappedSubObjectSubscription> SubscribeSubObject(const std::string& service_path, const std::string& objecttype="");

	};

	class WrappedWireSubscriptionDirector
	{
	public:
		virtual void WireValueChanged(const boost::shared_ptr<RobotRaconteur::WrappedWireSubscription>& wire_subscription, WrappedService_typed_packet& value, const TimeSpec& time) = 0;

		virtual ~WrappedWireSubscriptionDirector() {}
	};


	%nodefaultctor WrappedWireSubscription;
	class WrappedWireSubscription
	{
	public:

		WrappedService_typed_packet GetInValue(TimeSpec* time = NULL);
		bool TryGetInValue(WrappedService_typed_packet& val, TimeSpec* time = NULL);

		bool WaitInValueValid(int32_t timeout = RR_TIMEOUT_INFINITE);

		bool GetIgnoreInValue();
		void SetIgnoreInValue(bool ignore);

		int32_t GetInValueLifespan();
		void SetInValueLifespan(int32_t millis);

		size_t GetActiveWireConnectionCount();

		void Close();

		void SetRRDirector(WrappedWireSubscriptionDirector* director, int32_t id);

		boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> GetNode();

	};

	class WrappedWireSubscription_send_iterator
	{
	public:
		WrappedWireSubscription_send_iterator(const boost::shared_ptr<WrappedWireSubscription>& sub);
		boost::shared_ptr<WrappedWireConnection> Next();
		void SetOutValue(const boost::intrusive_ptr<MessageElement>& value);
		boost::shared_ptr<TypeDefinition> GetType();
		boost::shared_ptr<WrappedServiceStub> GetStub();
		virtual ~WrappedWireSubscription_send_iterator();
	};

	class WrappedPipeSubscriptionDirector
	{
	public:
		virtual void PipePacketReceived(const boost::shared_ptr<RobotRaconteur::WrappedPipeSubscription>& pipe_subscription) = 0;

		virtual ~WrappedPipeSubscriptionDirector() {}
	};

	%nodefaultctor WrappedPipeSubscription;
	class WrappedPipeSubscription
	{
	public:

		WrappedService_typed_packet ReceivePacket();
		bool TryReceivePacket(WrappedService_typed_packet& packet);
		bool TryReceivePacketWait(WrappedService_typed_packet& packet, int32_t timeout = RR_TIMEOUT_INFINITE, bool peek = false);

		size_t Available();
		size_t GetActivePipeEndpointCount();

		bool GetIgnoreReceived();
		void SetIgnoreReceived(bool ignore);

		void Close();

		void SetRRDirector(WrappedPipeSubscriptionDirector* director, int32_t id);

		boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> GetNode();
	};

	class WrappedPipeSubscription_send_iterator
	{
	public:
		WrappedPipeSubscription_send_iterator(const boost::shared_ptr<WrappedPipeSubscription>& sub);
		boost::shared_ptr<WrappedPipeEndpoint> Next();
		void AsyncSendPacket(const boost::intrusive_ptr<MessageElement>& value);
		boost::shared_ptr<TypeDefinition> GetType();
		boost::shared_ptr<WrappedServiceStub> GetStub();
		virtual ~WrappedPipeSubscription_send_iterator();
	};

	%nodefaultctor WrappedSubObjectSubscription;
	class WrappedSubObjectSubscription
	{
	public:

		boost::shared_ptr<RobotRaconteur::WrappedServiceStub> GetDefaultClient();

		WrappedServiceSubscription_TryDefaultClientRes TryGetDefaultClient();

		boost::shared_ptr<RobotRaconteur::WrappedServiceStub> GetDefaultClientWait(int32_t timeout = -1);

		WrappedServiceSubscription_TryDefaultClientRes TryGetDefaultClientWait(int32_t timeout = -1);

		void AsyncGetDefaultClient(int32_t timeout, AsyncStubReturnDirector* handler, int32_t id);

		void Close();

		boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> GetNode();
	};

	std::vector<ServiceSubscriptionClientID> WrappedServiceInfo2SubscriptionServicesToVector(std::map<ServiceSubscriptionClientID, ServiceInfo2Wrapped >& infos);

	boost::shared_ptr<WrappedServiceInfo2Subscription> WrappedSubscribeServiceInfo2(const boost::shared_ptr<RobotRaconteurNode>& node, const std::vector<std::string>& service_types, boost::shared_ptr<WrappedServiceSubscriptionFilter> filter = boost::shared_ptr<WrappedServiceSubscriptionFilter>());

	std::vector<ServiceSubscriptionClientID> WrappedServiceSubscriptionClientsToVector(std::map<ServiceSubscriptionClientID, boost::shared_ptr<WrappedServiceStub> >& clients);

	boost::shared_ptr<WrappedServiceSubscription> WrappedSubscribeServiceByType(const boost::shared_ptr<RobotRaconteurNode>& node, const std::vector<std::string>& service_types, boost::shared_ptr<WrappedServiceSubscriptionFilter> filter = boost::shared_ptr<WrappedServiceSubscriptionFilter>());

	boost::shared_ptr<WrappedServiceSubscription> WrappedSubscribeService(const boost::shared_ptr<RobotRaconteurNode>& node, const std::vector<std::string>& url, const std::string& username = "", boost::intrusive_ptr<MessageElementData> credentials=boost::intrusive_ptr<MessageElementData>(),  const std::string& objecttype = "");

	boost::shared_ptr<WrappedServiceSubscription> WrappedSubscribeService(const boost::shared_ptr<RobotRaconteurNode>& node, const std::string& url, const std::string& username = "", boost::intrusive_ptr<MessageElementData> credentials=boost::intrusive_ptr<MessageElementData>(),  const std::string& objecttype = "");

	enum ServiceSubscriptionManager_CONNECTION_METHOD
	{
		ServiceSubscriptionManager_CONNECTION_METHOD_DEFAULT,
		ServiceSubscriptionManager_CONNECTION_METHOD_URL,
		ServiceSubscriptionManager_CONNECTION_METHOD_TYPE
	};

	struct WrappedServiceSubscriptionManagerDetails
	{
		std::string Name;
		ServiceSubscriptionManager_CONNECTION_METHOD ConnectionMethod;
		std::vector<std::string> Urls;
		std::string UrlUsername;
		boost::intrusive_ptr<MessageElementData> UrlCredentials;
		std::vector<std::string> ServiceTypes;
		boost::shared_ptr<WrappedServiceSubscriptionFilter> Filter;
		bool Enabled;

		WrappedServiceSubscriptionManagerDetails();

		WrappedServiceSubscriptionManagerDetails(
			const std::string& Name = "",
			ServiceSubscriptionManager_CONNECTION_METHOD Connection_method =
				ServiceSubscriptionManager_CONNECTION_METHOD_DEFAULT,
			const std::vector<std::string>& Urls = std::vector<std::string>(), const std::string& UrlUsername = "",
			const boost::intrusive_ptr<MessageElementData>& UrlCredentials =
				boost::intrusive_ptr<MessageElementData>(),
			const std::vector<std::string>& ServiceTypes = std::vector<std::string>(),
			const boost::shared_ptr<WrappedServiceSubscriptionFilter>& Filter = boost::shared_ptr<WrappedServiceSubscriptionFilter>(),
			bool Enabled = false);
	};

	class WrappedServiceSubscriptionManager
	{
	public:

		WrappedServiceSubscriptionManager();

		WrappedServiceSubscriptionManager(const boost::shared_ptr<RobotRaconteurNode>& node);

		WrappedServiceSubscriptionManager(const std::vector<WrappedServiceSubscriptionManagerDetails>& details);

		WrappedServiceSubscriptionManager(const std::vector<WrappedServiceSubscriptionManagerDetails>& details,
								const boost::shared_ptr<RobotRaconteurNode>& node);

		void AddSubscription(const WrappedServiceSubscriptionManagerDetails& details);

		void RemoveSubscription(const std::string& name, bool close = true);

		void EnableSubscription(const std::string& name);

		void DisableSubscription(const std::string& name, bool close = true);

		boost::shared_ptr<WrappedServiceSubscription> GetSubscription(const std::string& name, bool force_create = true);

		bool IsConnected(const std::string& name);

		bool IsEnabled(const std::string& name);

		void Close(bool close_subscriptions = true);

		std::vector<std::string> GetSubscriptionNames();

    	std::vector<WrappedServiceSubscriptionManagerDetails> GetSubscriptionDetails();

		boost::shared_ptr<RobotRaconteurNode> GetNode();
	};

}
