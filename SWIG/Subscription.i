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

//Subscriptions

%shared_ptr(RobotRaconteur::WrappedServiceSubscriptionFilterNode);
%shared_ptr(RobotRaconteur::WrappedServiceSubscriptionFilter);
%shared_ptr(RobotRaconteur::WrappedServiceInfo2Subscription);
%shared_ptr(RobotRaconteur::WrappedServiceSubscription);
%shared_ptr(RobotRaconteur::WrappedPipeSubscription);
%shared_ptr(RobotRaconteur::WrappedWireSubscription);

%rename (WrappedServiceSubscriptionClientID) ServiceSubscriptionClientID;

%template(map_subscriptionserviceinfo2) std::map<RobotRaconteur::ServiceSubscriptionClientID, RobotRaconteur::ServiceInfo2Wrapped>;
%template(map_subscriptionclients) std::map<RobotRaconteur::ServiceSubscriptionClientID, boost::shared_ptr<RobotRaconteur::WrappedServiceStub> >;
%template(vectorptr_wrappedservicesubscriptionnode) std::vector<boost::shared_ptr<RobotRaconteur::WrappedServiceSubscriptionFilterNode> >;
%template(vectorptr_subscriptionclientid) std::vector<RobotRaconteur::ServiceSubscriptionClientID>;

%feature("director") RobotRaconteur::WrappedServiceInfo2SubscriptionDirector;
%feature("director") RobotRaconteur::WrappedServiceSubscriptionDirector;
%feature("director") RobotRaconteur::WrappedServiceSubscriptionFilterPredicateDirector;
%feature("director") RobotRaconteur::WrappedWireSubscriptionDirector;
%feature("director") RobotRaconteur::WrappedPipeSubscriptionDirector;



namespace RobotRaconteur
{
	
	class TimeSpec;
	class ServiceInfo2;

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
		virtual void ServiceDetected(boost::shared_ptr<RobotRaconteur::WrappedServiceInfo2Subscription> subscription, const ServiceSubscriptionClientID& id, const ServiceInfo2Wrapped& service) {}
		virtual void ServiceLost(boost::shared_ptr<RobotRaconteur::WrappedServiceInfo2Subscription> subscription, const ServiceSubscriptionClientID& id, const ServiceInfo2Wrapped& service) {}

		virtual ~WrappedServiceInfo2SubscriptionDirector() {}
	};

	%nodefaultctor WrappedServiceInfo2Subscription;
	class WrappedServiceInfo2Subscription
	{
	public:		

		std::map<ServiceSubscriptionClientID, ServiceInfo2Wrapped> GetDetectedServiceInfo2();

		void Close();

		void SetRRDirector(WrappedServiceInfo2SubscriptionDirector* director, int32_t id);
			
	};
	
	class WrappedServiceSubscriptionDirector
	{
	public:
		virtual void ClientConnected(boost::shared_ptr<RobotRaconteur::WrappedServiceSubscription> subscription, const ServiceSubscriptionClientID& id, boost::shared_ptr<WrappedServiceStub> client) = 0;
		virtual void ClientDisconnected(boost::shared_ptr<RobotRaconteur::WrappedServiceSubscription> subscription, const ServiceSubscriptionClientID& id, boost::shared_ptr<WrappedServiceStub> client) = 0;
		
		virtual ~WrappedServiceSubscriptionDirector() {}
	};

	class WrappedWireSubscription;
	class WrappedPipeSubscription;

	%nodefaultctor WrappedServiceSubscription;
	class WrappedServiceSubscription
	{
	public:
			
		std::map<ServiceSubscriptionClientID, boost::shared_ptr<WrappedServiceStub> > GetConnectedClients();

		void Close();
		
		void ClaimClient(boost::shared_ptr<WrappedServiceStub> client);
		void ReleaseClient(boost::shared_ptr<WrappedServiceStub> client);

		uint32_t GetConnectRetryDelay();
		void SetConnectRetryDelay(uint32_t delay_milliseconds);

		boost::shared_ptr<WrappedWireSubscription> SubscribeWire(const std::string& membername);

		boost::shared_ptr<WrappedPipeSubscription> SubscribePipe(const std::string& membername, int32_t max_recv_packets = -1);

		void SetRRDirector(WrappedServiceSubscriptionDirector* director, int32_t id);
			
	};

	class WrappedService_typed_packet
	{
	public:
		boost::intrusive_ptr<RobotRaconteur::MessageElement> packet;
		boost::shared_ptr<RobotRaconteur::TypeDefinition> type;
		boost::shared_ptr<RobotRaconteur::WrappedServiceStub> stub;		
	};

	class WrappedWireSubscriptionDirector
	{
	public:
		virtual void WireValueChanged(boost::shared_ptr<RobotRaconteur::WrappedWireSubscription> wire_subscription, WrappedService_typed_packet& value, const TimeSpec& time) = 0;
		
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

		size_t GetActiveWireConnectionCount();

		void Close();

		void SetRRDirector(WrappedWireSubscriptionDirector* director, int32_t id);
	
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
		virtual void PipePacketReceived(boost::shared_ptr<RobotRaconteur::WrappedPipeSubscription> pipe_subscription) = 0;

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

	std::vector<ServiceSubscriptionClientID> WrappedServiceInfo2SubscriptionServicesToVector(std::map<ServiceSubscriptionClientID, ServiceInfo2Wrapped >& infos);

	boost::shared_ptr<WrappedServiceInfo2Subscription> WrappedSubscribeServiceInfo2(boost::shared_ptr<RobotRaconteurNode> node, const std::vector<std::string>& service_types, boost::shared_ptr<WrappedServiceSubscriptionFilter> filter = boost::shared_ptr<WrappedServiceSubscriptionFilter>());

	std::vector<ServiceSubscriptionClientID> WrappedServiceSubscriptionClientsToVector(std::map<ServiceSubscriptionClientID, boost::shared_ptr<WrappedServiceStub> >& clients);

	boost::shared_ptr<WrappedServiceSubscription> WrappedSubscribeService(boost::shared_ptr<RobotRaconteurNode> node, const std::vector<std::string>& service_types, boost::shared_ptr<WrappedServiceSubscriptionFilter> filter = boost::shared_ptr<WrappedServiceSubscriptionFilter>());

}